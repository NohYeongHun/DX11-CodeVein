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


	if (FAILED(Add_Prototype_Navigation(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Navigation Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay Navigation"));
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

	if (FAILED(Add_Prototype_SkyBoss(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SkyBoss Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay SkyBoss "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_QueenKnight(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SkyBoss Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay QueenKnight "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_WolfDevil(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create WolfDevil Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay WolfDevil "));
		return E_FAIL;
	}


	if (FAILED(Add_Prototype_SkyBox(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SkyBox Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay SkyBox "));
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


// 1. Game 에서 사용할 Model.
HRESULT CLoader_GamePlay::Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_BossStage")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Map/BossStage.dat", L"BossStage\\"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Map")
		, CMap::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Prototype_Component_Navigation */
	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(pDevice, pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	{
		CRASH("Failed Load Navigation File");
		return E_FAIL;
	}*/

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(pDevice, pContext, "../../SaveFile/Navigation/Navigation.nav"))))
	{
		CRASH("Failed Load Navigation File");
		return E_FAIL;
	}

	return S_OK;

}


// 1.
HRESULT CLoader_GamePlay::Add_Prototype_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Player Prototype 생성.
	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Player_Body")
		, CPlayer_Body::Create(pDevice, pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Weapon")
		, CPlayerWeapon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Player")
		, CPlayer::Create(pDevice, pContext))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_SkyBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_SkyBoss")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Boss/SkyBoss.dat", L""))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_SkyBoss")
		, CSkyBoss::Create(pDevice, pContext))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_SkyBossTree")
		, CSKyBossTree::Create(pDevice, pContext))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_QueenKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
	//	, TEXT("Prototype_Component_Model_BlackKnight")
	//	, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Boss/BlackKnight.dat", L""))))
	//	return E_FAIL;

	

	// 방패
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_GodChildShield")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Weapon/GodChildShield.dat", L""))))
		return E_FAIL;


	// 무기
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_GodChildLance")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Weapon/GodChildLance.dat", L""))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_GodChildLance")
		, CKnightLance::Create(pDevice, pContext))))
		return E_FAIL;

	// 본체
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_QueenKnight")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Boss/QueenKnight.dat", L""))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_QueenKnight")
		, CQueenKnight::Create(pDevice, pContext))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_WolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_WolfDevil")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Monster/WolfDevil.dat", L""))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_WolfDevil")
		, CWolfDevil::Create(pDevice, pContext))))
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


HRESULT CLoader_GamePlay::Add_Prototype_SkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/SkyBox/SkyStar%d.dds"), 1))))
	//	return E_FAIL;

	/* Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(pDevice, pContext))))
	{
		CRASH("Sky Box Create Failed");
		return E_FAIL;
	}
		

	return S_OK;
}


