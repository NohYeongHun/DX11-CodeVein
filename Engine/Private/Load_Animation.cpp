#include "Load_Animation.h"

CLoad_Animation::CLoad_Animation()
{
}



HRESULT CLoad_Animation::Initialize(std::ifstream& ifs)
{
    if (FAILED(Load_Channels(ifs)))
        return E_FAIL;

    return S_OK;
}

void CLoad_Animation::Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    for (auto& pChannel : m_Channels)
        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
}

/*
    _float fDuration;
    _float fTickPerSecond;
    _float fCurrentTrackPostion;
    uint32_t iNumChannels; 
    vector<CHANNEL_INFO> Channels;
*/
HRESULT CLoad_Animation::Load_Channels(std::ifstream& ifs)
{
    ANIMATION_INFO info = {};

    ifs.read(reinterpret_cast<char*>(&info.fDuration), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fTickPerSecond), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fCurrentTrackPostion), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.iNumChannels), sizeof(uint32_t));
    
    m_fDuration = info.fDuration;
    m_fTickPerSecond = info.fTickPerSecond;
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

    return S_OK;
}

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

void CLoad_Animation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();
}
