
CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}


HRESULT CLoader::Initialize_Clone(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성하고 */
	/* 생성한 스레드가 로딩을 할 수 있도록 처리한다. */

	/* 스택 메모리를 제외한 기타 다른 메모리공간(힙, 데이터, 코드, ) 은 스레드간 서로 공유한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT			hr = {};

	switch(m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{

	lstrcpy(m_szLoadingText, TEXT("로고 레벨을 로딩중입니다."));

#pragma region MAP
	// 1. Model 로딩.
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	// Map Prototype 생성.
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO)
	//	, TEXT("Prototype_Component_Model_Map")
	//	, CTool_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../Bin/Resources/Models/Map/BossMap/BossMap.glb"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO)
	//	, TEXT("Prototype_GameObject_Map")
	//	, CMap::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma endregion


#pragma region MAP PART
// Map Prototype 생성.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO)
		, TEXT("MapPart_BluePillar")
		, CTool_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../Bin/Resources/Models/Map/BossMap/BluePillar.glb"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO)
		, TEXT("MapPart_Pillar")
		, CTool_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../Bin/Resources/Models/Map/BossMap/Pillar.glb"))))
		return E_FAIL;

	

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO)
		, TEXT("Prototype_GameObject_Map_Part")
		, CMap_Part::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region CAMERA
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	
	
	lstrcpy(m_szLoadingText, TEXT("로고 레벨 로딩이 완료되었습니다."));

	m_isFinished = true;
	
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	lstrcpy(m_szLoadingText, TEXT("게임플레이 레벨을 로딩중입니다."));

	

	lstrcpy(m_szLoadingText, TEXT("게임플레이 레벨 로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}
CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();
	
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
