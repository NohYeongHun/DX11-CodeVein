
CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext }
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& Prototype)
    : CGameObject( Prototype )
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype");
        return E_FAIL;
    }
        
    return S_OK;
}

HRESULT CSwordTrail::Initialize_Clone(void* pArg)
{

	SWORDTRAIL_DESC* pDesc = static_cast<SWORDTRAIL_DESC*>(pArg);

	/*
	* 생성 시 TransformDesc의 fSpeedPerSec = 5.f
	* 생성 시 TransformDesc의 fRotationPerSec = XMConvertToRadians(1.0f);
	*/
	if (FAILED(CGameObject::Initialize_Clone(pDesc)))
	{
		CRASH("Failed Ready_Components");
		return E_FAIL;
	}

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }
        
	m_eCurLevel = pDesc->eCurLevel;
    m_iShaderID = 2; // StretchTrail 패스 사용 (Ribbon Trail)

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDesc->pTarget);

	// 색상은 텍스처 원본을 사용하므로 더 이상 필요하지 않음
	// m_vColorFront = { 102, 255, 255, 255 };
	// m_vColorBack = { 12, 76, 255, 255 };
	m_fAlpha = 1.2f; // 기본 알파값 증가

    
    return S_OK;
} 

void CSwordTrail::Update(_float fTimeDelta)
{
    // 시간 누적 (Ribbon Trail용)
    m_fTime += fTimeDelta;
    
    CVIBuffer_SwordTrail::TRAILPOINT TrailPointDesc;

    TrailPointDesc.vPointUp = m_vPoint_Up;
    TrailPointDesc.vPointDown = m_vPointDown;

    TrailPointDesc.vTrailMatrix = m_WeaponMatrix;

    m_pVIBufferCom->Update(TrailPointDesc);
}

void CSwordTrail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CSwordTrail::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

#ifdef _DEBUG
    //Edit_Collider(m_pColliderCom, "Player Weapon");
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 300.f);
    ImVec2 windowSize = ImVec2(00.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Player Weapon Trail Debug", nullptr, ImGuiWindowFlags_NoCollapse);
	
	static _float TrailPos[3] = { m_WeaponMatrix.m[3][0], m_WeaponMatrix.m[3][1], m_WeaponMatrix.m[3][2] };

	ImGui::Text("Trail Pos : (%.2f, %.2f, %.2f)", TrailPos[0], TrailPos[1], TrailPos[2]);

	ImGui::Text("Current Cell Count : %d", m_pVIBufferCom->Get_CurrentPoint());

    ImGui::End();

#endif // _DEBUG



	if (FAILED(Bind_ShaderResources()))
	{
		CRASH("Failed Ready BindShader Resources");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(m_iShaderID)))
	{
		CRASH("Failed Begin Shader");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
	{
		CRASH("Failed VIBufferCom Bind Resources");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		CRASH("Failed VIBufferCom Render");
		return E_FAIL;
	}

	return S_OK;
}

void CSwordTrail::Update_Trail_Point(_float3 vPointDown, _float3 vPointUp, _fmatrix WeaponMatrix)
{
	m_vPointDown = vPointDown;
	m_vPoint_Up = vPointUp;

	XMStoreFloat4x4(&m_WeaponMatrix, WeaponMatrix);
} 

HRESULT CSwordTrail::Ready_Components(SWORDTRAIL_DESC* pDesc)
{
	m_eCurLevel = pDesc->eCurLevel;
	
	// 기본 검 궤적 텍스처 (TraillSword0)
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailSword")
		,TEXT("Com_BaseTexture")
	, reinterpret_cast<CComponent**>(&m_pBaseTexture))))
	{
		CRASH("Failed Create Base Texture Sword Trail");
		return E_FAIL;
	}

	// 슬래시 디테일 텍스처 (TraillSlash0)
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailSlash")
		,TEXT("Com_DetailTexture")
	, reinterpret_cast<CComponent**>(&m_pDetailTexture))))
	{
		CRASH("Failed Create Detail Texture Sword Trail");
		return E_FAIL;
	}

	// 발광 효과 텍스처 (Trail_SpWeapon0)
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailGlow")
		,TEXT("Com_GlowTexture")
	, reinterpret_cast<CComponent**>(&m_pGlowTexture))))
	{
		CRASH("Failed Create Glow Texture Sword Trail");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_VIBuffer_SwordTrail")
		, TEXT("Com_VIBuffer")
		, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		CRASH("Failed Create Texture Sword Trail");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Shader_Effect")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

    return S_OK;
}

HRESULT CSwordTrail::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// 트레일 정점은 이미 월드 좌표계로 변환된 상태이므로 단위 행렬 사용
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom,"g_WorldMatrix")))
	{
		CRASH("Failed Bind Identity WorldMatrix");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
	{
		CRASH("Failed Bind View Matrix SwordTrail");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
	{
		CRASH("Failed Bind Proj Matrix SwordTrail");
		return E_FAIL;
	}

	// SP_Weapon 텍스처를 리본 트레일로 사용
	if (FAILED(m_pGlowTexture->Bind_Shader_Resource(m_pShaderCom, "g_GlowTexture", 0)))
	{
		CRASH("Failed Bind SP_Weapon Texture as GlowTexture");
		return E_FAIL;
	}

	if (FAILED(m_pBaseTexture->Bind_Shader_Resource(m_pShaderCom, "g_BaseTexture", 0)))
	{
		CRASH("Failed Bind SP_Weapon Texture as GlowTexture");
		return E_FAIL;
	}

	// Ribbon Trail 파라미터 바인딩
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fTime, sizeof(_float))))
	{
		CRASH("Failed Bind Time");
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
	{
		CRASH("Failed Bind Scroll Speed");
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_FadePower", &m_fFadePower, sizeof(_float))))
	{
		CRASH("Failed Bind Fade Power");
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Intensity", &m_fIntensity, sizeof(_float))))
	{
		CRASH("Failed Bind Intensity");
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TrailColor", &m_vTrailColor, sizeof(_vector))))
	{
		CRASH("Failed Bind Trail Color");
		return E_FAIL;
	}
		
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
		
		


    return S_OK;
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordTrail::Clone(void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrail::Free()
{
	CGameObject::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBaseTexture);
	Safe_Release(m_pDetailTexture);
	Safe_Release(m_pGlowTexture);
}
