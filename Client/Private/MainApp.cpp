CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
#ifdef _DEBUG
	//AllocConsole();
#endif // DEBUG

	//AllocConsole();
	//
	//// 표준 출력, 에러, 입력 핸들을 콘솔에 연결
	//FILE* fp;
	//
	//freopen_s(&fp, "CONOUT$", "w", stdout); // std::cout
	//freopen_s(&fp, "CONOUT$", "w", stderr); // std::cerr
	//freopen_s(&fp, "CONIN$", "r", stdin);   // std::cin
	//
	//// 콘솔 버퍼 동기화
	//std::ios::sync_with_stdio(true);

	

	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
	{
		CRASH("Failed Ready Initialize Engine");
		return E_FAIL;
	}

	// Loader에서 Main App로딩 시작
	if (FAILED(m_cLoader_Static.Loading_Resource(m_pDevice, m_pContext, m_pGameInstance)))
	{
		CRASH("Failed Loading Resource");
		return E_FAIL;
	}
		

	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	if (FAILED(Ready_Clone_ForStatic()))
	{
		CRASH("Failed Ready Clone For Static");
		return E_FAIL;
	}
		
	
	if (FAILED(Ready_Pooling()))
	{
		CRASH("Failed Ready Pooling For Static");
		return E_FAIL;
	}


	if (FAILED(Start_Level(LEVEL::GAMEPLAY)))
		return E_FAIL;

	// 원본
	//if (FAILED(Start_Level(LEVEL::LOGO)))
	//	return E_FAIL;

	//if (FAILED(Start_Level(LEVEL::DEBUG)))
	//	return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.0f, 0.0f, 0.f, 1.f);
	
	// bool Return;

	m_pGameInstance->Render_Begin(&vClearColor);
	m_pImGui_Manager->Render_Begin();

	m_pGameInstance->Draw();

	ImGui::Begin("Information");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	m_pImGui_Manager->Render_End();
	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Clone_ForStatic()
{
	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_HUD(TEXT("Layer_HUD"))))
		return E_FAIL;
	//
	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_Inventory(TEXT("Layer_Inventory"))))
		return E_FAIL;

	// 
	if (FAILED(Ready_Clone_SkillUI(TEXT("Layer_SkillUI"))))
		return E_FAIL;


	return S_OK;
}




HRESULT CMainApp::Ready_Clone_HUD(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HUD"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Clone_Inventory(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Clone_SkillUI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Pooling()
{
	 
#pragma region TEXTURE 타입.
	// 1. Prototype Clone 객체 생성.
	CSlash::SLASHEFFECT_DESC slashDesc{};
	slashDesc.eShaderPath = POSTEX_SHADERPATH::MONSTER_LINESLASH;
	CGameObject* pGameObject = nullptr;

	// 2. 추가할 개수만큼 추가. 
	/* 3개 추가. => 풀링 제대로 되는지 테스트. */
	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SlashEffect"), &slashDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("SLASH_EFFECT"), ENUM_CLASS(CSlash::EffectType), pGameObject);
	}

	CHitFlashEffect::HITFLASH_DESC HitFlashDesc{};
	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"), &HitFlashDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create HitFlashGameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("HITFLASH_EFFECT"), ENUM_CLASS(CHitFlashEffect::EffectType), pGameObject);
	}

#pragma endregion

#pragma region PARTICLE 타입 => 디버그 용도. 
	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
	//		, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Particle"), &HitFlashDesc));
	//	if (nullptr == pGameObject)
	//	{
	//		CRASH("Failed Create Particle GameObject");
	//		return E_FAIL;
	//	}
	//	m_pGameInstance->Add_GameObject_ToPools(TEXT("Particle_EFFECT"), ENUM_CLASS(EFFECTTYPE::PARTICLE), pGameObject);
	//}

	CEffectParticle::EFFECT_PARTICLE_DESC ParticleDesc{};
	ParticleDesc.fSpeedPerSec = 10.f;
	ParticleDesc.fRotationPerSec = XMConvertToRadians(90.f);
	ParticleDesc.iShaderPath = 1;
	ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_QUEEN_WARP;
	//ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_QUEEN_WARP_SPAWN;
	ParticleDesc.iNumInstance = 500;
	ParticleDesc.vCenter = { 0.f, -1.f, 0.f };
	ParticleDesc.vRange = { 5.f, 5.f, 5.f };
	ParticleDesc.vSpeed = { 3.f, 9.f };
	ParticleDesc.vSize = { 0.1f, 0.11f };
	ParticleDesc.vLifeTime = { 5.f, 6.f };
	ParticleDesc.isLoop = false;
	ParticleDesc.isBillBoard = true;
	ParticleDesc.iSpawnCount = 50;
	
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 1;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;

	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = false;



	/* Particle Init Info 설정으로 Pool에서 사용할때마다. 값을 채워줍니다. */
	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &ParticleDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_WARP"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}

	ParticleDesc.fSpeedPerSec = 10.f;
	ParticleDesc.fRotationPerSec = XMConvertToRadians(90.f);
	ParticleDesc.iShaderPath = 1;
	ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_BOSS_EXPLOSION;
	//ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_QUEEN_WARP_SPAWN;
	ParticleDesc.iNumInstance = 450;
	ParticleDesc.vCenter = { 0.f, 0.f, 0.f };
	ParticleDesc.vRange = { 2.f, 2.f, 2.f };
	ParticleDesc.vSpeed = { 10.f, 15.f };
	ParticleDesc.vSize = { 0.1f, 0.11f };
	ParticleDesc.vLifeTime = { 3.f, 6.f };
	ParticleDesc.isLoop = false;
	ParticleDesc.isBillBoard = true;
	ParticleDesc.iSpawnCount = 50;

	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 1;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;

	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = false;



	/* Particle Init Info 설정으로 Pool에서 사용할때마다. 값을 채워줍니다. */
	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &ParticleDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_EXPLOSION"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}
#pragma endregion

#pragma region PARTICLE SYSTEM으로 만들기.
	//CParticleSystem::PARTICLESYSTEM_CLONE_DESC ParticleSystemDesc = ParticlePresets::Get(CParticleSystem::PARTICLE_TYPE_QUEEN_WARP);
	//
	//// 객체 100개 풀링.
	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
	//		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
	//		, TEXT("Prototype_QueenKnight_ParticleSystem"), &ParticleSystemDesc
	//	));
	//	m_pGameInstance->Add_GameObject_ToPools(ParticleSystemDesc.strPoolTag,
	//		ENUM_CLASS(EFFECTTYPE::PARTICLE), pGameObject);
	//}

	//CParticleSystem::PARTICLESYSTEM_DESC desc = ParticlePresets::Get(CParticleSystem::PARTICLE_TYPE::PARTICLE_TYPE_QUEEN_WARP);
	
#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
	{
		CRASH("Failed Start Level");
		return E_FAIL;
	}
		

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	CBase::Free();

	//FreeConsole();



	Safe_Release(m_pImGui_Manager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
