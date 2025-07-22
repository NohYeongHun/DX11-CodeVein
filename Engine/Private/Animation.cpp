#include "Animation.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
    m_iNumChannels = pAIAnimation->mNumChannels;
    
    
    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
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
void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    for (auto& pChannel : m_Channels)
        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
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

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();

}
