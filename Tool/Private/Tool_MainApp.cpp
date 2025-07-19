#include "Tool_MainApp.h"
#include "GameInstance.h"

CTool_MainApp::CTool_MainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTool_MainApp::Initialize()
{
	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;
	

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Console()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void CTool_MainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CTool_MainApp::Render()
{
	
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);
	
	// Render Begin Render End 사이에 넣어야함.
	m_pImGui_Manager->Render_Begin();
	
	m_pGameInstance->Draw();

	m_pImGui_Manager->Render_End();

	m_pGameInstance->Render_End();
	return S_OK;
}



HRESULT CTool_MainApp::Ready_Prototype_ForStatic()
{
	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	//m_pMapTool = CMap_Tool::Create(m_pDevice, m_pContext);

	/* ==================================================== Shader ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* ==================================================== Other ====================================================*/

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
	//	, TEXT("Prototype_Component_Model_Player")
	//	, CTool_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../Bin/Resources/Models/Player/Player.fbx", ""))))
	//	return E_FAIL;

	return S_OK;
}


/* Font Rendering */
HRESULT CTool_MainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../../Resources/Font/CodeVein.spritefont"))))
		return E_FAIL;


	/*if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../../Resources/Font/NEXONLv1Gothic.spritefont"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CTool_MainApp::Ready_Console()
{
	AllocConsole(); // 콘솔 창 생성

	// 콘솔 입출력 리디렉션
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout); // std::cout
	freopen_s(&fDummy, "CONOUT$", "w", stderr); // std::cerr
	freopen_s(&fDummy, "CONIN$", "r", stdin);   // std::cin

	std::ios::sync_with_stdio(); // iostream 동기화

	std::cout << "Console created!" << std::endl;
	return S_OK;
}

HRESULT CTool_MainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}


CTool_MainApp* CTool_MainApp::Create()
{
	CTool_MainApp* pInstance = new CTool_MainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTool_MainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_MainApp::Free()
{
	__super::Free();

	FreeConsole();

	Safe_Release(m_pMapTool);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pImGui_Manager);
}
