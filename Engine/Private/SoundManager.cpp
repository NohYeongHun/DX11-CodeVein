
CSoundManager::CSoundManager()
{
}

HRESULT CSoundManager::PriorityUpdate(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CSoundManager::Update(_float fTimeDelta)
{
    m_pSystem->update();
    return S_OK;
}

HRESULT CSoundManager::LateUpdate(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CSoundManager::Initialize()
{
    // 사운드를 담당하는 대표객체를 생성하는 함수
    FMOD::System_Create(&m_pSystem);
    // 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
    m_pSystem->init(64, FMOD_INIT_NORMAL, nullptr);

    LoadSoundFile();
    return S_OK;
}

void CSoundManager::PlaySoundEffect(wstring pSoundKey, _float fVolume)
{
    auto iter = m_mapSound.find(pSoundKey);
    if (iter == m_mapSound.end()) return;
    iter->second->setMode(FMOD_LOOP_OFF);
    iter->second->setLoopCount(0);

    FMOD::Channel* channel = nullptr;
    FMOD_RESULT res = m_pSystem->playSound(iter->second, nullptr, false, &channel);

    if (res != FMOD_OK)
    {
        CRASH("ERROR");
    }

    if (channel)
    {
        channel->setVolume(fVolume);
    }
}

void CSoundManager::PlayBGM(wstring pSoundKey, _float fVolume, _bool bloop)
{
    auto iter = m_mapSound.find(pSoundKey);
    if (iter == m_mapSound.end())
        CRASH("Sound Not Found");

    FMOD::Sound* sound = iter->second;
    // Sound가 loop 모드로 설정되어 있는지 보장 (초기 로드 시 설정 권장)
    if (bloop)
    {
        sound->setMode(FMOD_LOOP_NORMAL);
        sound->setLoopCount(-1); // 무한 루프
    }
    else
    {
        sound->setMode(FMOD_LOOP_OFF);
        sound->setLoopCount(0); // 루프 없음
    }

    if (bgm_channel)
    {
        bool playing = false;
        bgm_channel->isPlaying(&playing);
        if (playing)
        {
            bgm_channel->stop(); // 또는 페이드아웃 로직
            bgm_channel = nullptr;
        }
    }

    FMOD_RESULT res = m_pSystem->playSound(sound, bgm_group, false, &bgm_channel);
    if (res == FMOD_OK && bgm_channel)
    {
        bgm_channel->setVolume(fVolume);
    }
}

void CSoundManager::StopBGM()
{
    if (bgm_channel)
    {
        bool playing = false;
        bgm_channel->isPlaying(&playing);
        if (playing)
        {
            bgm_channel->stop(); // 또는 페이드아웃 로직
            bgm_channel = nullptr;
        }
    }
}

void CSoundManager::StopSound()
{
}



void CSoundManager::StopAll()
{
}

void CSoundManager::SetChannelVolume(float fVolume)
{
}


void CSoundManager::LoadSoundFile()
{
    _finddatai64_t fd;
    intptr_t handle = _findfirsti64("../Bin/Resources/Sound/*.*", &fd);

    if (handle == -1)
        return;

    do
    {
        std::string fullPath = "../Bin/Resources/Sound/" + std::string(fd.name);

        FMOD::Sound* pSound = nullptr;
        FMOD_RESULT eRes = m_pSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_CREATESAMPLE, nullptr, &pSound);

        if (eRes == FMOD_OK)
        {
            std::wstring soundKey;
            int length = MultiByteToWideChar(CP_ACP, 0, fd.name, -1, nullptr, 0);
            if (length > 0)
            {
                soundKey.resize(length - 1); // 널문자 제외
                MultiByteToWideChar(CP_ACP, 0, fd.name, -1, &soundKey[0], length);
            }
            wcout << soundKey.c_str() << endl;
            m_mapSound.emplace(soundKey, pSound);
        }
    }
    while (_findnexti64(handle, &fd) == 0);

    _findclose(handle);
    m_pSystem->update();
}


CSoundManager* CSoundManager::Create()
{
    CSoundManager* pInstance = new CSoundManager();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Create Sound Manager"));
    }
    return pInstance;
}

void CSoundManager::Free()
{
    CBase::Free();

    // 모든 사운드 리소스 해제
    for (auto& pair : m_mapSound)
    {
        pair.second->release(); // FMOD 사운드 해제
    }
    m_mapSound.clear();

    // FMOD 시스템 정리
    if (m_pSystem)
    {
        m_pSystem->close(); // 시스템 종료
        m_pSystem->release(); // 시스템 해제
        m_pSystem = nullptr;
    }
}



