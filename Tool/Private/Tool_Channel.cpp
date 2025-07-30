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
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3 vScale{};
    _float4 vRotation{};
    _float3 vTranslation{};
    
    // KeyFrame 값 채워줍니다.
    for (_uint i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            KeyFrame{};

        if (i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

     /*   _wstring strKeyFrameDebug = L"============= Current KeyFrame : " + to_wstring(i) + L" ========================\n";
        strKeyFrameDebug += L"vTranslation : " + to_wstring(vTranslation.x) + L" , " + to_wstring(vTranslation.y)
            + L" , " + to_wstring(vTranslation.z) + L"\n"
            + L"vScale : " + to_wstring(vScale.x) + L" , " + to_wstring(vScale.y)
            + L" , " + to_wstring(vScale.z) + L"\n"
            + L"vRotation : " + to_wstring(vRotation.x) + L" , " + to_wstring(vRotation.y)
            + L" , " + to_wstring(vRotation.z) + L"\n";*/

        //OutputDebugString(strKeyFrameDebug.c_str());

        m_KeyFrames.emplace_back(KeyFrame);
    }

    return S_OK;
}

void CTool_Channel::Update_TransformationMatrix(const vector<class CTool_Bone*>& Bones, _float fCurrentTrackPosition)
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
