#include "Loader_Logo.h"

HRESULT CLoader_Logo::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(Add_Prototype_Title(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Title Component "));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_Title"),
		CTitle::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/Logo%d.png"), 1))))
		return E_FAIL;


#pragma region Title BackGround
	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_Title"),
	//	CTitle::Create(pDevice, pContext))))
	//	return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title_BackGround")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/Text%d.png"), 4))))
		return E_FAIL;*/

#pragma endregion


#pragma region Title Text
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level), TEXT("Prototype_GameObject_TitleText"),
		CTitleText::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCur_Level)
		, TEXT("Prototype_Component_Texture_Title_Text")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Title/Text%d.png"), 5))))
		return E_FAIL;
#pragma endregion



	//if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))

	

	return S_OK;
}

HRESULT CLoader_Logo::Add_Prototype_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	// lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	///* Prototype_Component_Texture_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXTURE::RECT, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXTURE::RECT, TEXT("../Bin/Resources/Textures/Player/Player0.png"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Explosion */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXTURE::RECT, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;


	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXTURE::CUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	// lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	///* Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

	///* Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//
	//lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	//
	//lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	///* Prototype_GameObject_Terrain*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Camera*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Camera"),
	//	CCamera::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	// lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
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
