#include "Tool_Channel.h"

CTool_Channel::CTool_Channel()
{
}

HRESULT CTool_Channel::Initialize(const aiNodeAnim* pAIChannel, const vector<CTool_Bone*>& Bones)
{
   

    auto	iter = find_if(Bones.begin(), Bones.end(), [&](CTool_Bone* pBone)->_bool
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
        ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

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

void CTool_Channel::Update_TransformationMatrix(const vector<class CTool_Bone*>& Bones, _float fCurrentTrackPosition)
{
    //_vector vScale, vRotation, vTranslation;

    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    //_matrix TransformationMatrix = XMMatrixAffineTransformation();

    //Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

}

/*
    string channelName; 
    _uint iBoneIndex;
    _uint iNumKeyFrames;
    vector<KEYFRAME> KeyFrames;
*/
// 9. Chanel 정보 저장.
void CTool_Channel::Save_Channel(CHANNEL_INFO& channelInfo)
{
    channelInfo.channelName = m_szName;
    channelInfo.iBoneIndex = m_iBoneIndex;
    channelInfo.iNumKeyFrames = m_iNumKeyFrames;
    channelInfo.KeyFrames = m_KeyFrames; // vector 깊은 복사.
}

CTool_Channel* CTool_Channel::Create(const aiNodeAnim* pAIChannel, const vector<class CTool_Bone*>& Bones)
{
    CTool_Channel* pInstance = new CTool_Channel();
    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
    {
        MSG_BOX(TEXT("Create Channel : Failed"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTool_Channel::Free()
{
    __super::Free();
    m_KeyFrames.clear();

}
