
#include "MainApp.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize_Clone()
{
#ifdef _DEBUG
	AllocConsole();
#endif // DEBUG
	
	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;


	if (FAILED(Ready_Clone_ForStatic()))
		return E_FAIL;

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
	//_float4		vClearColor = _float4(0.7f, 0.7f, 0.7f, 0.1f);
	_float4		vClearColor = _float4(0.0f, 0.0f, 1.f, 1.f);


	
	m_pGameInstance->Render_Begin(&vClearColor);
	m_pImGui_Manager->Render_Begin();

	m_pGameInstance->Draw();

	//m_pImGui_Manager->Render();
	m_pImGui_Manager->Render_End();
	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(Ready_Prototype_ForSkillTexture()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_HUD()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Inventory()))
		return E_FAIL;


	//. 자주 사용하는 얘들 모아둔다.
	if (FAILED(Ready_Prototype_Fonts()))
		return E_FAIL;
	
	
#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForSkillTexture()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Action_SkillIcon")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillIcon/Action/Action%d.png"), 12))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_HUD()
{

#pragma region Skill Panel
	// Skill Slot UI Texture
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SkillSlot")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillSlot/SkillSlot%d.png"), 3))))
		return E_FAIL;

	// Skill Slot
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillIcon"),
		CSkill_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillSlot"),
		CSkill_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillPanel"),
		CSkill_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	
#pragma region Status Panel
	// Status 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_HPBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/HPBar/HPBar%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SteminaBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/HPBar/Stemina%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HPBar"),
		CHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SteminaBar"),
		CSteminaBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_StatusPanel"),
		CStatusPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HUD"),
		CHUD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Inventory()
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Middle_Inventory")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/MiddleSlot%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_InventorySlot")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Slot%d.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region OBJECT
	// CInventory 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory_Panel"),
		CInventory_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventorySkill_Slot"),
		CInventorySkill_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventorySkill_Icon"),
		CInventorySkill_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryItem_Slot"),
		CInventoryItem_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryItem_Icon"),
		CInventoryItem_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Fonts()
{
 	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../Bin/Resources/Font/143.spritefont"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainApp::Ready_Clone_ForStatic()
{
	// 주로 교체하면서 사용할 Texture들을 미리 생성해둡니다.
	if (FAILED(Ready_Clone_Texture()))
		return E_FAIL;

	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_HUD(TEXT("Layer_HUD"))))
		return E_FAIL;

	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_Inventory(TEXT("Layer_Inventory"))))
		return E_FAIL;


	

	return S_OK;
}

// 가장 먼저 Clone 되어야함.
HRESULT CMainApp::Ready_Clone_Texture()
{
#pragma region SKILL ICON
	if (FAILED(m_pGameInstance->Add_Texture(ENUM_CLASS(LEVEL::STATIC),
		TEXT("Prototype_Component_Texture_Action_SkillIcon"),
		TEXT("Action_SkillIcon"))))
		return E_FAIL;
#pragma endregion

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





HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
 	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

#ifdef _DEBUG
	FreeConsole();
#endif // DEBUG

	Safe_Release(m_pImGui_Manager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
