#include "Loader_GamePlay.h"

HRESULT CLoader_GamePlay::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(Add_Prototype_Terrain(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Terrain "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Camera_Free(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Camera_Free "));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Terrain 용도 텍스쳐.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	// Terrain 생성.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Free 시점 카메라 생성.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

