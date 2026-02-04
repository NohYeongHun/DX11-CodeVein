#pragma region  기본 함수들
CSwordWindCircle::CSwordWindCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CSwordWindCircle::CSwordWindCircle(const CSwordWindCircle& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CSwordWindCircle::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordWindCircle::Initialize_Clone(void* pArg)
{
	SWORDWIND_CIRCLE_DESC* pDesc = static_cast<SWORDWIND_CIRCLE_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);
	pDesc->fSpeedPerSec = 10.f;

	m_eCurLevel = pDesc->eCurLevel; // Activate시 지정됌.

#pragma region 기본 변수들 초기화
	m_fDisplayTime = pDesc->fDisplayTime;
	m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
	m_pOwner = pDesc->pOwner;
	
#pragma endregion

	if (FAILED(CGameObject::Initialize_Clone(pDesc)))
		CRASH("Failed Clone Sword Wind");

	// 초기 지정 정보. 
	if (FAILED(Ready_Components(pDesc)))
		CRASH("Failed Clone Sword Wind");


	_float3 vScale = { 5.f, 5.f, 5.f };
	m_pTransformCom->Set_Scale(vScale);
	//m_pTransformCom->Set_Scale(pDesc->vStartScale);
	//m_pTransformCom->Set_Scale({100.f, 100.f, 100.f});
	
	m_IsActivate = false;
	return S_OK;
}



void CSwordWindCircle::Priority_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Priority_Update(fTimeDelta);


}


void CSwordWindCircle::Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Update(fTimeDelta);
	m_fTime += fTimeDelta;

	// ⭐ 상태별 업데이트
	//m_pTransformCom->Update_WorldMatrix();

	/* Scale 키우기. */
	XMLoadFloat3(&m_vStartScale);

	_float fRatio = m_fTime / m_fDisplayTime;
	_float3 vCurScale = {};

	_vector vStartScale = XMLoadFloat3(&m_vStartScale);
	_vector vTargetScale = XMLoadFloat3(&m_vTargetScale);
	XMStoreFloat3(&vCurScale, XMVectorLerp(vStartScale, vTargetScale, fRatio));

	m_pTransformCom->Set_Scale(vCurScale);


	/* 위치 계속 업데이트 */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		m_pTransformCom->Get_WorldMatrix() *
		XMLoadFloat4x4(m_pSocketMatrix) *
		XMLoadFloat4x4(m_pTargetWorldMatrix));
	
}

void CSwordWindCircle::Late_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;

	//* Distortion은 Renderer의 Combine이후에 모든 과정이 끝나고 나서 출력.*/
	//if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DISTORTION, this)))
	//	return;
}

HRESULT CSwordWindCircle::Render()
{
#ifdef _DEBUG
	//ImGui_Render();
#endif // _DEBUG


	if (FAILED(Bind_ShaderResources()))
	{
		CRASH("Ready Render Resource Failed");
		return E_FAIL;
	}


	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		// 기본 Texture를 NoiseTexture로 지정.
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPath);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSwordWindCircle::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources_Distortion()))
	{
		CRASH("Ready Render Resource Distortion Failed");
		return E_FAIL;
	}

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pDistortionShaderCom->Begin(static_cast<_uint>(EFFECTMESH_DISTORTIONSHADERPATH::SWORD_WIND));
		m_pModelCom->Render(i);
	}

	return S_OK;
}
#pragma endregion


#pragma region  풀링시 지정할 정보들
void CSwordWindCircle::OnActivate(void* pArg)
{
	m_IsActivate = true;
	SWORDWIND_CIRCLE_ACTIVATE_DESC* pDesc = static_cast<SWORDWIND_CIRCLE_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();

	m_fDisplayTime = pDesc->fDisplayTime;;
	m_vStartScale = pDesc->vStartScale;
	m_vTargetScale = pDesc->vTargetScale;

	// 0. Transform 정보 초기화 OffSet만큼 설정.
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vStartPos));
	m_pTransformCom->Set_Scale(m_vStartScale);

	// 1. Matrix 추가
	m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;
	m_pSocketMatrix = pDesc->pSocketMatrix;



	

	// Last 상태 및 시간 초기화
	m_eState = STATE_STAY;
	m_fMoveTime = 0.f;
	m_fCurrentTime = 0.f;
	m_fDissolveTime = 0.f;
	m_fDissolveThreshold = 0.f;
}



void CSwordWindCircle::OnDeActivate()
{
	
}

#pragma endregion


void CSwordWindCircle::Calc_Timer(_float fTimeDelta)
{
	
}


HRESULT CSwordWindCircle::Bind_ShaderResources()
{
#pragma region 행렬 바인딩
	// ⭐ 화면을 채우는 효과 강화
	_float4 vIntenseColor = {
		1.0f,
		0.8f + sin(m_fTime * 3.0f) * 0.2f,  // 더 강한 색상 변화
		0.3f + cos(m_fTime * 2.5f) * 0.2f,
		0.8f  // 더 높은 알파값
	};

	_float fFasterScrollSpeed = m_fScrollSpeed * 2.0f;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFresnelColor", &vIntenseColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollSpeed", &fFasterScrollSpeed, sizeof(_float))))
		return E_FAIL;

	// 쉐이더에 바인딩
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
	{
		CRASH("Failed Bind World Matrix");
		return E_FAIL;
	}



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
	{
		CRASH("Failed Bind View Matrix");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
	{
		CRASH("Failed Bind Proj Matrix");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	

	
#pragma region SHADER변수

	_float fRatio = m_fTime / m_fDisplayTime;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		CRASH("Failed Bind Time");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEffectDuration", &m_fDisplayTime, sizeof(_float))))
	{
		CRASH("Failed Bind Time");
		return E_FAIL;
	}

	m_fScrollSpeed = 0.5f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
	{
		CRASH("Failed Bind Time");
		return E_FAIL;
	}

#pragma endregion

	

	
	
#pragma endregion

#pragma region TEXTURE 바인딩.
	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTextures")))
	{
		CRASH("Failed Bind Texture Diffuse Texture ");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom[TEXTURE::TEXTURE_OTHER]->Bind_Shader_Resources(m_pShaderCom, "g_OtherTextures")))
	{
		CRASH("Failed Bind Texture Other Texture ");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom[TEXTURE::TEXTURE_NOISE]->Bind_Shader_Resources(m_pShaderCom, "g_NoiseTextures")))
	{
		CRASH("Failed Bind Texture Other Texture ");
		return E_FAIL;
	}
	
	if (FAILED(m_pTextureCom[TEXTURE::TEXTURE_SWIRL]->Bind_Shader_Resources(m_pShaderCom, "g_SwirlTextures")))
	{
		CRASH("Failed Bind Texture Other Texture ");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

HRESULT CSwordWindCircle::Bind_ShaderResources_Distortion()
{

#pragma region 행렬 바인딩
	// 쉐이더에 바인딩
	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
	{
		CRASH("Failed Bind World Matrix");
		return E_FAIL;
	}


	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
	{
		CRASH("Failed Bind View Matrix");
		return E_FAIL;
	}

	if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
	{
		CRASH("Failed Bind Proj Matrix");
		return E_FAIL;
	}

	if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

#pragma region SHADER 변수

	if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		CRASH("Failed Bind Time");
		return E_FAIL;
	}
	//
	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fEffectDuration", &m_fDisplayTime, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Time");
	//	return E_FAIL;
	//}

	//m_fScrollSpeed = 2.f;
	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Time");
	//	return E_FAIL;
	//}


#pragma endregion






#pragma endregion

#pragma region TEXTURE 바인딩.
	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resources(m_pDistortionShaderCom, "g_DiffuseTextures")))
	{
		CRASH("Failed Bind Texture Diffuse Texture ");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom[TEXTURE::TEXTURE_OTHER]->Bind_Shader_Resources(m_pDistortionShaderCom, "g_OtherTextures")))
	{
		CRASH("Failed Bind Texture Other Texture ");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

HRESULT CSwordWindCircle::Ready_Components(SWORDWIND_CIRCLE_DESC* pDesc)
{
	/* 사용할 쉐이더.*/
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		CRASH("Failed SwordWind Components");


	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMeshDistortion"),
		TEXT("Com_DistortionShader"), reinterpret_cast<CComponent**>(&m_pDistortionShaderCom), nullptr)))
		CRASH("Failed SwordWind Components");

	

	/* 사용할 모델.*/
	CLoad_Model::LOADMODEL_DESC Desc{};
	Desc.pGameObject = this;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Effect_SwordWind"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
		CRASH("Failed SwordWind Components");

	/* 사용할 텍스쳐들 ?*/

	// 1. Diffuse 바인딩 => 늘수도있음. 아마 이건 0번 쓰겠지? 인덱스로
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordWindDiffuse")
		, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_DIFFUSE]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	// 2. Other 바인딩
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordWindOther")
		, TEXT("Com_Other"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_OTHER]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	// 3. Noise 바인딩
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordWindNoise")
		, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_NOISE]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}
	
	// 4. Swirl 바인딩
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordWindSwirl")
		, TEXT("Com_Swirl"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_SWIRL]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_Component_Texture_TrailDistortion")
		, TEXT("Com_DistortionTexture")
		, reinterpret_cast<CComponent**>(&m_pDistortionTexture))))
	{
		CRASH("Failed Create Glow Texture Trail Distortion");
		return E_FAIL;
	}


	// 사용할 인덱스 지정.?
	for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
		m_iTextureIndexArray[i] = pDesc->iTextureIndexArray[i];

	return S_OK;
}



CSwordWindCircle* CSwordWindCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordWindCircle* pInstance = new CSwordWindCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordWindCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordWindCircle::Clone(void* pArg)
{
	CSwordWindCircle* pInstance = new CSwordWindCircle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordWindCircle"));
		Safe_Release(pInstance);
	}
	

	return pInstance;
}

void CSwordWindCircle::Free()
{
	CPartObject::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDistortionShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDistortionTexture);
	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	m_pOwner = nullptr;
	m_pParentMatrix = nullptr;
}


#ifdef _DEBUG
void CSwordWindCircle::ImGui_Render()
{

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(300.f, 300.f);
	ImVec2 windowPos = ImVec2(600.f, 0.f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	ImGui::Begin("Sword Wind Circle Debug", nullptr, ImGuiWindowFlags_NoCollapse);

	_float3 vPos = {};
	memcpy(&vPos, m_CombinedWorldMatrix.m[3], sizeof(_float3));

	ImGui::Text("Sword Wind Circle Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

	static _float Rotation[3] = { 0.f, 0.f, 0.f };

	ImGui::InputFloat3("Rotation ", Rotation);

	if (ImGui::Button("Apply Rotation"))
	{
		m_pTransformCom->Add_Rotation(XMConvertToRadians(Rotation[0])
			, XMConvertToRadians(Rotation[1])
			, XMConvertToRadians(Rotation[2]));
	}

	ImGui::End();
}
#endif // _DEBUG




