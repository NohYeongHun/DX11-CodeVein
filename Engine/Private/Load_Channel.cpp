#include "Load_Channel.h"

CLoad_Channel::CLoad_Channel()
{
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

void CLoad_Channel::Update_TransformationMatrix(const vector<class CLoad_Bone*>& Bones, _float fCurrentTrackPosition)
{
}

/*
    string channelName;
    uint32_t iBoneIndex;
    uint32_t iNumKeyFrames;
    vector<KEYFRAME> KeyFrames;
*/
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
