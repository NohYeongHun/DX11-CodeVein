#include "Loader_GamePlay.h"

HRESULT CLoader_GamePlay::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(Add_Prototype_Terrain(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Terrain "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Map(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Map "));
		return E_FAIL;
	}


	if (FAILED(Add_Prototype_Camera_Free(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Camera Free Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Camera_Free "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Camera_Player(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Camera Player Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Camera_Player "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Player(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Player Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Player "));
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


// 1.
HRESULT CLoader_GamePlay::Add_Prototype_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Player Prototype 생성.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Player")
		, CPlayer::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

// 1. Game 에서 사용할 Model.
HRESULT CLoader_GamePlay::Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
 	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_CircleFloor")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Map/CircleFloor.dat", L"CircleFloor\\"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_BossStage")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Map/BossStage.dat", L"BossStage\\"))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_BossStage")
		, CModel::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../Bin/Resources/Models/Map/BossMap/BossMap.fbx"))))
		return E_FAIL;*/
	
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Map")
		, CMap::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Terrain 용도 텍스쳐.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 1))))
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

/* Camera Player Create */
HRESULT CLoader_GamePlay::Add_Prototype_Camera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

