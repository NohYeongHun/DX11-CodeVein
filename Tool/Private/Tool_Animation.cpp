#include "Animation.h"
#include "Tool_Animation.h"

CTool_Animation::CTool_Animation()
{
}

CTool_Animation::CTool_Animation(const CTool_Animation& Prototype)
    : m_fTickPerSecond{ Prototype.m_fTickPerSecond }
    , m_fDuration{ Prototype.m_fDuration }
    , m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
    , m_iNumChannels{ Prototype.m_iNumChannels }
    , m_Channels{ Prototype.m_Channels }
    , m_ChannelCurrentKeyFrameIndices{ Prototype.m_ChannelCurrentKeyFrameIndices }
{
    strcpy_s(m_szName, Prototype.m_szName);

    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}



HRESULT CTool_Animation::Initialize(const aiAnimation* pAIAnimation, const vector<CTool_Bone*>& Bones)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);
    string str = m_szName;
    _wstring wstr = L"Animation Name: " + _wstring(str.begin(), str.end()) + L"\n";
    OutputDebugString(wstr.c_str());

    m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
    m_fDuration = static_cast<_float>(pAIAnimation->mDuration);

    m_iNumChannels = pAIAnimation->mNumChannels;

    m_ChannelCurrentKeyFrameIndices.resize(m_iNumChannels);
    
    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        //_wstring strChannelDebug = L"============= Current Channel Index : " + to_wstring(i) + L" ========================\n";
        //OutputDebugString(strChannelDebug.c_str());
        CTool_Channel* pChannel = CTool_Channel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
        {
            CRASH("CHANNEL CRASH");
            return E_FAIL;
        }

        m_Channels.emplace_back(pChannel);
    }
    

    return S_OK;
}

/* Animation에서 Transform 행렬들을 모두 업데이트해줍니다. */
void CTool_Animation::Update_TransformationMatrices(const vector<class CTool_Bone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{
    /* 현재 재생위치를 계산하자. */
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;
            m_fCurrentTrackPosition = m_fDuration;
            return;
        }
        else
            m_fCurrentTrackPosition = 0.f;
    }

    for (auto& pChannel : m_Channels)
        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
}

/* 저장 로직. 
    _float fDuration;
    _float fTickPerSecond;
    _float fCurrentTrackPostion;
    _uint iNumChannels;
    vector<CHANNEL_INFO> Channels;
*/
// 5. Animation 정보 구조체 단위로 저장.
void CTool_Animation::Save_Animation(ANIMATION_INFO& animInfo)
{
    // 6. 매개 변수들 저장.
    animInfo.strAnimName = m_szName;
    animInfo.fDuration = m_fDuration;
    animInfo.fTickPerSecond = m_fTickPerSecond;
    animInfo.fCurrentTrackPostion = m_fCurrentTrackPosition;
    animInfo.iNumChannels = m_iNumChannels;

    // 7. Channel 벡터 크기지정.
    animInfo.Channels.reserve(animInfo.iNumChannels);

    vector<CHANNEL_INFO>& channels = animInfo.Channels;
    // 8. Channel 벡터 순회하면서 Channel 정보 저장.
    for (_uint i = 0; i < animInfo.iNumChannels; i++)
    {
        CHANNEL_INFO channelInfo = {};
        m_Channels[i]->Save_Channel(channelInfo);

        // Channel Info를 채워서 넣어줍니다.
        channels.emplace_back(channelInfo);
    }

    // 9. 최근 KeyFrame Indices 저장.
    animInfo.iNumKeyFrameIndices = m_ChannelCurrentKeyFrameIndices.size();
    animInfo.CurrentKeyFrameIndices = m_ChannelCurrentKeyFrameIndices;
}

CTool_Animation* CTool_Animation::Create(const aiAnimation* pAIAnimation, const vector<CTool_Bone*>& Bones)
{
    CTool_Animation* pInstance = new CTool_Animation();
    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX(TEXT("Create Failed : Animation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTool_Animation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();

    m_ChannelCurrentKeyFrameIndices.clear();

}
