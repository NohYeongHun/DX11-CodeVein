#include "Load_Channel.h"

CLoad_Channel::CLoad_Channel()
{
}

_float Lerp(_float a, _float b, _float t)
{
    return a + (b - a) * t;
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


void CLoad_Channel::Update_TransformationMatrix(const vector<class CLoad_Bone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{

    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyFrameIndex = 0;

    // 나머지는 기존 코드와 동일
    _vector vScale, vRotation, vTranslation;

    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
            ++*pCurrentKeyFrameIndex;

        _vector vSourScale, vDestScale;
        _vector vSourRotation, vDestRotation;
        _vector vSourTranslation, vDestTranslation;

        vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
        vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);


        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vSourTranslation, vDestTranslation, fRatio), 1.f);
    }

    // 이 값을 가져와야한다?
    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}




_matrix CLoad_Channel::Get_TransformMatrixAtTime(_float fCurrentTrackPosition)
{
    // KeyFrame이 없는 채널의 경우?
    if (m_KeyFrames.empty())
        return XMMatrixIdentity();

    // 마지막 프레임 넘은 경우: 마지막 키프레임 값 사용
    if (fCurrentTrackPosition >= m_KeyFrames.back().fTrackPosition)
    {
        const auto& key = m_KeyFrames.back();
        return XMMatrixAffineTransformation(
            XMLoadFloat3(&key.vScale),
            XMVectorZero(),
            XMLoadFloat4(&key.vRotation),
            XMLoadFloat3(&key.vTranslation));
    }

    for (_uint i = 0; i < m_KeyFrames.size() - 1; ++i)
    {
        if (fCurrentTrackPosition < m_KeyFrames[i + 1].fTrackPosition)
        {
            // 보간할 키프레임 찾기.
            const auto& key0 = m_KeyFrames[i];
            const auto& key1 = m_KeyFrames[i + 1];

            _float delta = key1.fTrackPosition - key0.fTrackPosition;
            // 보간 Ratio
            _float t = (delta == 0.f) ? 0.f : (fCurrentTrackPosition - key0.fTrackPosition) / delta;

            _vector vScale = XMVectorLerp(XMLoadFloat3(&key0.vScale), XMLoadFloat3(&key1.vScale), t);

            _vector vRot = XMQuaternionSlerp(XMLoadFloat4(&key0.vRotation), XMLoadFloat4(&key1.vRotation), t);

            _vector vTrans = XMVectorLerp(XMLoadFloat3(&key0.vTranslation), XMLoadFloat3(&key1.vTranslation), t);

            return XMMatrixAffineTransformation(vScale, XMVectorZero(), vRot, vTrans);
        }
    }

    return XMMatrixIdentity(); // 예외 상황

}

// 현재 애니메이션의 현재 시간에맞는 키프레임 추출.
KEYFRAME CLoad_Channel::Get_KeyFrameAtTime(_float fTime)
{
    if (m_KeyFrames.empty())
        return KEYFRAME(); // 기본 키프레임 반환

    // 시간이 첫 번째 키프레임보다 이전이면
    if (fTime <= m_KeyFrames[0].fTrackPosition)
        return m_KeyFrames[0];

    // 시간이 마지막 키프레임보다 이후면
    if (fTime >= m_KeyFrames.back().fTrackPosition)
        return m_KeyFrames.back();

    // 현재 시간에 맞는 키프레임 찾기
    for (_uint i = 0; i < m_KeyFrames.size() - 1; ++i)
    {
        if (fTime >= m_KeyFrames[i].fTrackPosition && fTime <= m_KeyFrames[i + 1].fTrackPosition)
        {
            // 두 키프레임 사이의 보간
            _float t = (fTime - m_KeyFrames[i].fTrackPosition) /
                (m_KeyFrames[i + 1].fTrackPosition - m_KeyFrames[i].fTrackPosition);

            KEYFRAME result;
            result.fTrackPosition = fTime;

            // 위치 보간 (선형)
            result.vTranslation.x = Lerp(m_KeyFrames[i].vTranslation.x, m_KeyFrames[i + 1].vTranslation.x, t);
            result.vTranslation.y = Lerp(m_KeyFrames[i].vTranslation.y, m_KeyFrames[i + 1].vTranslation.y, t);
            result.vTranslation.z = Lerp(m_KeyFrames[i].vTranslation.z, m_KeyFrames[i + 1].vTranslation.z, t);

            // 스케일 보간 (선형)
            result.vScale.x = Lerp(m_KeyFrames[i].vScale.x, m_KeyFrames[i + 1].vScale.x, t);
            result.vScale.y = Lerp(m_KeyFrames[i].vScale.y, m_KeyFrames[i + 1].vScale.y, t);
            result.vScale.z = Lerp(m_KeyFrames[i].vScale.z, m_KeyFrames[i + 1].vScale.z, t);

            // 회전 보간 (구면 선형 보간)
            _vector q1 = XMLoadFloat4(&m_KeyFrames[i].vRotation);
            _vector q2 = XMLoadFloat4(&m_KeyFrames[i + 1].vRotation);
            _vector qResult = XMQuaternionSlerp(q1, q2, t);
            XMStoreFloat4(&result.vRotation, qResult);

            return result;
        }
    }

    return m_KeyFrames[0]; // 기본값
}

_vector CLoad_Channel::QuaternionSlerpShortest(_vector q1, _vector q2, _float t)
{
    // 내적 계산
    _float dot = XMVectorGetX(XMQuaternionDot(q1, q2));

    // 최단 경로 선택
    if (dot < 0.0f)
    {
        q2 = XMVectorNegate(q2);
        dot = -dot;
    }

    // 거의 같은 방향이면 선형 보간
    if (dot > 0.9995f)
    {
        _vector result = XMVectorLerp(q1, q2, t);
        return XMQuaternionNormalize(result);
    }

    // 구면 선형 보간
    return XMQuaternionSlerp(q1, q2, t);
}

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
