HRESULT CLoader_StageOne::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	if (FAILED(Add_Prototype_Map(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : StageOne Map "));
		return E_FAIL;
	}


	if (FAILED(Add_Prototype_Navigation(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Navigation Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne Navigation"));
		return E_FAIL;
	}


	if (FAILED(Add_Prototype_Camera_Free(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Camera Free Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne Camera_Free "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Camera_Player(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Camera Player Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne Camera_Player "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Player(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Player Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne Player "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_GiantWhiteDevil(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create GiantWhiteDevil Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne GiantWhiteDevil "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_WolfDevil(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create WolfDevil Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne WolfDevil "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SlaveVampire(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SlaveVampire Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne SlaveVampire "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SkyBox(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create SkyBox Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne SkyBox "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Snow(pDevice, pContext, pGameInstance)))
	{
		CRASH("Create Snow Failed");
		MSG_BOX(TEXT("Create Failed Loading : StageOne Snow"));
		return E_FAIL;
	}


	return S_OK;
}

// 1. Game 에서 사용할 Model.
HRESULT CLoader_StageOne::Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

  	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_StageOne")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Map/StageOne.dat", L""))))
	{
		CRASH("Failed Load Stage One");
		return E_FAIL;
	}
		

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Map")
		, CMap::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_StageOne::Add_Prototype_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(pDevice, pContext, "../../SaveFile/Navigation/StageOne.nav"))))
	{
		CRASH("Failed Load Navigation File");
		return E_FAIL;
	}

	return S_OK;

}


// 1.
HRESULT CLoader_StageOne::Add_Prototype_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{


	return S_OK;
}


HRESULT CLoader_StageOne::Add_Prototype_GiantWhiteDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	// 무기
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_WhiteLargeHalberd")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Weapon/WhiteLargeHalberd.dat", L""))))
	{
		CRASH("Failed Load Model WhiteLargeHalberd");
		return E_FAIL;
	}
		

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_WhiteLargeHalberd")
		, CWhiteLargeHalberd::Create(pDevice, pContext))))
	{
		CRASH("Failed Create WhiteLargeHalberd Weapon")
		return E_FAIL;
	}
		

	// 본체
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_Giant_WhiteDevil")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Boss/GiantWhiteDevil.dat", L""))))
	{
		CRASH("Failed Load Model GiantWhite Devil")
		return E_FAIL;
	}
		

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_GameObject_Giant_WhiteDevil")
		, CGiant_WhiteDevil::Create(pDevice, pContext))))
	{
		CRASH("Failed Create White Giant Devil");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_StageOne::Add_Prototype_WolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
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

HRESULT CLoader_StageOne::Add_Prototype_SlaveVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
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


	return S_OK;
}


HRESULT CLoader_StageOne::Add_Prototype_Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// Free 시점 카메라 생성.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

/* Camera Player Create */
HRESULT CLoader_StageOne::Add_Prototype_Camera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(pDevice, pContext))))
	{
		CRASH("Ready Failed Camera Player");
		return E_FAIL;
	}
		

	return S_OK;
}


HRESULT CLoader_StageOne::Add_Prototype_SkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_SkyStageOne"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/SkyBox/StageOne%d.dds"), 1))))
	{
		CRASH("Failed Create SkyTexture");
		return E_FAIL;
	}
		

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	// SkySphere
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Model_SkySphere")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Sky/SkySphere.dat", L""))))
	{
		CRASH("Failed Create SkySphere Prototype");
		return E_FAIL;
	}
		


	/* Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(pDevice, pContext))))
	{
		CRASH("Sky Box Create Failed");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoader_StageOne::Add_Prototype_Snow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	{
		CRASH("Failed Load Texture Snow");
		return E_FAIL;
	}

	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		SnowDesc{};
	SnowDesc.iNumInstance = 3000;
	SnowDesc.vCenter = _float3(64.f, 20.f, 64.f);
	SnowDesc.vRange = _float3(128.f, 1.f, 128.f);
	SnowDesc.vSize = _float2(0.1f, 0.2f);
	SnowDesc.vLifeTime = _float2(5.0f, 10.f);
	SnowDesc.vPivot = _float3(0.f, 0.f, 0.f);
	SnowDesc.vSpeed = _float2(1.5f, 3.f);
	SnowDesc.isLoop = true;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_Component_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(pDevice, pContext, &SnowDesc))))
	{
		CRASH("Failed VIBuffer Point Instance Snow");
		return E_FAIL;
	}
		
	///* Prototype_GameObject_Snow */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Snow"),
		CSnow::Create(pDevice, pContext))))
	{
		CRASH("Failed Create Snow Object");
		return E_FAIL;
	}
		
		


	return S_OK;
}

