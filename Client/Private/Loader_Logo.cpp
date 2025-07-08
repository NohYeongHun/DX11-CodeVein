#include "Loader_Logo.h"

HRESULT CLoader_Logo::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(Add_Prototype_Title(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Title Object "));
		return E_FAIL;
	}

	/*if (FAILED(Add_Prototype_BackGround(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Background Object "));
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Title"),
		CTitle::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/BackGround/Logo%d.png"), 1))))
		return E_FAIL;


#pragma region Title Text
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title_Text")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/Text/Text%d.png"), 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Title_Text"),
		CTitleText::Create(pDevice, pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Title BackGround
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title_BackGround")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/BackGround/BackGround%d.png"), 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Title_BackGround"),
		CTitle_BackGround::Create(pDevice, pContext))))
		return E_FAIL;
#pragma endregion

	

	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Block(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}
