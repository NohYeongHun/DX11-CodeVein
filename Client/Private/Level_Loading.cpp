#include "Level_Loading.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize_Clone(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;	 

	if (FAILED(Ready_LoadingScene()))
		return E_FAIL;

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	/* 다음 레벨을 위한 로딩작업을 시작 한다. */
	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;

	// Event로 Loading을 끝냅니다.
	if (FAILED(Ready_Events()))
		return E_FAIL;
	
	return S_OK;
}

/*
* Loading 끝나면 Fade In Fade Out 되면서 전환?
*/
void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished() && 
		GetKeyState(VK_SPACE) & 0x8000)
	{
		// Fade Out 시작. => 나중에는 로더가 끝나는 시점? isFinshed 받으면?
		m_pGameInstance->Publish<CLoading_BackGround>(EventType::LOADING_END, nullptr);
	}
}

HRESULT CLevel_Loading::Render()
{
	/* 생성해놓은 객체들을 렌더한다. */
	m_pLoader->Show_LoadingText();

	return S_OK;
}

void CLevel_Loading::Open_Level()
{
	CLevel* pNewLevel = { nullptr };

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
		break;
	case LEVEL::GAMEPLAY:
		pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
		break;
	}

	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(m_eNextLevelID), pNewLevel)))
		return;
}

HRESULT CLevel_Loading::Ready_LoadingBackGround(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingScene()
{
	LOADINGEVENT_DESC Desc{};
	Desc.isVisibility = true;
	m_pGameInstance->Publish<LOADINGEVENT_DESC>(EventType::LOAIDNG_DISPLAY, & Desc);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_GameObjects()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingThread()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Events()
{
	// Event 등록
	m_pGameInstance->Subscribe(EventType::OPEN_LEVEL, Get_ID(), [this](void* pData)
		{
			this->Open_Level();
		});

	// Event 목록 관리.
	m_Events.emplace_back(EventType::OPEN_LEVEL, Get_ID());

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();


	// 2. Loading Display를 종료한다. => Level Loading이 끝나면.
	LOADINGEVENT_DESC Desc{};
	Desc.isVisibility = false;
	m_pGameInstance->Publish<LOADINGEVENT_DESC>(EventType::LOAIDNG_DISPLAY,  & Desc);


	
	// 3. 지울 때 제거
	for (auto& Event : m_Events)
		m_pGameInstance->UnSubscribe(Event.first, Event.second);
	

	Safe_Release(m_pLoader);

}
