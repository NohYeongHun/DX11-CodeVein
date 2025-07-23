#include "Load_Animation.h"

CLoad_Animation::CLoad_Animation()
{
}

CLoad_Animation::CLoad_Animation(const CLoad_Animation& Prototype)
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



HRESULT CLoad_Animation::Initialize(std::ifstream& ifs)
{
    if (FAILED(Load_Channels(ifs)))
        return E_FAIL;

    return S_OK;
}

/* 뼈들의 m_TransformationMatrix를 애니메이터분들이 제공해준 시간에 맞는 뼈의 상태로 갱신해준다. */
void CLoad_Animation::Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
  
    cout << "CurrentTrack Position : " <<  m_fCurrentTrackPosition << " | Duration : " << m_fDuration << endl;
    
    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        m_fCurrentTrackPosition = 0.f;
    }
        

    for (auto& pChannel : m_Channels)
    {
        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
    }
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

    string strAnimName = ReadString(ifs);
    ifs.read(reinterpret_cast<char*>(&info.fDuration), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fTickPerSecond), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.fCurrentTrackPostion), sizeof(_float));
    ifs.read(reinterpret_cast<char*>(&info.iNumChannels), sizeof(uint32_t));
    
    strcpy_s(m_szName, strAnimName.c_str()); // 이름 복사.
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

    // Current KeyFrame Index Load
    ifs.read(reinterpret_cast<char*>(&info.iNumKeyFrameIndices), sizeof(uint32_t));
    
    info.CurrentKeyFrameIndices.resize(info.iNumKeyFrameIndices);
    if (!ReadBytes(ifs, info.CurrentKeyFrameIndices.data(), sizeof(uint32_t) * info.iNumKeyFrameIndices))
        CRASH("READ INDICIES FAILED");

    // 최근 키프레임 정보 저장.
    m_ChannelCurrentKeyFrameIndices = move(info.CurrentKeyFrameIndices);

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

CLoad_Animation* CLoad_Animation::Clone()
{
    return new CLoad_Animation(*this);
}

void CLoad_Animation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);
    m_Channels.clear();

    m_ChannelCurrentKeyFrameIndices.clear();
}
