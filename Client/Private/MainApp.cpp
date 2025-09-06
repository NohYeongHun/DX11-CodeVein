CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize_Clone()
{
#ifdef _DEBUG
	//AllocConsole();
#endif // DEBUG

	//AllocConsole();
	//
	//// 표준 출력, 에러, 입력 핸들을 콘솔에 연결
	//FILE* fp;
	//
	//freopen_s(&fp, "CONOUT$", "w", stdout); // std::cout
	//freopen_s(&fp, "CONOUT$", "w", stderr); // std::cerr
	//freopen_s(&fp, "CONIN$", "r", stdin);   // std::cin
	//
	//// 콘솔 버퍼 동기화
	//std::ios::sync_with_stdio(true);

	
	
	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
	{
		CRASH("Failed Ready Initialize Engine");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_ForStatic()))
	{
		CRASH("Failed Ready Prototype For Static");
		return E_FAIL;
	}
	
	if (FAILED(Ready_Prototype_Effect()))
	{
		CRASH("Failed Ready Prototype For Static");
		return E_FAIL;
	}

	if (FAILED(Ready_Clone_ForStatic()))
	{
		CRASH("Failed Ready Clone For Static");
		return E_FAIL;
	}
		
	
	if (FAILED(Ready_Pooling()))
	{
		CRASH("Failed Ready Pooling For Static");
		return E_FAIL;
	}


	if (FAILED(Start_Level(LEVEL::GAMEPLAY)))
		return E_FAIL;

	// 원본
	//if (FAILED(Start_Level(LEVEL::LOGO)))
	//	return E_FAIL;

	//if (FAILED(Start_Level(LEVEL::DEBUG)))
	//	return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.0f, 0.0f, 0.f, 1.f);
	
	// bool Return;

	m_pGameInstance->Render_Begin(&vClearColor);
	m_pImGui_Manager->Render_Begin();

	m_pGameInstance->Draw();

	ImGui::Begin("Information");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	m_pImGui_Manager->Render_End();
	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	m_pImGui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pContext);
	/* ==================================================== Shader ====================================================*/

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BehaviourTree"),
	//	CBehaviourTree::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	/* ==================================================== Shader ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		CRASH("Failed Create Shader Cube");
		return E_FAIL;
	}
		

	/* Prototype_Component_Shader_VtxInstance_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Particle.hlsl"), VTXPARTICLE::Elements, VTXPARTICLE::iNumElements))))
	{
		CRASH("Failed Create Shader Particle");
		return E_FAIL;
	}
		

	///* Prototype_Component_Shader_VtxInstance_Point_Particle */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
	//{
	//	MSG_BOX(TEXT("Create Failed Point Particle Shader"));
	//	CRASH("Failed Bind Shader_VtxInstance_PointParticle");
	//	return E_FAIL;
	//}
		



	/* ==================================================== Other ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* ==================================================== COLLIDER ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_FadeOut_Texture")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/FadeOut/FadeOut%d.png"), 1))))
	{
		CRASH("Failed Create FadeOut Texture")
		return E_FAIL;
	}
		

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FadeOut")
	, CFade_Out::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Create Fade Out");
		return E_FAIL;
	}

	/* ==================================================== FSM ====================================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fsm"),
		CFsm::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(Ready_Prototype_ForUsageTexture()))
	{
		CRASH("Failed Load Usage Texture");
		return E_FAIL;
	}
		
	
	/* Model Load */
	if (FAILED(Ready_Prototype_ForPlayer()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_HUD()))
		return E_FAIL;

	

	if (FAILED(Ready_Prototype_Inventory()))
		return E_FAIL;
	
	if (FAILED(Ready_Prototype_SkillUI()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_MonsterUI()))
		return E_FAIL;




	//. 자주 사용하는 얘들 모아둔다.
	if (FAILED(Ready_Prototype_Fonts()))
		return E_FAIL;
	
	
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForUsageTexture()
{
	/* Action Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Action_SkillIcon")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillIcon/Action/Action%d.png"), 12))))
		return E_FAIL;

	/* Status Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_StatusIcon")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Category/Category%d.png"), 10))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForPlayer()
{

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XM_PI);


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Player")
		, CLoad_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Player.dat", L"Player\\"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Model_Sword")
		, CLoad_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, PreTransformMatrix, "../../SaveFile/Model/Player/Sword.dat", L""))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_GameObject_Weapon")
		, CPlayerWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_GameObject_Player")
		, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_HUD()
{

#pragma region Skill Panel
	// Skill Slot UI Texture
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SkillSlot")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillSlot/SkillSlot%d.png"), 3))))
		return E_FAIL;

	// Skill Slot
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillIcon"),
		CSkill_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillSlot"),
		CSkill_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillPanel"),
		CSkill_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	
#pragma region Status Panel
	// Status 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_HPBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/HPBar/HPBar%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SteminaBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/HPBar/Stemina%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HPBar"),
		CHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SteminaBar"),
		CSteminaBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_StatusPanel"),
		CStatusPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HUD"),
		CHUD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region LOCKON
	// LockOn UI 텍스처
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LockOnSite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/LockOn/LockOnSite%d.png"), 1))))
	{
		CRASH("Failed Load LockOnUI Texture");
		return E_FAIL;
	}
		

	// LockOn UI 게임오브젝트
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LockOnUI"),
		CLockOnUI::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load LockOn GameObject ");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Inventory()
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Middle_Inventory")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/MiddleSlot%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_InventorySlot")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Slot%d.png"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Inventory_StatusInfo")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/Inventory/Info/StatusInfo%d.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region OBJECT
	// CInventory 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory_Panel"),
		CInventory_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventorySkill_Slot"),
		CInventorySkill_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventorySkill_Icon"),
		CInventorySkill_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryItem_Slot"),
		CInventoryItem_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryItem_Icon"),
		CInventoryItem_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryStatus_Icon"),
		CInventoryStatus_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryStatus_Info"),
		CInventoryStatus_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_SkillUI()
{
#pragma region TEXTURE
	/* MIDDLE Inventory Texture */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SkillUI_Panel")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/User/SkillUI/Panel%d.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region OBJECT
	// 1. UI Panel 출력부터.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI"),
		CSkillUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Panel"),
		CSkillUI_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Slot"),
		CSkillUI_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI_Icon"),
		CSkillUI_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_MonsterUI()
{

#pragma region BOSS UI
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_BossHPBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Monster/QueenKnight/HPBar%d.png"), 1))))
	{
		CRASH("Failed Create Texture BossHPBar UI");
		return E_FAIL;
	}
		

	// 보스 UI
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BossHPBar"),
		CBossHpBarUI::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Create BossHpBar UI");
		return E_FAIL;
	}
#pragma endregion

#pragma region MONSTER UI
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_MonsterHPBar")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Monster/HPBar/HPBar%d.png"), 1))))
	{
		CRASH("Failed Create Texture MonsterHPBar UI");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterHPBar"),
		CMonsterHpBar::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Create MonsterHpBar UI");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Fonts()
{
	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("KR_TEXT")
			, TEXT("../Bin/Resources/Font/153ex.spritefont"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance
		->Load_Font(
			TEXT("HUD_TEXT")
			, TEXT("../Bin/Resources/Font/Arial.spritefont"))))
		return E_FAIL;



	return S_OK;
}


HRESULT CMainApp::Ready_Clone_ForStatic()
{
	// 주로 교체하면서 사용할 Texture들을 미리 생성해둡니다.
	if (FAILED(Ready_Clone_Texture()))
		return E_FAIL;

	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_HUD(TEXT("Layer_HUD"))))
		return E_FAIL;
	//
	// 생성은 하되 비활성화 해두어야 합니다.
	if (FAILED(Ready_Clone_Inventory(TEXT("Layer_Inventory"))))
		return E_FAIL;

	// 
	if (FAILED(Ready_Clone_SkillUI(TEXT("Layer_SkillUI"))))
		return E_FAIL;


	

	return S_OK;
}

// 가장 먼저 Clone 되어야함.
HRESULT CMainApp::Ready_Clone_Texture()
{
#pragma region SKILL ICON
	/*if (FAILED(m_pGameInstance->Add_Texture(ENUM_CLASS(LEVEL::STATIC),
		TEXT("Prototype_Component_Texture_Action_SkillIcon"),
		TEXT("Action_SkillIcon"))))
		return E_FAIL;*/
#pragma endregion

	return S_OK;
}


HRESULT CMainApp::Ready_Clone_HUD(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HUD"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Clone_Inventory(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventory"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Clone_SkillUI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkillUI"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Effect()
{
#pragma region DISSOLVE
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_Dissolve")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Noise/Noise%d.png"), 4))))
	{
		CRASH("Failed Load Dissolve Texture");
		return E_FAIL;
	}
#pragma endregion


#pragma region SLASH UI

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SlashEffectMask")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Slash/Slash%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_SlashEffectDiffuse")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/Slash/SlashDiffuse%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SlashEffect"),
		CSlash::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load Slash GameObject ");
		return E_FAIL;
	}
#pragma endregion

#pragma region HITFLASTH EFFECT
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_HitFlashEffectMask")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/HitFlash/HitFlashMask%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_HitFlashEffectDiffuse")
		, CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Texture/HitFlash/HitFlashDiffuse%d.png"), 1))))
	{
		CRASH("Failed Load SlashEffect Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"),
		CHitFlashEffect::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load HItFlashEffect GameObject ");
		return E_FAIL;
	}
#pragma endregion


#pragma region PARTICLE



#pragma region TEXTURE OBJECT

	// 0. Texture 종류별 생성.
	for (_uint i = 0; i < Effect_TexturePrototypeSize; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
			, Effect_TexturePrototypes[i].prototypeName
			, CTexture::Create(m_pDevice, m_pContext, Effect_TexturePrototypes[i].textureFilePath
				, Effect_TexturePrototypes[i].iNumTextures))))
		{
			CRASH("Failed Load Effect Texture");
			return E_FAIL;
		}
	}
#pragma endregion

#pragma region 1. Shader 생성.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl")
			, VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
	{
		CRASH("Failed Load Point Particle Shader");
		return E_FAIL;
	}

	// 실제 사용하는 VIBuffer
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointDirParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointDirParticle.hlsl")
			, VTXPOINTDIRPARTICLE::Elements, VTXPOINTDIRPARTICLE::iNumElements))))
	{
		CRASH("Failed Load PointDirection Particle Shader");
		return E_FAIL;
	}
#pragma endregion

#pragma region 2. PARTICLE OBJECT
	if (FAILED(m_pGameInstance->Add_Prototype(
		ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_GameObject_EffectParticle")
		, CEffectParticle::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load Effect Particle Object");
		return E_FAIL;
	}
#pragma endregion


	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Particle"),
		CParticle::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load Effect Particle GameObject ");
		return E_FAIL;
	}
#pragma endregion

#pragma region TRAIL

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectTrail.hlsl")
			, VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		CRASH("Failed Load Point Effect Shader");
		return E_FAIL;
	}

	/*For.Prototype_Component_VIBuffer_Swordtrail*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_SwordTrail"),
		CVIBuffer_SwordTrail::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load SowrdTrail VIBuffer");
		return E_FAIL;
	}
		

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailSword")
		, CTexture::Create(m_pDevice, m_pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/TraillSword%d.png"), 3))))
	{
		CRASH("Failed Load Effect SwordTrail Texture");
		return E_FAIL;
	}

	// 슬래시 디테일 텍스처 등록
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailSlash")
		, CTexture::Create(m_pDevice, m_pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/TraillSlash%d.png"), 2))))
	{
		CRASH("Failed Load Effect TrailSlash Texture");
		return E_FAIL;
	}

	// 발광 효과 텍스처 등록  
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailGlow")
		, CTexture::Create(m_pDevice, m_pContext
			, TEXT("../Bin/Resources/Textures/Effects/Texture/Trail/Trail_SpWeapon%d.png"), 1))))
	{
		CRASH("Failed Load Effect TrailGlow Texture");
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_GameObject_SwordTrail"),
		CSwordTrail::Create(m_pDevice, m_pContext))))
	{
		CRASH("Failed Load SwordTrail Object");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}



HRESULT CMainApp::Ready_Pooling()
{
	 
#pragma region TEXTURE 타입.
	// 1. Prototype Clone 객체 생성.
	CSlash::SLASHEFFECT_DESC slashDesc{};
	slashDesc.eShaderPath = POSTEX_SHADERPATH::MONSTER_LINESLASH;
	CGameObject* pGameObject = nullptr;

	// 2. 추가할 개수만큼 추가. 
	/* 3개 추가. => 풀링 제대로 되는지 테스트. */
	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SlashEffect"), &slashDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create GameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("SLASH_EFFECT"), ENUM_CLASS(CSlash::EffectType), pGameObject);
	}

	CHitFlashEffect::HITFLASH_DESC HitFlashDesc{};
	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
			, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"), &HitFlashDesc));
		if (nullptr == pGameObject)
		{
			CRASH("Failed Create HitFlashGameObject");
			return E_FAIL;
		}
		m_pGameInstance->Add_GameObject_ToPools(TEXT("HITFLASH_EFFECT"), ENUM_CLASS(CHitFlashEffect::EffectType), pGameObject);
	}

#pragma endregion

#pragma region PARTICLE 타입 => 디버그 용도. 
	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT
	//		, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Particle"), &HitFlashDesc));
	//	if (nullptr == pGameObject)
	//	{
	//		CRASH("Failed Create Particle GameObject");
	//		return E_FAIL;
	//	}
	//	m_pGameInstance->Add_GameObject_ToPools(TEXT("Particle_EFFECT"), ENUM_CLASS(EFFECTTYPE::PARTICLE), pGameObject);
	//}

	CEffectParticle::EFFECT_PARTICLE_DESC ParticleDesc{};
	ParticleDesc.iShaderPath = 1;
	ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_QUEEN_WARP;
	//ParticleDesc.eParticleType = CEffectParticle::PARTICLE_TYPE_BOSS_EXPLOSION;
	ParticleDesc.iNumInstance = 400;
	ParticleDesc.vCenter = { 0.f, 0.f, 0.f };
	ParticleDesc.vRange = { 6.f, 6.f, 3.f };
	ParticleDesc.vSpeed = { 3.f, 6.f };
	ParticleDesc.vSize = { 0.1f, 0.11f };
	ParticleDesc.vLifeTime = { 5.f, 6.f };
	ParticleDesc.isLoop = false;
	ParticleDesc.isBillBoard = true;

	
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_DIFFUSE] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 1;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_MASK] = true;
	ParticleDesc.useTextureIndexArray[TEXTURE::TEXTURE_MASK] = 0;

	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_GRADIENT_ALPHA] = false;
	ParticleDesc.useTextureCheckArray[TEXTURE::TEXTURE_NOISE] = false;

	/* Particle Init Info 설정으로 Pool에서 사용할때마다. 값을 채워줍니다. */
	for (_uint i = 0; i < 200; ++i)
	{
		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC)
			, TEXT("Prototype_GameObject_EffectParticle"), &ParticleDesc
		));
		m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_WARP"), ENUM_CLASS(CEffectParticle::EffectType), pGameObject);
	}

#pragma endregion


	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
	{
		CRASH("Failed Start Level");
		return E_FAIL;
	}
		

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	CBase::Free();

	//FreeConsole();



	Safe_Release(m_pImGui_Manager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
