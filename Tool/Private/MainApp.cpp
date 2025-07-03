
#include "MainApp.h"
#include "GameInstance.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
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

	/*if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;*/

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);



	m_pGameInstance->Render_Begin(&vClearColor);
	
	// Render Begin Render End 사이에 넣어야함.
#ifdef _DEBUG
	m_pImGui_Manager->Render_Begin();
#endif // _DEBUG

	
	//m_pImGui_Manager->Render();
	m_pMapTool->ImGui_Render();

	m_pGameInstance->Draw();

#ifdef _DEBUG
	m_pImGui_Manager->Render_End();
#endif // _DEBUG

	m_pGameInstance->Render_End();
	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);

	m_pMapTool = CMap_Tool::Create(m_pDevice, m_pContext);

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_STATIC), TEXT("Prototype_Component_Transform"),
	//	CTransform::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	return S_OK;
}

/* Font Rendering */
HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../../Resources/Font/CodeVein.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Console()
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
	__super::Free();

	FreeConsole();

	Safe_Release(m_pMapTool);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pImGui_Manager);
}
