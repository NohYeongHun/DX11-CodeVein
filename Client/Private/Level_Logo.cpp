#include "Level_Logo.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize_Clone()
{

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Title(TEXT("Layer_Title"))))
		return E_FAIL;

	if (FAILED(Ready_Events()))
		return E_FAIL;
	
	
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (!m_IsLogoEnd)
	{
		m_IsLogoEnd = true;
		m_pGameInstance->Publish<CTitle>(EventType::LOGO_END, nullptr);
		return;
	}

	/*if (!m_IsLogoEnd && m_pGameInstance->Get_KeyUp(DIK_RETURN))
	{
		m_IsLogoEnd = true;
		m_pGameInstance->Publish<CTitle>(EventType::LOGO_END, nullptr);
		return;
	}*/

	return;
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

void CLevel_Logo::Open_Level()
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
		return;
}

HRESULT CLevel_Logo::Ready_Layer_Title(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), strLayerTag,
		ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_Title"))))
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
	CLevel::Free();

	for (auto& Event : m_Events)
		m_pGameInstance->UnSubscribe(Event.first, Event.second);

}
