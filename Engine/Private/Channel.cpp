#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
        {
            if (true == pBone->Compare_Name(pAIChannel->mNodeName.data))
                return true;

            m_iBoneIndex++;

            return false;
        });

    // KeyFrames에는 가장 큰 KeyFrame값만 저장.
    m_iNumKeyFrames = max(pAIChannel->mNumRotationKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3 vScale{};
    _float4 vRotation{};
    _float3 vTranslation{};
    
    // KeyFrame 값 채워줍니다.
    for (_uint i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME KeyFrame{};

        if (i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime; // 몇초에 어떤 상태인지를 나타내기 위함.
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            // Assimp에는 Queternion 구조체로 들어가있으므로 값을 일일히 넣어주어야 합니다.
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime; // 몇초에 어떤 상태인지를 나타내기 위함.
        }
        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i], sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime; // 몇초에 어떤 상태인지를 나타내기 위함.
        }
        
       
        /* 없는 경우 기존에 있는 값으로 채워집니다. => 동작이 고정된 상태. */
        KeyFrame.vScale = vScale; 
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.emplace_back(KeyFrame);
    }

    return S_OK;
}

void CChannel::Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
    if (0.f == fCurrentTrackPosition)
    {
        (*pCurrentKeyFrameIndex) = 0;
    }

    KEYFRAME		LastKeyFrame = m_KeyFrames.back();

    _vector			vScale, vRotation, vTranslation;

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        /*마지막 모션을 취한다. */
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);

        (*pCurrentKeyFrameIndex) = m_iNumKeyFrames - 1;
    }
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        _float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
            (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

        /* 선형보간한다. */
        _vector		vLeftScale, vRightScale;
        _vector		vLeftRotation, vRightRotation;
        _vector		vLeftTranslation, vRightTranslation;

        vLeftScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
        vLeftRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
        vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);

        vRightScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);
        vRightRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);
        vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);

        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio), 1.f);
    }

    _matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();
    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
    {
        MSG_BOX(TEXT("Create Channel : Failed"));
        Safe_Release(pInstance);
    }
    return pInstance;
}


void CChannel::Free()
{
    CBase::Free();
    m_KeyFrames.clear();

}
