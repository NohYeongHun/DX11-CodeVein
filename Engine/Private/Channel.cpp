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

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition)
{
    //_vector vScale, vRotation, vTranslation;

    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    //_matrix TransformationMatrix = XMMatrixAffineTransformation();

    //Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

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
    __super::Free();
    m_KeyFrames.clear();

}
