#include "Level_GamePlay.h"



CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize_Clone()
{
	if (FAILED(Ready_HUD()))
	{
		CRASH("Failed Ready_HUD");
		return E_FAIL;
	}

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		CRASH("Failed Ready_Layer_Camera");
		return E_FAIL;
	}
	
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		CRASH("Failed Layer_Player");
		return E_FAIL;
	}
	
	// 몬스터는 무조건 Player 이후에 만들어야합니다.
	//if (FAILED(Ready_Layer_SkyBoss(TEXT("Layer_SkyBoss"))))
	//{
	//	CRASH("Failed Layer_SkyBoss");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
		CRASH("Failed Layer_Monster");
		return E_FAIL;
	}

	


	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
	{
		CRASH("Failed Ready_Layer_Map");
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		CRASH("Failed Light");
		return E_FAIL;
	}
	

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		CRASH("Failed Ready_Layer_BackGround");
		return E_FAIL;
	}


	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		CRASH("Failed Ready_Layer_Effect");
		return E_FAIL;
	}
	
	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox"))))
	{
		CRASH("Failed Ready_Layer_");
		return E_FAIL;
	}

	
	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyUp(DIK_I))
		m_pGameInstance->Publish<CInventory>(EventType::INVENTORY_DISPLAY, nullptr);

	

}

HRESULT CLevel_GamePlay::Render()
{
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_HUD()
{
	HUDEVENT_DESC Desc{};
	Desc.isVisibility = true;
	 
	// 이벤트 실행이지 구독이아님.
	m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY,  & Desc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}

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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc{};
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
	}


	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
	//	ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc )))
	//	return E_FAIL;



	return S_OK; 
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	
	CPlayer::PLAYER_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.eCurLevel = m_eCurLevel;

	CCamera_Player::CAMERA_PLAYER_DESC CameraPlayerDesc{};
	CameraPlayerDesc.vEye = _float4(0.f, 10.f, -20.f, 1.f);
	CameraPlayerDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraPlayerDesc.fFovy = XMConvertToRadians(60.0f);
	CameraPlayerDesc.fNear = 0.1f;
	CameraPlayerDesc.fFar = 500.f;
	CameraPlayerDesc.fSpeedPerSec = 10.f;
	CameraPlayerDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraPlayerDesc.fMouseSensor = 0.5f;
	

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Player"), &Desc)))
		return E_FAIL;
	

	list<CGameObject*> pGameObjects = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)->Get_GameObjects();
	auto iter = pGameObjects.begin();

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*iter);
	CameraPlayerDesc.pTarget = pPlayer;

	if (nullptr == CameraPlayerDesc.pTarget)
		CRASH("Failed CameraPlayer Add");

	if (FAILED(m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), ENUM_CLASS(LEVEL::GAMEPLAY)
		, TEXT("Prototype_GameObject_Camera_Player"), &CameraPlayerDesc)))
	{
		CRASH("Add Camera Player Failed");
		return E_FAIL;
	}


	//// 메인 카메라 변경.
	//if (FAILED(m_pGameInstance->Change_Camera(TEXT("ActionCamera"), ENUM_CLASS(LEVEL::GAMEPLAY))))
	//{
	//	CRASH("Change Camera Failed");
	//	return E_FAIL;
	//}

	// 메인 카메라 변경.
	if (FAILED(m_pGameInstance->Change_Camera(TEXT("PlayerCamera"), ENUM_CLASS(LEVEL::GAMEPLAY))))
	{
		CRASH("Change Camera Failed");
		return E_FAIL;
	}

	pPlayer->Set_Camera(dynamic_cast<CCamera_Player*>(m_pGameInstance->Get_MainCamera()));
	//pPlayer->Set_Camera(dynamic_cast<CCamera_Action*>(m_pGameInstance->Get_MainCamera()));


	return S_OK;
}

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
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkyBoss"), &Desc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
	//	ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkyBoss"), &Desc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC Desc = {};

	Desc.PrototypeTag = L"Prototype_Component_Model_BossStage";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{

	CWolfDevil::WOLFDEVIL_DESC Desc{};
	Desc.pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));
	Desc.eCurLevel = m_eCurLevel;
	Desc.eMonsterType = MONSTERTYPE::NORMAL;
	Desc.fMaxHP = 500.f;
	Desc.fAttackPower = 20.f;
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
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_WolfDevil"), &Desc)))
	{
		CRASH("Failed Create WolfDevil");
		return E_FAIL;
	}
		

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"))))
	{

		CRASH("Failed SkyBox");
		return E_FAIL;
	}
		

	return S_OK;
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
	__super::Free();



}
