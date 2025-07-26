
#include "MainApp.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize_Clone()
{
//#ifdef _DEBUG
//	AllocConsole();
//#endif // DEBUG

	AllocConsole();

	// 표준 출력, 에러, 입력 핸들을 콘솔에 연결
	FILE* fp;

	freopen_s(&fp, "CONOUT$", "w", stdout); // std::cout
	freopen_s(&fp, "CONOUT$", "w", stderr); // std::cerr
	freopen_s(&fp, "CONIN$", "r", stdin);   // std::cin

	// 콘솔 버퍼 동기화
	std::ios::sync_with_stdio(true);

	
	
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
	_float4		vClearColor = _float4(0.0f, 0.0f, 1.f, 1.f);
	
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


HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	/* ==================================================== Shader ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	/* ==================================================== Other ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* ==================================================== FSM ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fsm"),
		CFsm::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(Ready_Prototype_ForUsageTexture()))
		return E_FAIL;
	
	/* Model Load */
	if (FAILED(Ready_Prototype_ForModel()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_HUD()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Inventory()))
		return E_FAIL;
	
	if (FAILED(Ready_Prototype_SkillUI()))
		return E_FAIL;


	//. 자주 사용하는 얘들 모아둔다.
	if (FAILED(Ready_Prototype_Fonts()))
		return E_FAIL;
	
	
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForUsageTexture()
{
	/* Action Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Action_SkillIcon")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillIcon/Action/Action%d.png"), 12))))
		return E_FAIL;

	/* Status Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_StatusIcon")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Category/Category%d.png"), 10))))
		return E_FAIL;

	return S_OK;
}

// 2. Model Prototype Static에 생성.
HRESULT CMainApp::Ready_Prototype_ForModel()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model */
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Player")
		, CLoad_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Player.dat", L"Player\\"))))
		return E_FAIL;

	/* Non Anim Test */
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Player")
		, CModel::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, PreTransformMatrix, "../Bin/Resources/Models/Player/Player.fbx", "../Bin/Resources/Models/Player/Textures/Player/"))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Player")
		, CModel::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, PreTransformMatrix, "../Bin/Resources/Models/Fiona/Fiona.fbx", "/"))))
		return E_FAIL;*/



	
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Player")
		, CLoad_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Fiona.dat", L""))))
		return E_FAIL;*/

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


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Inventory_StatusInfo")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Info/StatusInfo%d.png"), 1))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryStatus_Icon"),
		CInventoryStatus_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryStatus_Info"),
		CInventoryStatus_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_SkillUI()
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SkillUI_Panel")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillUI/Panel%d.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region OBJECT
	// 1. UI Panel 출력부터.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI"),
		CSkillUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Panel"),
		CSkillUI_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Slot"),
		CSkillUI_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Icon"),
		CSkillUI_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Fonts()
{
 	/*if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../Bin/Resources/Font/143.spritefont"))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../Bin/Resources/Font/Arial.spritefont"))))
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
	//
	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_Inventory(TEXT("Layer_Inventory"))))
		return E_FAIL;

	// 
	if (FAILED(Ready_Clone_SkillUI(TEXT("Layer_SkillUI"))))
		return E_FAIL;


	

	return S_OK;
}

// 가장 먼저 Clone 되어야함.
HRESULT CMainApp::Ready_Clone_Texture()
{
#pragma region SKILL ICON
	/*if (FAILED(m_pGameInstance->Add_Texture(ENUM_CLASS(LEVEL::STATIC),
		TEXT("Prototype_Component_Texture_Action_SkillIcon"),
		TEXT("Action_SkillIcon"))))
		return E_FAIL;*/
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

HRESULT CMainApp::Ready_Clone_SkillUI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI"))))
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

	FreeConsole();



	Safe_Release(m_pImGui_Manager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
