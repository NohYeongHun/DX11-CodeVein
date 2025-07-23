#include "Animation.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
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

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);

    m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
    m_fDuration = static_cast<_float>(pAIAnimation->mDuration);

    m_iNumChannels = pAIAnimation->mNumChannels;

    m_ChannelCurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }
    

    return S_OK;
}

/* Animation에서 Transform 행렬들을 모두 업데이트해줍니다. */
_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
    _bool		isFinished = { false };

    /* 현재 재생위치를 계산하자. */
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    // 애니메이션 진행시간이 끝났다면?
    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
            isFinished = true;
        else
        {
            // 현재 시간을 0초로 돌립니다.
            m_fCurrentTrackPosition = 0.f;
        }
    }

    _uint		iChannelIndex = { 0 };

    // 뼈들의 행렬을 변경해줍니다.
    for (auto& pChannel : m_Channels)
    {
        pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones, &m_ChannelCurrentKeyFrameIndices[iChannelIndex++]);
    }

    return isFinished;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();
    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX(TEXT("Create Failed : Animation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}


void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();

}
