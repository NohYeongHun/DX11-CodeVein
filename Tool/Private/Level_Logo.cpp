#include "Level_Logo.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
	
{
}

HRESULT CLevel_Logo::Initialize_Clone()
{
	//m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Title(TEXT("Layer_Title"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Parts(TEXT("Layer_Map_Part"))))
		return E_FAIL;


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;

	if (FAILED(Ready_Map_Tool()))
		return E_FAIL;
	

	

	/* 등록 후 등록한 개체들 순회하면서 Hierarchy에 등록? */
	
	//m_pImGui_Manager->Register_Hierarchy_Layer(pLayer);
	
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	m_pMapTool->Update(fTimeDelta);

	return;
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	m_pMapTool->Render();

	return S_OK;
}

void CLevel_Logo::Open_Level()
{
	
}

HRESULT CLevel_Logo::Ready_Layer_Title(const _wstring& strLayerTag)
{
	
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Map(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map"))))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

// Map_Part 레이어에 순번으로 존재.
HRESULT CLevel_Logo::Ready_Layer_Map_Parts(const _wstring& strLayerTag)
{
	
	MODEL_CREATE_DESC Desc{}; // Desc에 전달한 Model Tag를 이용해서 적합한 Model Component를 임포트합니다..
	// 1. Floor
	/*Desc.pModelTag = TEXT("MapPart_Floor");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map_Part"));
		return E_FAIL;
	}

	Desc.pModelTag = TEXT("MapPart_Side_Floor");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map_Part"));
		return E_FAIL;
	}

	Desc.pModelTag = TEXT("MapPart_Circle_Floor");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map_Part"));
		return E_FAIL;
	}*/

	// 1. BluePillar
	/*Desc.pModelTag = TEXT("MapPart_BluePillar");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map_Part"));
		return E_FAIL;
	}*/

	/*Desc.pModelTag = TEXT("MapPart_CircleFloor");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer MapPart_CircleFloor"));
		return E_FAIL;
	}*/

	//// 2. Pillar
	/*Desc.pModelTag = TEXT("MapPart_Pillar");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
		, strLayerTag, ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map_Part"), &Desc)))
	{
		MSG_BOX(TEXT("Failed to Add GameObject to Layer Map_Part"));
		return E_FAIL;
	}*/


	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Events()
{
	m_pGameInstance->Subscribe(EventType::OPEN_GAMEPAY, Get_ID(), [this](void* pData)
		{
			this->Open_Level();
		});

	m_Events.emplace_back(EventType::OPEN_GAMEPAY, Get_ID());

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Map_Tool()
{
	_uint iCurrentLevelID = ENUM_CLASS(m_eCurLevel);
	CLayer* pLayer = m_pGameInstance->Get_Layer(iCurrentLevelID, TEXT("Layer_Map_Part"));

	m_pMapTool = CMap_Tool::Create(m_pDevice, m_pContext, m_eCurLevel);
	if (nullptr == m_pMapTool)
		return E_FAIL;
	
	// Map Part Prototype 객체들을 Hierarchy에 등록합니다.
	m_pMapTool->Register_Prototype_Hierarchy(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Map_Part"), TEXT("MapPart"));
	
	// Map Part 객체들 Layer
	//m_pMapTool->Register_Layer_Hierarchy(pLayer);
	
	return S_OK;
}


CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Logo::Free()
{
	__super::Free();

	for (auto& Event : m_Events)
		m_pGameInstance->UnSubscribe(Event.first, Event.second);

	Safe_Release(m_pMapTool); // Map Tool은 별개 객체.

}
