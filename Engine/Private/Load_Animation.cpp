#include "Load_Animation.h"

CLoad_Animation::CLoad_Animation()
{
}

CLoad_Animation::CLoad_Animation(const CLoad_Animation& Prototype)
    : m_fTickPerSecond{ Prototype.m_fTickPerSecond }
    , m_fDuration{ Prototype.m_fDuration }
    , m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
    , m_iNumChannels{ Prototype.m_iNumChannels }
    , m_Channels{ Prototype.m_Channels }
    , m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
    , m_BoneChannelCache { Prototype.m_BoneChannelCache }
{
    strcpy_s(m_szName, Prototype.m_szName);
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);


}


HRESULT CLoad_Animation::Initialize(std::ifstream& ifs)
{
    if (FAILED(Load_Channels(ifs)))
        return E_FAIL;

    return S_OK;
}

void CLoad_Animation::Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _bool isLoop, _bool* pFinished, BLEND_DESC& blendDesc, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (blendDesc.isBlending)
    {
        Blend_Update_TransformationMatrices(Bones, isLoop, pFinished, blendDesc, fTimeDelta);
        return;
    }

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;
            m_fCurrentTrackPosition = m_fDuration;
            return;
        }
        else
        {
            *pFinished = true;
            m_fCurrentTrackPosition = 0.f;
        }
    }

    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);
    }
}

void CLoad_Animation::Blend_Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _bool isLoop, _bool* pFinished, BLEND_DESC& blendDesc, _float fTimeDelta)
{
    m_fBlendTrackPosition += fTimeDelta;

    // 시간 따로 계산.
    _float fRatio = m_fBlendTrackPosition / blendDesc.fBlendDuration;
    fRatio = min(fRatio, 1.f);

    // Blending 종료. => 한번에 되버리잖아?
    if (m_fBlendTrackPosition >= blendDesc.fBlendDuration)
    {
        blendDesc.isBlending = false;
        m_RootMotionTranslate = true;
    }
    else
    {
        // 1. 일단 블렌딩 이전 애니메이션 Channel들을 가져오자.
        if (nullptr == blendDesc.pLoad_Animation)
            CRASH("Failed Blend Load Animation");

        // 나는 시간을 알고있어.
        _float fPrevAnimTime = blendDesc.fPrevAnimTime;

        // 2. 복사 해서 쓰는 것이 아닌 참조로
        const vector<CLoad_Channel*>& prevAnimChannels = blendDesc.pLoad_Animation->Get_Channels();

        _vector vScale, vRotation, vTranslation;

        // 3. AnimChannels 로부터. KeyFrame 정보들을 가져옵니다.
        for (_uint i = 0; i < m_iNumChannels; ++i)
        {
            // Prev TransformMatirx Channel.
            KEYFRAME prevKeyFrame = prevAnimChannels[i]->Get_KeyFrameAtTime(blendDesc.fPrevAnimTime);
            KEYFRAME curKeyFrame = m_Channels[i]->Get_KeyFrameAtTime(m_fCurrentTrackPosition);


            _vector vSourScale, vDestScale;
            _vector vSourRotation, vDestRotation;
            _vector vSourTranslation, vDestTranslation;

            vSourScale = XMLoadFloat3(&prevKeyFrame.vScale);
            vSourRotation = XMLoadFloat4(&prevKeyFrame.vRotation);
            vSourTranslation = XMLoadFloat3(&prevKeyFrame.vTranslation);

            vDestScale = XMLoadFloat3(&curKeyFrame.vScale);
            vDestRotation = XMLoadFloat4(&curKeyFrame.vRotation);
            vDestTranslation = XMLoadFloat3(&curKeyFrame.vTranslation);

            vScale = blendDesc.bScale ? XMVectorLerp(vSourScale, vDestScale, fRatio) : vDestScale;
            vRotation = blendDesc.bRotate ? XMVectorLerp(vSourRotation, vDestRotation, fRatio) : vDestRotation;
            vTranslation = blendDesc.bTranslate ? XMVectorSetW(XMVectorLerp(vSourTranslation, vDestTranslation, fRatio), 1.f) : vDestTranslation;

            _matrix blendedMatrix = XMMatrixAffineTransformation(vScale, XMVectorZero(), vRotation, vTranslation);
            Bones[m_Channels[i]->Get_BoneIndex()]->Set_TransformationMatrix(blendedMatrix);
        }
    }
}

// "그 시간에서의 최종 Bone 변환 행렬이 뭐야?" 라는 질문
// → 즉, 해당 시간의 보간된 결과.
_matrix CLoad_Animation::Get_BoneMatrixAtTime(_uint iBoneIndex, _float fCurrentTrackPosition)
{
    // 해당 본의 채널을 찾기
    CLoad_Channel* pChannel = Find_Channel(iBoneIndex);
    if (!pChannel)
    {
        // 채널이 없으면 단위 행렬 반환
        return XMMatrixIdentity();
    }


    if (iBoneIndex >= m_BoneChannelCache.size() || m_BoneChannelCache[iBoneIndex] == nullptr)
        return XMMatrixIdentity(); // 해당 뼈에 대한 애니메이션 없음

    return m_BoneChannelCache[iBoneIndex]->Get_TransformMatrixAtTime(fCurrentTrackPosition);
}

//

void CLoad_Animation::Build_BoneChannelCache(_uint iNumBones)
{
    m_BoneChannelCache.clear();
    m_BoneChannelCache.resize(iNumBones, nullptr);

    for (auto& pChannel : m_Channels)
    {
        _uint idx = pChannel->Get_BoneIndex();
        if (idx < m_BoneChannelCache.size())
            m_BoneChannelCache[idx] = pChannel;
        else
        {
            CRASH("Get Failed Bone Index");
            return;
        }
    }
}

void CLoad_Animation::Reset()
{
    m_fBlendTrackPosition = 0.f;
    m_fCurrentTrackPosition = 0.f;

    for (_uint i = 0; i < m_CurrentKeyFrameIndices.size(); ++i)
        m_CurrentKeyFrameIndices[i] = 0;
}

void CLoad_Animation::Update_TrackPosition(_float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
        m_fCurrentTrackPosition = m_fDuration;


}

CLoad_Channel* CLoad_Animation::Find_Channel(_uint iBoneIndex)
{
    return m_BoneChannelCache[iBoneIndex];
}

_uint CLoad_Animation::Get_CurrentKeyFrame()
{
    return _uint();
}


   
HRESULT CLoad_Animation::Load_Channels(std::ifstream& ifs)
{
    ANIMATION_INFO info = {};

    string strAnimName = ReadString(ifs);
    ifs.read(reinterpret_cast<char*>(&info.fDuration), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fTickPerSecond), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fCurrentTrackPostion), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.iNumChannels), sizeof(uint32_t));
    
    strcpy_s(m_szName, strAnimName.c_str()); // 이름 복사.
    m_fDuration = info.fDuration;
    m_fTickPerSecond = info.fTickPerSecond * 1.5f;
    m_fCurrentTrackPosition = info.fCurrentTrackPostion;
    m_iNumChannels = info.iNumChannels;

    m_Channels.reserve(m_iNumChannels);
    
    // Channels Load
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        CLoad_Channel* pLoad_Channel = CLoad_Channel::Create(ifs);
        if (nullptr == pLoad_Channel)
            CRASH("Load Channel Failed");

        m_Channels.emplace_back(pLoad_Channel);
    }

    // Current KeyFrame Index Load
    ifs.read(reinterpret_cast<char*>(&info.iNumKeyFrameIndices), sizeof(uint32_t));
    
    info.CurrentKeyFrameIndices.resize(info.iNumKeyFrameIndices);
    if (!ReadBytes(ifs, info.CurrentKeyFrameIndices.data(), sizeof(uint32_t) * info.iNumKeyFrameIndices))
        CRASH("READ INDICIES FAILED");

    // ... 채널 로딩 완료 후
    


    // 최근 키프레임 정보 저장.
    m_CurrentKeyFrameIndices = move(info.CurrentKeyFrameIndices);

    return S_OK;
}

// Bone 캐쉬용 개수 전달.
CLoad_Animation* CLoad_Animation::Create(std::ifstream& ifs)
{
    CLoad_Animation* pInstance = new CLoad_Animation();
    if (FAILED(pInstance->Initialize(ifs)))
    {
        MSG_BOX(TEXT("Create Failed : Load_Animation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CLoad_Animation* CLoad_Animation::Clone()
{
    return new CLoad_Animation(*this);
}

void CLoad_Animation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();

    m_CurrentKeyFrameIndices.clear();
}
