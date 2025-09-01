#include "Level_StageOne.h"
CLevel_StageOne::CLevel_StageOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_StageOne::Initialize_Clone()
{
	if (FAILED(Ready_Layer_FadeOut(TEXT("Layer_FadeOut"))))
	{
		CRASH("Failed Layer_FadeOut");
		return E_FAIL;
	}
	
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

	//if (FAILED(Ready_Monster_Trigger()))
	//{
	//	CRASH("Failed Ready_Layer_Trigger");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		CRASH("Failed Ready_Layer_Effect");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Pooling()))
	{
		CRASH("Failed Ready_Layer_Pooling");
		return E_FAIL;
	}

	if (FAILED(Ready_Events()))
	{
		CRASH("Failed Ready_Events");
		return E_FAIL;
	}

	// 레벨 시작 시 FadeIn 효과 시작
	Start_FadeIn();

	return S_OK;
}

void CLevel_StageOne::Update(_float fTimeDelta)
{
	// 1. 현재 트리거가 모두 종료되었다면? => 완료의 기준 
	// => 마지막 트리거가 발동되었고 객체들이 모두 사망 처리 또는 죽었다던지?
	if (m_pGameInstance->Trigger_Finished(ENUM_CLASS(m_eCurLevel)) && !m_IsFadeOutStarted)
	{
		// 2. FadeOut 시작
		Start_FadeOut();
	}

	if (m_pGameInstance->Get_KeyUp(DIK_F1))
	{
		_uint iLevelID = m_pGameInstance->Get_CurrentLevelID();
		m_pGameInstance->Publish<CLevel_StageOne>(EventType::OPEN_DEBUG, nullptr);
		return;
	}

}

HRESULT CLevel_StageOne::Render()
{
	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_FadeOut(const _wstring& strLayerTag)
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

HRESULT CLevel_StageOne::Ready_Lights()
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

HRESULT CLevel_StageOne::Ready_HUD()
{
	HUDEVENT_DESC Desc{};
	Desc.isVisibility = true;

	// 이벤트 실행이지 구독이아님.
	m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY, &Desc);

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC Desc = {};

	// X, Z 2배로 깔았음.
	//Desc.PrototypeTag = L"Prototype_Component_Model_BossStage";
	Desc.PrototypeTag = L"Prototype_Component_Model_StageOne";
	Desc.vScale = { 1.f, 1.f, 1.f };
	Desc.eCurLevel = m_eCurLevel;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC Desc{};
#pragma region 1. 플레이어에게 넣어줘야할 레벨 별 다른 값들.
	Desc.eCurLevel = m_eCurLevel;
	//Desc.vPos = { 183.f, 21.f, -24.f };
	//Desc.vPos = { 200.f, 13.f, 9.f };
	Desc.vPos = { 0.f, 0.f, 0.f };
#pragma endregion

#pragma region 2. 게임에서 계속 들고있어야할 플레이어 값들.
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	// 특히 가장 중요.
	Desc.fMaxHP = 1672;
	Desc.fHP = 1672;
	Desc.fAttackPower = 90;
#pragma endregion


	//Desc.vPos = { 100.f, 0.f, 0.f };


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"), &Desc)))
	{
		CRASH("Failed Ready GameObject Player");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_Camera(const _wstring& strLayerTag)
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
	CameraPlayerDesc.vTargetOffset = { 0.f, 1.6f, -3.f, 0.f };
	CameraPlayerDesc.vLockOnOffset = { 0.f, 1.6f, -3.f, 0.f };
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

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{

	CSky::SKY_DESC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc.PrototypeTag = TEXT("Prototype_Component_Texture_SkyStageOne");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Sky"), &Desc)))
	{

		CRASH("Failed SkyBox");
		return E_FAIL;
	}

	return S_OK;
}


#pragma region 1. Monster Trigger 준비 => 몬스터들을 Trigger Manager에 담아둡니다.
HRESULT CLevel_StageOne::Ready_Monster_Trigger()
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

HRESULT CLevel_StageOne::Ready_Layer_Monster(const _wstring& strLayerTag)
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

	TriggerDesc = { { 0.f ,0.f, 0.f }, 250.f , TEXT("Layer_SlaveVampire")
		, TEXT("Layer_Monster") , 4, 0 };

	/*TriggerDesc = { { 250.f , 0.f, 0.f }, 200.f , TEXT("Layer_WolfDevil")
		, TEXT("Layer_Monster") , 2, 0 };*/

	m_pGameInstance->Add_Trigger(ENUM_CLASS(m_eCurLevel), TriggerDesc);

	if (FAILED(Ready_Layer_GiantWhiteDevil(strLayerTag)))
	{
		CRASH("Faiiled Layer GiantWhite Devil");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_WolfDevil(const _wstring& strLayerTag)
{
	/* 주로 몬스터들을 Trigger 객체에 추가하기. */
	/* 공통.. */
	CWolfDevil::WOLFDEVIL_DESC Desc{};
	Desc.eCurLevel = m_eCurLevel;

	Desc = { 50.f, XMConvertToRadians(90.0f)
		, nullptr,  m_eCurLevel, MONSTERTYPE::NORMAL,
		500.f, 70.f, 15.f, 7.f, 50.f, 50.f };

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
		Desc.vPos = { 250.f, 0.f, 3.f };
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

HRESULT CLevel_StageOne::Ready_Layer_SlaveVampire(const _wstring& strLayerTag)
{
	CSlaveVampire::SLAVE_VAMPIRE_DSEC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc = { 50.f, XMConvertToRadians(90.0f)
		, nullptr,  m_eCurLevel, MONSTERTYPE::NORMAL,
		900.f, 70.f, 15.f, 5.f, 50.f, 50.f };

	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));

	if (nullptr == Desc.pPlayer)
	{
		CRASH("Failed Search Player");
		return E_FAIL;
	}

	_float3 monsterPositionArray[4] = {
		{ 32.f, 21.f, -28.f }, { 50.f, 21.f, -28.f },
		{ 62.f, 21.f, -38.f }, { 61.f, 21.f, -38.f }
	};


	for (_uint i = 0; i < 4; ++i)
	{
		Desc.vPos = monsterPositionArray[i];
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

HRESULT CLevel_StageOne::Ready_Layer_GiantWhiteDevil(const _wstring& strLayerTag)
{
	CGiant_WhiteDevil::GIANTWHITEDEVIL_DESC Desc{};
	/* Transform 설정.*/
	Desc.fSpeedPerSec = 50.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.eCurLevel = m_eCurLevel;
	Desc = { 50.f, XMConvertToRadians(180.f)
		, nullptr, m_eCurLevel, MONSTERTYPE::BOSS, 2200.f, 150.f
		, 22.f, 7.f, 10.f, 10.f, {1.f, 1.f, 1.f}
	};
	// 60, 21, -28
	Desc.vPos = { 203.f, 15.f, 18.f }; // 위치 설정.
	//Desc.vPos = { 60.f, 21.f, -28.f }; // 위치 설정.

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
		, TEXT("Layer_GiantWhiteDevil"), ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Giant_WhiteDevil"), &Desc)))
	{
		CRASH("Failed_Create WolfDevil");
		return E_FAIL;
	}

	TRIGGER_MONSTER_DESC TriggerDesc{};
	// 60, 21, -28
	TriggerDesc = { { 200.f , 15.f, 18.f }, 200.f , TEXT("Layer_GiantWhiteDevil")
		, TEXT("Layer_Monster") , 1, 0 }; // 이전 2개 트리거 완료 후 발동

	m_pGameInstance->Add_Trigger(ENUM_CLASS(m_eCurLevel), TriggerDesc);

	return S_OK;
}

HRESULT CLevel_StageOne::Ready_Layer_Pooling()
{
	//// 1. Prototype Clone 객체 생성.
	//CSlash::SLASHEFFECT_DESC slashDesc{};
	//slashDesc.eCurLevel = m_eCurLevel;

	//CGameObject* pGameObject = nullptr;

	///* 100개 추가. */
	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
	//		, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SlashEffect"), &slashDesc));
	//	if (nullptr == pGameObject)
	//	{
	//		CRASH("Failed Create GameObject");
	//		return E_FAIL;
	//	}
	//	m_pGameInstance->Add_GameObject_ToPools(
	//		ENUM_CLASS(m_eCurLevel), TEXT("SLASH_EFFECT"), pGameObject);
	//}

	
	return S_OK;
}

#pragma endregion


HRESULT CLevel_StageOne::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	//CSnow::SNOW_DESC Desc{};
	//Desc.eCurLevel = m_eCurLevel;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
	//	ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Snow"), &Desc)))
	//{
	//	CRASH("Failed Ready Layer Snow");
	//	return E_FAIL;
	//}
		
	return S_OK;
}

#pragma region 레벨 전환
void CLevel_StageOne::Open_Level(LEVEL eLevel)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevel))))
	{
		CRASH("Failed Open Level StageOne");
		return;
	}
}

void CLevel_StageOne::Start_FadeOut()
{
	m_IsFadeOutStarted = true;
	
	// FadeOut 객체 찾기
	CGameObject* pFadeOut = m_pGameInstance->Get_GameObjcet(ENUM_CLASS(m_eCurLevel), TEXT("Layer_FadeOut"), 0);
	if (nullptr != pFadeOut)
	{
		CFade_Out* pFade = dynamic_cast<CFade_Out*>(pFadeOut);
		if (nullptr != pFade)
		{
			pFade->Start_FadeOut();
		}
	}
}

void CLevel_StageOne::Start_FadeIn()
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

HRESULT CLevel_StageOne::Ready_Events()
{
	m_pGameInstance->Subscribe(EventType::OPEN_GAMEPAY, Get_ID(), [this](void* pData)
		{
			this->Open_Level(LEVEL::GAMEPLAY);
		});

	m_Events.emplace_back(EventType::OPEN_GAMEPAY, Get_ID());

	m_pGameInstance->Subscribe(EventType::OPEN_DEBUG, Get_ID(), [this](void* pData)
		{
			this->Open_Level(LEVEL::DEBUG);
		});

	m_Events.emplace_back(EventType::OPEN_DEBUG, Get_ID());



	return S_OK;
}
#pragma endregion



CLevel_StageOne* CLevel_StageOne::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_StageOne* pInstance = new CLevel_StageOne(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_StageOne"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_StageOne::Free()
{
	CLevel::Free();
}
