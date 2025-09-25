
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
	//// 콘솔 버퍼 동기화,
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


	//if (FAILED(Start_Level(LEVEL::GAMEPLAY)))
	//	return E_FAIL;

	//// 원본
	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	
	

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);



}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.0f, 0.0f, 0.f, 1.f);
	

	m_pGameInstance->Render_Begin(&vClearColor);
	m_pImGui_Manager->Render_Begin();

	m_pGameInstance->Draw();

	

#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();

	// 기존 Player Debug Window
	ImVec2 windowPos = ImVec2(0.f, io.DisplaySize.y - 300.f);
	ImVec2 windowSize = ImVec2(300.f, 300.f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
	ImGui::Begin("Information");


	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
#endif // _DEBUG



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
	CGameObject* pGameObject = nullptr;
	
#pragma region MESH 타입.
	CEffect_Pillar::EFFECT_PILLARDESC EffectPillarDesc{};
	// 한번에 십자모양 12개씩 사용 예정. => 넉넉하게 넣자.
	for (_uint i = 0; i < 500; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectPillar"), &EffectPillarDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("BLOOD_PILLAR"), ENUM_CLASS(CEffect_Pillar::EffectType), pGameObject);
	}

	pGameObject = nullptr;

	//CEffect_LungePillar::EFFECT_LUNGEPILLARDESC EffectLunge_PillarDesc{};
	//// 한번에 1개씩 사용 예정. => 넉넉하게 넣자.
	//for (_uint i = 0; i < 300; ++i)
	//{
	//	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
	//		, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_LungePillar"), &EffectLunge_PillarDesc));
	//	if (nullptr == pGameObject)
	//	{
	//		CRASH("Failed Create GameObject");
	//		return E_FAIL;
	//	}
	//	m_pGameInstance->Add_GameObject_ToPools(TEXT("LUNGE_PILLAR"), ENUM_CLASS(CEffect_LungePillar::EffectType), pGameObject);
	//}

	//pGameObject = nullptr;


	CEffect_Wind::EFFECTWIND_DESC EffectWindDesc{};
	for (_uint i = 0; i < 500; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectWind"), &EffectWindDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("SWORD_WIND"), ENUM_CLASS(CEffect_Wind::EffectType), pGameObject);
	}

	CEffect_WindCircle::EFFECTWIND_CIRCLE_DESC EffectWindCircleDesc{};
	for (_uint i = 0; i < 500; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectWindCircle"), &EffectWindCircleDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("SWORD_WINDCIRCLE"), ENUM_CLASS(CEffect_WindCircle::EffectType), pGameObject);
	}


	pGameObject = nullptr;
#pragma endregion


#pragma region TEXTURE 타입.
	// 1. Prototype Clone 객체 생성.
	CSlash::SLASHEFFECT_DESC slashDesc{};
	//slashDesc.eShaderPath = POSTEX_SHADERPATH::MONSTER_LINESLASH;
	slashDesc.eShaderPath = EFFECTPOSTEX_SHADERPATH::MONSTER_LINESLASH;
	

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

	CRenketsuSlash::SLASHEFFECT_DESC RenketsuslashDesc{};
	//slashDesc.eShaderPath = POSTEX_SHADERPATH::MONSTER_LINESLASH;
	RenketsuslashDesc.eShaderPath = EFFECTPOSTEX_SHADERPATH::RENKETSU_LINESLASH;

	// 2. 추가할 개수만큼 추가. 
	/* 3개 추가. => 풀링 제대로 되는지 테스트. */
	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Renketsu_SlashEffect"), &slashDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("RENKETSU_SLASH_EFFECT"), ENUM_CLASS(CRenketsuSlash::EffectType), pGameObject);
	}

	CHitFlashEffect::HITFLASH_DESC HitFlashDesc{};
	HitFlashDesc.eShaderPath = EFFECTPOSTEX_SHADERPATH::HITFLASH;
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

	// MainApp.cpp의 Ready_Pooling 함수 내부에 추가

	CEffectParticle::EFFECT_PARTICLE_DESC burstDesc{};
	burstDesc.iNumInstance = 50; // 한 번에 터질 최대 파티클 개수
	burstDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_EXPLOSION; // 
	burstDesc.iShaderPath = ENUM_CLASS(POINTDIRPARTICLE_SHADERPATH::EXPLOSION); // 셰이더의 QueenKnightWarpPass (PS_DIFFUSE_MASK_MAIN) 사용

	// 파티클의 속도, 크기, 수명 범위를 설정합니다.
	burstDesc.vSpeed = { 1.f, 3.f };   // 10 ~ 15의 무작위 속도
	burstDesc.vSize = { 0.05f, 0.2f };  // 0.05 ~ 0.1의 무작위 크기
	burstDesc.vLifeTime = { 5.f, 10.f }; // 0.5 ~ 1.0초의 무작위 수명

	// 사용할 텍스처를 지정합니다.
	burstDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true; // g_DiffuseTexture
	burstDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 4;
	

	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &burstDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("EXPLOSION"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}


	/* Tornado*/
	CEffectParticle::EFFECT_PARTICLE_DESC QueenKnightTornadoParticle{};
	QueenKnightTornadoParticle.fSpeedPerSec = 10.f;
	QueenKnightTornadoParticle.fRotationPerSec = XMConvertToRadians(90.f);
	QueenKnightTornadoParticle.iShaderPath = static_cast<_uint>(POINTDIRPARTICLE_SHADERPATH::TORNADO);
	QueenKnightTornadoParticle.eParticleType = CEffectParticle::PARTICLE_TYPE_TORNADO; // VIBuffer에 들어감.
	QueenKnightTornadoParticle.iNumInstance = 500;
	QueenKnightTornadoParticle.vCenter = { 0.f, 0.f, 0.f };

	/*
	*  Pivot => 피봇 중심으로 x,z 기준으로 회전.
	*/
	QueenKnightTornadoParticle.vPivot = { 0.f, 0.f, 0.f }; 
	QueenKnightTornadoParticle.vRange = { 7.f, 7.f, 7.f };
	/*
	* 회전 속도.
	*/
	QueenKnightTornadoParticle.vSpeed = { 10.f, 15.f };
	/*
	* 크기.
	*/
	QueenKnightTornadoParticle.vSize = { 0.1f, 0.11f };
	QueenKnightTornadoParticle.vLifeTime = { 2.f, 5.f };
	QueenKnightTornadoParticle.isLoop = false;
	QueenKnightTornadoParticle.isBillBoard = true;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	QueenKnightTornadoParticle.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 2;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	QueenKnightTornadoParticle.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = true;
	QueenKnightTornadoParticle.useTextureCheckArray[TEXTURE::TEXTURE_OTHER] = true;


	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &QueenKnightTornadoParticle
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("TORNADO"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}


	CEffectParticle::EFFECT_PARTICLE_DESC ParticleDesc{};
	ParticleDesc.fSpeedPerSec = 10.f;
	ParticleDesc.fRotationPerSec = XMConvertToRadians(90.f);
	ParticleDesc.iShaderPath = static_cast<_uint>(POINTDIRPARTICLE_SHADERPATH::QUEENKNIGHTWARP);
	ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_QUEEN_WARP;
	ParticleDesc.iNumInstance = 700;
	ParticleDesc.vCenter = { 0.f, -1.f, 0.f };
	ParticleDesc.vRange = { 5.f, 15.f, 5.f };
	ParticleDesc.vSpeed = { 3.f, 9.f };
	ParticleDesc.vSize = { 0.1f, 0.11f };
	ParticleDesc.vLifeTime = { 2.f, 5.f };
	ParticleDesc.isLoop = false;
	ParticleDesc.isBillBoard = true;
	ParticleDesc.iSpawnCount = 50;
	
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 2;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;

	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = true;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_OTHER] = true;



	/* Particle Init Info 설정으로 Pool에서 사용할때마다. 값을 채워줍니다. */
	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &ParticleDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_WARP"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}

	CEffectParticle::EFFECT_PARTICLE_DESC ParticleExplosionDesc{};
	ParticleExplosionDesc.fSpeedPerSec = 10.f;
	ParticleExplosionDesc.fRotationPerSec = XMConvertToRadians(90.f);
	ParticleExplosionDesc.iShaderPath = 1;
	ParticleExplosionDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_BOSS_EXPLOSION;
	ParticleExplosionDesc.iNumInstance = 450;
	ParticleExplosionDesc.vCenter = { 0.f, 0.f, 0.f };
	ParticleExplosionDesc.vRange = { 2.f, 2.f, 2.f };
	ParticleExplosionDesc.vSpeed = { 3.f, 7.f };
	ParticleExplosionDesc.vSize = { 0.1f, 0.11f };
	ParticleExplosionDesc.vLifeTime = { 3.f, 10.f };
	ParticleExplosionDesc.isLoop = false;
	ParticleExplosionDesc.isBillBoard = true;
	//ParticleExplosionDesc.iSpawnCount = 50;
	ParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	ParticleExplosionDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 1;
	ParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	ParticleExplosionDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;
	ParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	ParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	ParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = true;



	/* Particle Init Info 설정으로 Pool에서 사용할때마다. 값을 채워줍니다. */
	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &ParticleExplosionDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_EXPLOSION"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}
#pragma endregion


#pragma region HIT PARTICLE 타입
	CEffectParticle::EFFECT_PARTICLE_DESC HitParticleExplosionDesc{};
	HitParticleExplosionDesc.fSpeedPerSec = 10.f;
	HitParticleExplosionDesc.fRotationPerSec = XMConvertToRadians(90.f);
	HitParticleExplosionDesc.iShaderPath = static_cast<_uint>(POINTDIRPARTICLE_SHADERPATH::HITPARTICLE);
	HitParticleExplosionDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_HIT_PARTCILE;
	HitParticleExplosionDesc.iNumInstance = 50;
	HitParticleExplosionDesc.vCenter = { 0.f, 0.f, 0.f };
	HitParticleExplosionDesc.vRange = { 1.f, 1.f, 1.f };
	HitParticleExplosionDesc.vSpeed = { 2.f, 3.f };
	HitParticleExplosionDesc.vSize = { 0.05f, 0.06f };
	HitParticleExplosionDesc.vLifeTime = { 1.f, 2.f };
	HitParticleExplosionDesc.isLoop = false;
	HitParticleExplosionDesc.isBillBoard = true;
	HitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	HitParticleExplosionDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 2;
	HitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_OTHER] = true;
	HitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = true;


	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &HitParticleExplosionDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("HIT_PARTICLE"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}

	CEffectParticle::EFFECT_PARTICLE_DESC PlayerHitParticleExplosionDesc{};
	PlayerHitParticleExplosionDesc.fSpeedPerSec = 10.f;
	PlayerHitParticleExplosionDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PlayerHitParticleExplosionDesc.iShaderPath = static_cast<_uint>(POINTDIRPARTICLE_SHADERPATH::PLAYERHITPARTICLE);
	PlayerHitParticleExplosionDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_HIT_PARTCILE; // VIBuffer에 들어감.
	PlayerHitParticleExplosionDesc.iNumInstance = 50;
	PlayerHitParticleExplosionDesc.vCenter = { 0.f, 0.f, 0.f };
	PlayerHitParticleExplosionDesc.vRange = { 1.f, 1.f, 1.f };
	PlayerHitParticleExplosionDesc.vSpeed = { 2.f, 3.f };
	PlayerHitParticleExplosionDesc.vSize = { 0.05f, 0.06f };
	PlayerHitParticleExplosionDesc.vLifeTime = { 1.f, 2.f };
	PlayerHitParticleExplosionDesc.isLoop = false;
	PlayerHitParticleExplosionDesc.isBillBoard = true;
	PlayerHitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	PlayerHitParticleExplosionDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 7;
	PlayerHitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_OTHER] = true;
	PlayerHitParticleExplosionDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = true;


	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &PlayerHitParticleExplosionDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("PLAYER_HITPARTICLE"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}

#pragma endregion

#pragma region PLAYER SKILL
	CEffect_PlayerSkill::EFFECT_PLAYERSKILL_DESC PlayerSkillDesc{};
	PlayerSkillDesc.fSpeedPerSec = 10.f;
	PlayerSkillDesc.fRotationPerSec = XMConvertToRadians(90.f);

	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectPlayerAuraContainer"), &PlayerSkillDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("PLAYER_AURA"), ENUM_CLASS(CEffect_PlayerSkill::EffectType), pGameObject);
	}
	
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
