#include "Load_Channel.h"

CLoad_Channel::CLoad_Channel()
{
}

/*
    string channelName;
    uint32_t iBoneIndex;
    uint32_t iNumKeyFrames;
    vector<KEYFRAME> KeyFrames;
*/
HRESULT CLoad_Channel::Initialize(std::ifstream& ifs)
{
    if (FAILED(Load_Channel(ifs)))
    {
        CRASH("Failed Load Channel");
        return E_FAIL;
    }

    return S_OK;
}

void CLoad_Channel::Update_TransformationMatrix(const vector<class CLoad_Bone*>& Bones, _float fCurrentTrackPosition)
{
    if (fCurrentTrackPosition == 0.f)
        m_iCurrentKeyFrameIndex = 0;

    /* 선택된 애니메이션이 이용하고 있는 이 뼈(Channel)의 현재 재생된 위치(fCurrrentTrackPosition)에 맞는 상태행렬을 만들어 준다. */
    _vector         vScale, vRotation, vTranslation;

    /* 마지막 키프레임상태를 취하낟. */
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }

    /* 양쪽 키프레임사이에서의 중간상태를 보간하여 만든다. */
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
            ++m_iCurrentKeyFrameIndex;

        _vector    vSourScale, vDestScale;
        _vector    vSourRotation, vDestRotation;
        _vector    vSourTranslation, vDestTranslation;

        vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
        vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vTranslation), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
        vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vTranslation), 1.f);

        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vSourTranslation, vDestTranslation, fRatio), 1.f);


    }

    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    _matrix         TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

/*
    string channelName;
    uint32_t iBoneIndex;
    uint32_t iNumKeyFrames;
    vector<KEYFRAME> KeyFrames;
*/
HRESULT CLoad_Channel::Load_Channel(std::ifstream& ifs)
{
    CHANNEL_INFO info = {};

    // 1. 멤버변수 초기화.
    info.channelName = ReadString(ifs);
    ifs.read(reinterpret_cast<char*>(&info.iBoneIndex), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&info.iNumKeyFrames), sizeof(uint32_t));
    
    strcpy_s(m_szName, info.channelName.c_str());
    m_iBoneIndex = info.iBoneIndex;
    m_iNumKeyFrames = info.iNumKeyFrames;

    /* 3. Vector 사이즈 설정*/
    info.KeyFrames.resize(info.iNumKeyFrames);

    if (!ReadBytes(ifs, info.KeyFrames.data(), sizeof(KEYFRAME) * info.iNumKeyFrames))
    {
        CRASH("READ KEYFRAME FAILED");
        return E_FAIL;
    }

    // 4. Vector 이동.
    m_KeyFrames = move(info.KeyFrames);
    return S_OK;
}

CLoad_Channel* CLoad_Channel::Create(std::ifstream& ifs)
{
    CLoad_Channel* pInstance = new CLoad_Channel();
    if (FAILED(pInstance->Initialize(ifs)))
    {
        MSG_BOX(TEXT("Create CLoad_Channel : Failed"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLoad_Channel::Free()
{
    __super::Free();
    m_KeyFrames.clear();
}
