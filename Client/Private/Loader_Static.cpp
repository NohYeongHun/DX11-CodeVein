#include "Loader_Static.h"
HRESULT CLoader_Static::Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(Add_Prototype_ForShader(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Shader  "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_ForVIBuffer(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static VIBuffer "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_ForCollider(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Collider "));
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_ForFadeout(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static FadeOut "));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Fsm(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Fsm "));
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_Texture(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Texture"));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_ForPlayer(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Player"));
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_HUD(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static HUD"));
		return E_FAIL;
	}


	if (FAILED(Add_Prototype_Inventory(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Inventory"));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SkillUI(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static SkillUI"));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_MonsterUI(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static MonsterUI"));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Fonts(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Fonts"));
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Effects(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Effects"));
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_ParticleSystem(pDevice, pContext, pGameInstance)))
	{
		MSG_BOX(TEXT("Create Failed Loading : Static Particle System"));
		return E_FAIL;
	}



    return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ForShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	/* ==================================================== Shader ====================================================*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxEffectPosTex"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		CRASH("Failed Create Shader Cube");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		CRASH("Failed Create Shader Cube");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxMeshDistortion"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshDistortion.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		CRASH("Failed Create Shader Cube");
		return E_FAIL;
	}

	


	


    return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ForVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(pDevice, pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ForCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(pDevice, pContext, COLLIDER::AABB))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(pDevice, pContext, COLLIDER::OBB))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(pDevice, pContext, COLLIDER::SPHERE))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ForFadeout(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_FadeOut_Texture")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/FadeOut/FadeOut%d.png"), 1))))
	{
		CRASH("Failed Create FadeOut Texture")
			return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_FadeOut")
		, CFade_Out::Create(pDevice, pContext))))
	{
		CRASH("Failed Create Fade Out");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Fsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Fsm"),
		CFsm::Create(pDevice, pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	/* Action Skill */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_Action_SkillIcon")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/SkillIcon/Action/Action%d.png"), 12))))
		return E_FAIL;

	/* Status Icon */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_StatusIcon")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Category/Category%d.png"), 10))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ForPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Player")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Player.dat", L"Player\\"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Sword")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Sword.dat", L""))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Weapon")
		, CPlayerWeapon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Player")
		, CPlayer::Create(pDevice, pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_HUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region Skill Panel
	// Skill Slot UI Texture
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SkillSlot")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/SkillSlot/SkillSlot%d.png"), 3))))
		return E_FAIL;

	// Skill Slot
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillIcon"),
		CSkill_Icon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillSlot"),
		CSkill_Slot::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillPanel"),
		CSkill_Panel::Create(pDevice, pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Status Panel
	// Status 
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_HPBar")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/HPBar/HPBar%d.png"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SteminaBar")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/HPBar/Stemina%d.png"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_HPBar"),
		CHPBar::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SteminaBar"),
		CSteminaBar::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_StatusPanel"),
		CStatusPanel::Create(pDevice, pContext))))
		return E_FAIL;

#pragma endregion


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_HUD"),
		CHUD::Create(pDevice, pContext))))
		return E_FAIL;

#pragma region LOCKON
	// LockOn UI 텍스처
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Texture_LockOnSite"),
		CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/LockOn/LockOnSite%d.png"), 1))))
	{
		CRASH("Failed Load LockOnUI Texture");
		return E_FAIL;
	}


	// LockOn UI 게임오브젝트
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_LockOnUI"),
		CLockOnUI::Create(pDevice, pContext))))
	{
		CRASH("Failed Load LockOn GameObject ");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_Middle_Inventory")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/Inventory/MiddleSlot%d.png"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_InventorySlot")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Slot%d.png"), 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_Inventory_StatusInfo")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Info/StatusInfo%d.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region OBJECT
	// CInventory 
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Inventory_Panel"),
		CInventory_Panel::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventorySkill_Slot"),
		CInventorySkill_Slot::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventorySkill_Icon"),
		CInventorySkill_Icon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventoryItem_Slot"),
		CInventoryItem_Slot::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventoryItem_Icon"),
		CInventoryItem_Icon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventoryStatus_Icon"),
		CInventoryStatus_Icon::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_InventoryStatus_Info"),
		CInventoryStatus_Info::Create(pDevice, pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_SkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SkillUI_Panel")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/User/SkillUI/Panel%d.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region OBJECT
	// 1. UI Panel 출력부터.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillUI"),
		CSkillUI::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillUI_Panel"),
		CSkillUI_Panel::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillUI_Slot"),
		CSkillUI_Slot::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SkillUI_Icon"),
		CSkillUI_Icon::Create(pDevice, pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_MonsterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region BOSS UI
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BossHPBar")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Monster/QueenKnight/HPBar%d.png"), 1))))
	{
		CRASH("Failed Create Texture BossHPBar UI");
		return E_FAIL;
	}


	// 보스 UI
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_BossHPBar"),
		CBossHpBarUI::Create(pDevice, pContext))))
	{
		CRASH("Failed Create BossHpBar UI");
		return E_FAIL;
	}
#pragma endregion

#pragma region MONSTER UI
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_MonsterHPBar")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Monster/HPBar/HPBar%d.png"), 1))))
	{
		CRASH("Failed Create Texture MonsterHPBar UI");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_MonsterHPBar"),
		CMonsterHpBar::Create(pDevice, pContext))))
	{
		CRASH("Failed Create MonsterHpBar UI");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance
		->Load_Font(
			TEXT("KR_TEXT")
			, TEXT("../Bin/Resources/Font/153ex.spritefont"))))
		return E_FAIL;


	if (FAILED(pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../Bin/Resources/Font/Arial.spritefont"))))
		return E_FAIL;

	return S_OK;
}

#pragma region EFFECT
HRESULT CLoader_Static::Add_Prototype_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE 생성
	// 0. Texture 종류별 생성.
	for (_uint i = 0; i < Effect_TexturePrototypeSize; ++i)
	{
		if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
			, ClientEffect_TexturePrototypes[i].prototypeName
			, CTexture::Create(pDevice, pContext, ClientEffect_TexturePrototypes[i].textureFilePath
				, ClientEffect_TexturePrototypes[i].iNumTextures))))
		{
			CRASH("Failed Load Effect Texture");
			return E_FAIL;
		}
	}
#pragma endregion

	if (FAILED(Add_Prototype_Dissolve_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Slash_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_Renketsu_Slash_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}

	
	if (FAILED(Add_Prototype_HitFlash_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_Particle_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}
	
	if (FAILED(Add_Prototype_Trail_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_BloodPillar_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed Clone Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SwordWind_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed CLone SwordWind Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_SwordWindCircle_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed CLone SwordWind Effects");
		return E_FAIL;
	}

	if (FAILED(Add_Prototype_BloodAura_Effects(pDevice, pContext, pGameInstance)))
	{
		CRASH("Failed CLone SwordWind Effects");
		return E_FAIL;
	}
		


	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Dissolve_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_Dissolve")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Noise/Noise%d.png"), 4))))
	{
		CRASH("Failed Load Dissolve Texture");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Slash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region SLASH Effect

	// Diffuse
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SlashEffectDiffuse")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Slash/Diffuse%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SlashEffectOther")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Slash/Other%d.png"), 5))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SlashEffect"),
		CSlash::Create(pDevice, pContext))))
	{
		CRASH("Failed Load Slash GameObject ");
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Renketsu_Slash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region RENKETSU Slash Effect

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxEffectPosTexDistortion"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectPosTexDistortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		CRASH("Failed Load Point Effect Shader");
		return E_FAIL;
	}
	// Distortion
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SlashDistortion")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/RenketsuSlash/Distortion%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	// Diffuse
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_RenketsuDiffuse")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/RenketsuSlash/Diffuse%d.png"), 2))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	// Mask
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_RenketsuMask")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/RenketsuSlash/Mask%d.png"), 2))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	// Other
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_RenketsuOther")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/RenketsuSlash/Other%d.png"), 3))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Renketsu_SlashEffect"),
		CRenketsuSlash::Create(pDevice, pContext))))
	{
		CRASH("Failed Load RenketsuSlash GameObject ");
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_HitFlash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region HITFLASTH EFFECT
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_HitFlashDiffuse")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/HitFlash/Diffuse%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_HitFlashOpacity")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/HitFlash/Opacity%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_HitFlashOther")
		, CTexture::Create(pDevice, pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/HitFlash/Other%d.png"), 4))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_HitEffect"),
		CHitFlashEffect::Create(pDevice, pContext))))
	{
		CRASH("Failed Load HItFlashEffect GameObject ");
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Particle_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region PARTICLE


#pragma region 3. Shader 생성.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl")
			, VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
	{
		CRASH("Failed Load Point Particle Shader");
		return E_FAIL;
	}

	// 실제 사용하는 VIBuffer
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxInstance_PointDirParticle"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointDirParticle.hlsl")
			, VTXPOINTDIRPARTICLE::Elements, VTXPOINTDIRPARTICLE::iNumElements))))
	{
		CRASH("Failed Load PointDirection Particle Shader");
		return E_FAIL;
	}
#pragma endregion

#pragma region 3. PARTICLE OBJECT
	

	if (FAILED(pGameInstance->Add_Prototype(
		ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_EffectParticle")
		, CEffectParticle::Create(pDevice, pContext))))
	{
		CRASH("Failed Load Effect Particle Object");
		return E_FAIL;
	}
	
#pragma endregion
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_Trail_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TRAIL

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_Effect"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectTrail.hlsl")
			, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		CRASH("Failed Load Point Effect Shader");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxSwordTrailDistortion"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectTrailDistortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		CRASH("Failed Load Point Effect Shader");
		return E_FAIL;
	}

	/*For.Prototype_Component_VIBuffer_Swordtrail*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_VIBuffer_SwordTrail"),
		CVIBuffer_SwordTrail::Create(pDevice, pContext))))
	{
		CRASH("Failed Load SowrdTrail VIBuffer");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_TrailSword")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/TraillSword%d.png"), 3))))
	{
		CRASH("Failed Load Effect SwordTrail Texture");
		return E_FAIL;
	}

	// 슬래시 디테일 텍스처 등록
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_TrailSlash")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/TraillSlash%d.png"), 2))))
	{
		CRASH("Failed Load Effect TrailSlash Texture");
		return E_FAIL;
	}

	// 발광 효과 텍스처 등록  
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_TrailGlow")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/Trail_SpWeapon%d.png"), 1))))
	{
		CRASH("Failed Load Effect TrailGlow Texture");
		return E_FAIL;
	}

	// 왜곡 효과 텍스처 등록  
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_TrailDistortion")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/Distortion%d.png"), 1))))
	{
		CRASH("Failed Load Effect TrailDistortion Texture");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_TrailGradient")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/Gradient%d.png"), 7))))
	{
		CRASH("Failed Load Effect SwordTrail Texture");
		return E_FAIL;
	}



	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_SwordTrail"),
		CSwordTrail::Create(pDevice, pContext))))
	{
		CRASH("Failed Load SwordTrail Object");
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{





	CVIBuffer_Particle_Instance::PARTICLE_INSTANCE_DESC ParticleInstanceDesc{};
	ParticleInstanceDesc.iMaxNumInstance = 500;

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_CVIBuffer_Particle_Instance"),
		CVIBuffer_Particle_Instance::Create(pDevice, pContext, &ParticleInstanceDesc))))
	{
		CRASH("Failed Load Point Particle Shader");
		return E_FAIL;
	}

	// Particle System 전용.
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_ParticleSystem"),
		CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointDirParticle.hlsl")
			, VTXPOINTDIRPARTICLE::Elements, VTXPOINTDIRPARTICLE::iNumElements))))
	{
		CRASH("Failed Load PointDirection Particle Shader");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(
		ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_QueenKnight_ParticleSystem")
		, CParticleSystem::Create(pDevice, pContext))))
	{
		CRASH("Failed Load Effect Particle System");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_BloodPillar_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE
	// 발광 효과 텍스처 등록  
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodPillarDiffuse")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/Diffuse/Diffuse%d.png"), 6))))
	{
		CRASH("Failed Load Effect TrailGlow Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodPillarOther")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/Other/Other%d.png"), 12))))
	{
		CRASH("Failed Load Effect TrailGlow Texture");
		return E_FAIL;
	}


#pragma endregion


#pragma region 모델
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	/* Prototype_Component_Model */


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_BloodPillarA")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/BloodA.dat", L""))))
		CRASH("Failed Prototype Blood Pillar Effects");



	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_BloodPillarB")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/BloodB.dat", L""))))
		CRASH("Failed Prototype Blood Pillar Effects");


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_BloodPillarC")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/BloodC.dat", L""))))
		CRASH("Failed Prototype Blood Pillar Effects");


#pragma endregion

	

	/* 부품 메시들. */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_BloodPillarA")
		, CBlood_PillarA::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Pillar Effects");

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_BloodPillarB")
		, CBlood_PillarB::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Pillar Effects");

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_BloodPillarC")
		, CBlood_PillarC::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Pillar Effects");


	/* 최종 명령을 내리는 주체.*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_EffectPillar")
		, CEffect_Pillar::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Pillar Effects");



	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_SwordWind_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE
	// 발광 효과 텍스처 등록  
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SwordWindDiffuse")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/SwordWind/Diffuse/Diffuse%d.png"), 7))))
	{
		CRASH("Failed Load Effect SwordDiffuse Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SwordWindOther")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/SwordWind/Other/Other%d.png"), 13))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SwordWindNoise")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/SwordWind/Noise/Noise%d.png"), 5))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_SwordWindSwirl")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/SwordWind/Swirl/Swirl%d.png"), 6))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}


#pragma endregion

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_SwordWind")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/SwordWind.dat", L""))))
	{
		CRASH("Failed Prototype_Component_Model_Effect_SwordWind");
		return E_FAIL;
	}
		
	/* 부품 메시들. */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_SwordWind")
		, CSwordWind::Create(pDevice, pContext))))
		CRASH("Failed Prototype Sword Wind Effects");

	/* 최종 명령을 내리는 주체.*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_EffectWind")
		, CEffect_Wind::Create(pDevice, pContext))))
		CRASH("Failed Prototype Effects Wind");

	
	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_SwordWindCircle_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	/* 부품 메시들. */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_SwordWindCircle")
		, CSwordWindCircle::Create(pDevice, pContext))))
		CRASH("Failed Prototype Sword Wind Effects");

	/* 최종 명령을 내리는 주체.*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_EffectWindCircle")
		, CEffect_WindCircle::Create(pDevice, pContext))))
		CRASH("Failed Prototype Effects WindCircle");

	return S_OK;
}

HRESULT CLoader_Static::Add_Prototype_BloodAura_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
#pragma region TEXTURE
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodAuraDiffuse")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/BloodAura/Diffuse/Diffuse%d.png"), 5))))
	{
		CRASH("Failed Load Effect BloodAura Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodAuraOther")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/BloodAura/Other/Other%d.png"), 13))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodAuraNoise")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/BloodAura/Noise/Noise%d.png"), 8))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Texture_BloodAuraSwirl")
		, CTexture::Create(pDevice, pContext
			, TEXT("../Bin/Resources/Models/EffectMesh/BloodAura/Swirl/Swirl%d.png"), 6))))
	{
		CRASH("Failed Load Effect SwordOther Texture");
		return E_FAIL;
	}


#pragma endregion

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);

	/* 바닥 객체. */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_BloodFloorAura")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/BloodFloorAura.dat", L""))))
	{
		CRASH("Failed Prototype_Component_Model_Effect_BloodFloorAura");
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_BloodFloorAura")
		, CEffect_FloorAura::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Aura Effects");

	/* Body 객체. */
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_Component_Model_Effect_BloodBodyAura")
		, CLoad_Model::Create(pDevice, pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Effect/BloodBodyAura.dat", L""))))
	{
		CRASH("Failed Prototype_Component_Model_Effect_BloodBodyAura");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_BloodBodyAura")
		, CEffect_BodyAura::Create(pDevice, pContext))))
		CRASH("Failed Prototype Blood Body Aura Effects");


	/* 최종 명령을 내리는 주체.*/
	if (FAILED(pGameInstance->Add_Prototype(ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_EffectPlayerAuraContainer")
		, CEffect_PlayerSkill::Create(pDevice, pContext))))
		CRASH("Failed Prototype Effects PlayerSkill");
	return S_OK;
}


#pragma endregion

