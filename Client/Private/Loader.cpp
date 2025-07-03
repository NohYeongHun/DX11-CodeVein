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


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* �����带 �����ϰ� */
	/* ������ �����尡 �ε��� �� �� �ֵ��� ó���Ѵ�. */

	/* ���� �޸𸮸� ������ ��Ÿ �ٸ� �޸𸮰���(��, ������, �ڵ�, ) �� �����尣 ���� �����Ѵ�. */
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

	lstrcpy(m_szLoadingText, TEXT("�ΰ� ������ �ε����Դϴ�."));

	if (FAILED(m_cLoader_Logo
		.Loading_Resource(m_pDevice, m_pContext, m_pGameInstance)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ΰ� ���� �ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	lstrcpy(m_szLoadingText, TEXT("�����÷��� ������ �ε����Դϴ�."));

	if (FAILED(m_cLoader_GamePlay
		.Loading_Resource(m_pDevice, m_pContext, m_pGameInstance)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�����÷��� ���� �ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;
	return S_OK;
}
CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
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
