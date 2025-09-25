#pragma once

NS_BEGIN(Engine)
class CSoundManager : public CBase
{
public:
    explicit CSoundManager();
    virtual ~CSoundManager() = default;
    HRESULT PriorityUpdate(_float fTimeDelta);
    HRESULT Update(_float fTimeDelta);
    HRESULT LateUpdate(_float fTimeDelta);
    HRESULT Initialize();

#pragma region ENGINE에서 사용.
    void PlaySoundEffect(wstring pSoundKey, _float fVolume);
    void PlayBGM(wstring pSoundKey, _float fVolume, _bool bloop);
    void StopBGM();
    void StopSound();
    void StopAll();
    void SetChannelVolume(_float fVolume);
#pragma endregion

    

private:
    void LoadSoundFile();

    // 사운드 리소스 정보를 갖는 객체 
    map<std::wstring, FMOD::Sound*> m_mapSound;

    // FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
    // FMOD::Channel* m_pChannelArr[64] = {nullptr};
    FMOD::Channel* bgm_channel = {nullptr};
    FMOD::ChannelGroup* bgm_group = {nullptr};
    FMOD::Channel* move_sound = {nullptr};
    // 사운드 ,채널 객체 및 장치를 관리하는 객체 
    FMOD::System* m_pSystem = {nullptr};

public:
    static CSoundManager* Create();
    virtual void Free() override;
};
NS_END