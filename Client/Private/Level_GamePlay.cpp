CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize_Clone()
{
	// FadeOut 객체 먼저 생성
	if (FAILED(Ready_Layer_FadeOut(TEXT("Layer_FadeOut"))))
	{
		CRASH("Failed Layer_FadeOut");
		return E_FAIL;
	}

	// 레벨 시작 시 플레이어 데이터 초기화
	if (FAILED(Ready_Lights()))
	{
		CRASH("Failed Light");
		return E_FAIL;
	}

	if (FAILED(Ready_HUD()))
	{
		CRASH("Failed Ready_HUD");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
	{
		CRASH("Failed Ready_Layer_Map");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		CRASH("Failed Layer_Player");
		return E_FAIL;
	}

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		CRASH("Failed Ready_Layer_Camera");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox"))))
	{
		CRASH("Failed Ready_Layer_SkyBox");
		return E_FAIL;
	}

	if (FAILED(Ready_Monster_Trigger()))
	{
		CRASH("Failed Ready_Layer_Trigger");
		return E_FAIL;
	}

	

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		CRASH("Failed Ready_Layer_Effect");
		return E_FAIL;
	}
	
	// 레벨 시작 시 FadeIn 효과 시작
	Start_FadeIn();
	
	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

#pragma region 0. 먼저 생성되어야 하는 객체들 추가. 
HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_HUD()
{
	HUDEVENT_DESC Desc{};
	Desc.isVisibility = true;

	// 이벤트 실행이지 구독이아님.
	m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY, &Desc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC Desc = {};

	// X, Z 2배로 깔았음. => Prototype Tag에 해당하는 맵 모델 가져옴.
	Desc.PrototypeTag = L"Prototype_Component_Model_BossStage";
	Desc.vScale = { 2.f, 1.f, 2.f };
	Desc.eCurLevel = m_eCurLevel;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map"), &Desc)))
	{
		CRASH("Failed Ready Layer Map");
		return E_FAIL;
	}
		

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{

	CPlayer::PLAYER_DESC Desc{};
#pragma region 1. 플레이어에게 넣어줘야할 레벨 별 다른 값들.
	Desc.eCurLevel = m_eCurLevel;
	Desc.vPos = { 270.f, 0.f, 0.f };
	//Desc.vPos = { 100.f, 0.f, 0.f };
#pragma endregion

#pragma region 2. 게임에서 계속 들고있어야할 플레이어 값들.
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	// TODO: 이전 레벨에서 플레이어 데이터를 가져오는 로직 필요
	// 현재는 StageOne 완료 후 예상 스탯으로 설정
	Desc.fMaxHP = 1672;
	Desc.fHP = 1672; // StageOne 전투 후 감소된 HP
	Desc.fAttackPower = 90;
#pragma endregion

	

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"), &Desc)))
	{
		CRASH("Failed Ready GameObject Player");
		return E_FAIL;
	}
		

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Player::CAMERA_PLAYER_DESC CameraPlayerDesc{};
	CameraPlayerDesc.vEye = _float4(0.f, 10.f, -20.f, 1.f);
	CameraPlayerDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraPlayerDesc.fFovy = XMConvertToRadians(60.0f);
	CameraPlayerDesc.fNear = 0.1f;
	CameraPlayerDesc.fFar = 500.f;
	CameraPlayerDesc.fSpeedPerSec = 10.f;
	CameraPlayerDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraPlayerDesc.fMouseSensor = 0.8f;
	CameraPlayerDesc.vTargetOffset = { 0.f, 1.7f, -3.3f, 0.f };
	CameraPlayerDesc.vLockOnOffset = { 0.f, 1.7f, -3.3f, 0.f };
	CameraPlayerDesc.eCurLevel = m_eCurLevel;

	list<CGameObject*> pGameObjects = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Player"))->Get_GameObjects();
	auto iter = pGameObjects.begin();

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*iter);
	CameraPlayerDesc.pTarget = pPlayer;

	if (nullptr == CameraPlayerDesc.pTarget)
		CRASH("Failed CameraPlayer Add");

	if (FAILED(m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Camera_Player"), &CameraPlayerDesc)))
	{
		CRASH("Add Camera Player Failed");
		return E_FAIL;
	}

	// 메인 카메라 변경.
	if (FAILED(m_pGameInstance->Change_Camera(TEXT("PlayerCamera"), ENUM_CLASS(m_eCurLevel))))
	{
		CRASH("Change Camera Failed");
		return E_FAIL;
	}

	pPlayer->Set_Camera(dynamic_cast<CCamera_Player*>(m_pGameInstance->Get_MainCamera()));

	/*CCamera_Free::CAMERA_FREE_DESC CameraDesc{};
	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_Camera(TEXT("FreeCamera"), ENUM_CLASS(LEVEL::GAMEPLAY)
		, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	{
		CRASH("Add Free Camera Failed");
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	CSky::SKY_DESC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc.PrototypeTag = TEXT("Prototype_Component_Texture_Sky");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Sky"), &Desc)))
	{

		CRASH("Failed SkyBox");
		return E_FAIL;
	}

	return S_OK;
}
#pragma endregion


#pragma region 1. 조건에 부합하는 Trigger 추가.

HRESULT CLevel_GamePlay::Ready_Monster_Trigger()
{
	// 0. TriggerManager에 Player 전달.
	m_pGameInstance->Set_TargetPlayer(
		m_pGameInstance->Get_GameObjcet(ENUM_CLASS(m_eCurLevel)
		, TEXT("Layer_Player"), 0));
	
	// 1. 객체 생성 해서 Trigger Manager에 전달.
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
		CRASH("Failed Layer_Monster");
		return E_FAIL;
	}

	

	return S_OK;
}

/* 모두 Monster Layer에 추가. => Trigger */
HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{

	if (FAILED(Ready_Layer_WolfDevil(strLayerTag)))
	{
		CRASH("Failed Layer_WolfDevil");
		return E_FAIL;
	}
	
	if (FAILED(Ready_Layer_SlaveVampire(strLayerTag)))
	{
		CRASH("Failed Layer SlaveVampire");
		return E_FAIL;
	}
	
	/*
	* 트리거 등록
	*/
	// 2. 트리거 등록. => 생성과 트리거 등록은 구별.
	TRIGGER_MONSTER_DESC TriggerDesc{};
	
	// 첫 번째 트리거: WolfDevil
	TriggerDesc = { { 250.f , 0.f, 0.f }, 50.f , TEXT("Layer_WolfDevil")
		, TEXT("Layer_Monster") , 2, 0 };
	
	//m_pGameInstance->Add_Trigger(ENUM_CLASS(m_eCurLevel), TriggerDesc);
	
	// 두 번째 트리거: SlaveVampire
	TriggerDesc = { { 200.f , 0.f, 0.f }, 50.f , TEXT("Layer_SlaveVampire")
		, TEXT("Layer_Monster") , 2, 0 };
	
	//m_pGameInstance->Add_Trigger(ENUM_CLASS(m_eCurLevel), TriggerDesc);

	/* 다 같은 Monster 레이어에 추가하기. */
	if (FAILED(Ready_Layer_QueenKnight(strLayerTag)))
	{
		CRASH("Failed Layer_QueenKnight");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_WolfDevil(const _wstring& strLayerTag)
{
	/* 주로 몬스터들을 Trigger 객체에 추가하기. */
	/* 공통.. */
	CWolfDevil::WOLFDEVIL_DESC Desc{};

	Desc.eCurLevel = m_eCurLevel;
	Desc = { 50.f, XMConvertToRadians(90.0f)
		, nullptr,  m_eCurLevel, MONSTERTYPE::NORMAL,
		500.f, 70.f, 15.f, 7.f, 50.f, 50.f};

	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));

	if (nullptr == Desc.pPlayer)
	{
		CRASH("Failed Search Player");
		return E_FAIL;
	}
	
	for (_uint i = 0; i < 4; ++i)
	{
		Desc.vPos = { 250.f, 0.f, 3.f};
		Desc.vPos.x += (i / 2) * -10.f;
		Desc.vPos.z *= i % 2 == 0 ? -1.f : 1.f;
		if (FAILED(m_pGameInstance->Add_GameObject_ToTrigger(ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_WolfDevil"), ENUM_CLASS(m_eCurLevel)
			, TEXT("Prototype_GameObject_WolfDevil"), &Desc)))
		{
			CRASH("Failed_Create WolfDevil");
			return E_FAIL;
		}
	}

	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SlaveVampire(const _wstring& strLayerTag)
{

	CSlaveVampire::SLAVE_VAMPIRE_DSEC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc = { 50.f, XMConvertToRadians(90.0f)
		, nullptr,  m_eCurLevel, MONSTERTYPE::NORMAL,
		900.f, 70.f, 20.f, 5.f, 50.f, 50.f };

	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));

	if (nullptr == Desc.pPlayer)
	{
		CRASH("Failed Search Player");
		return E_FAIL;
	}

	for (_uint i = 0; i < 2; ++i)
	{
		Desc.vPos = { 200.f, 0.f, 3.f };
		Desc.vPos.x += (i) * -20.f;
		Desc.vPos.z *= i % 2 == 0 ? -1.f : 1.f;
		if (FAILED(m_pGameInstance->Add_GameObject_ToTrigger(ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_SlaveVampire"), ENUM_CLASS(m_eCurLevel)
			, TEXT("Prototype_GameObject_SlaveVampire"), &Desc)))
		{
			CRASH("Failed_Create SlaveVampire");
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_QueenKnight(const _wstring& strLayerTag)
{
	CQueenKnight::QUEENKNIGHT_DESC Desc{};
	/* Transform 설정.*/
	Desc.fSpeedPerSec = 50.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.eCurLevel = m_eCurLevel;
	Desc = { 50.f, XMConvertToRadians(180.f)
		, nullptr, m_eCurLevel, MONSTERTYPE::BOSS, 2500.f, 150.f
		, 30.f, 7.f, 10.f, 10.f, {1.f, 1.f, 1.f}
	};

	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));

	if (nullptr == Desc.pPlayer)
	{
		CRASH("Failed Search Player");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToTrigger(ENUM_CLASS(m_eCurLevel)
		, TEXT("Layer_QueenKnight"), ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_QueenKnight"), &Desc)))
	{
		CRASH("Failed_Create WolfDevil");
		return E_FAIL;
	}

	// 3. 세 번째 트리거: QueenKnight (보스)
	TRIGGER_MONSTER_DESC TriggerDesc{};

	TriggerDesc = { { 150.f , 0.f, 0.f }, 200.f , TEXT("Layer_QueenKnight")
		, TEXT("Layer_Monster") , 1, 2 }; // 이전 2개 트리거 완료 후 발동

	m_pGameInstance->Add_Trigger(ENUM_CLASS(m_eCurLevel), TriggerDesc);

	return S_OK;
}

#pragma endregion




HRESULT CLevel_GamePlay::Ready_Layer_SkyBoss(const _wstring& strLayerTag)
{
	CSkyBoss::SKYBOSS_DESC Desc{};
	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));
	Desc.eCurLevel = m_eCurLevel;
	Desc.eMonsterType = MONSTERTYPE::BOSS;
	Desc.fMaxHP = 3000.f;
	Desc.fAttackPower = 30.f;
	Desc.fDetectionRange = 20.f;
	Desc.fAttackRange = 10.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fMoveSpeed = 10.f;
	/* Transform 설정.*/
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (nullptr == Desc.pPlayer)
	{
		CRASH("Failed Search Player");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_QueenKnight"), &Desc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
	//	ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkyBoss"), &Desc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_FadeOut(const _wstring& strLayerTag)
{
	CFade_Out::FADEOUT_DESC Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.iTextureIndex = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FadeOut"), &Desc)))
	{
		CRASH("Failed Ready Fade Out");
		return E_FAIL;
	}
		
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	
	return S_OK;
}

void CLevel_GamePlay::Start_FadeIn()
{
	// FadeIn 객체 찾기
	CGameObject* pFadeOut = m_pGameInstance->Get_GameObjcet(ENUM_CLASS(m_eCurLevel), TEXT("Layer_FadeOut"), 0);
	if (nullptr != pFadeOut)
	{
		CFade_Out* pFade = dynamic_cast<CFade_Out*>(pFadeOut);
		if (nullptr != pFade)
		{
			pFade->Start_FadeIn();
		}
	}
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_GamePlay::Free()
{
	CLevel::Free();
}
