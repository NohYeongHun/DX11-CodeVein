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

	if (FAILED(Add_Prototype_SlaveVampire(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SlaveVampire Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay SlaveVampire "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SkyBox(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SkyBox Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay SkyBox "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_UI(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create UI Failed");
		MSG_BOX(TEXT("Create Failed Loading : GamePlay UI "));
		return E_FAIL;
	}

	
	return S_OK;
}

// 1. Game 에서 사용할 Model.
HRESULT CLoader_GamePlay::Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_BossStage")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Map/BossStageType1.dat", L"BossStage\\"))))
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

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(pDevice, pContext, "../../SaveFile/Navigation/BossStage.nav"))))
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
	
	// 방패
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_GodChildShield")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Weapon/GodChildShield.dat", L""))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_GodChildShield")
		, CKnightShield::Create(pDevice, pContext))))
	{
		CRASH("Failed Create QueenKnight Shield")
		return E_FAIL;
	}
		

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

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_WolfWeapon")
		, CWolfWeapon::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_SlaveVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_SlaveVampire")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Monster/SlaveVampire.dat", L""))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_SlaveVampire")
		, CSlaveVampire::Create(pDevice, pContext))))
		return E_FAIL;



	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// 무기
 	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_SlaveVampireGreatSword")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Weapon/SlaveGreatSword.dat", L""))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_SlaveVampireSword")
		, CSlaveVampireSword::Create(pDevice, pContext))))
		return E_FAIL;


	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
	//	, TEXT("Prototype_GameObject_WolfWeapon")
	//	, CWolfWeapon::Create(pDevice, pContext))))
	//	return E_FAIL;

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
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/SkyBox/SkyStar%d.dds"), 1))))
	//	return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// SkySphere
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_SkySphere")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Sky/SkySphere.dat", L""))))
		return E_FAIL;


	/* Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(pDevice, pContext))))
	{
		CRASH("Sky Box Create Failed");
		return E_FAIL;
	}
		

	return S_OK;
}

HRESULT CLoader_GamePlay::Add_Prototype_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	
	return S_OK;
}


