#pragma region  기본 함수들
CSwordWind::CSwordWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CSwordWind::CSwordWind(const CSwordWind& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CSwordWind::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordWind::Initialize_Clone(void* pArg)
{
	SWORDWIND_DESC* pDesc = static_cast<SWORDWIND_DESC*>(pArg);
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


	//m_pTransformCom->Set_Scale(pDesc->vStartScale);
	//m_pTransformCom->Set_Scale({100.f, 100.f, 100.f});
	
	m_IsActivate = false;
	

	return S_OK;
}



void CSwordWind::Priority_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Priority_Update(fTimeDelta);


}


void CSwordWind::Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Update(fTimeDelta);
	m_fTime += fTimeDelta;

	// ⭐ 상태별 업데이트
	switch (m_eState)
	{
	case STATE_STAY:
		Update_Stay(fTimeDelta);
		break;
	case STATE_MOVE:
		Update_RotateMove(fTimeDelta);
		break;
	case STATE_DECREASE:
		Update_Decrease(fTimeDelta);
		break;
	}

	m_pTransformCom->Update_WorldMatrix();

	// ⭐ 소켓이 있을 때만 업데이트, 없으면 고정
	if (m_pSocketMatrix && m_pTargetTransform)
	{
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMLoadFloat4x4(m_pSocketMatrix) *
			m_pTargetTransform->Get_WorldMatrix());
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix());
	}
}

void CSwordWind::Late_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;

	///* Distortion은 Renderer의 Combine이후에 모든 과정이 끝나고 나서 출력.*/
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DISTORTION, this)))
		return;
}

HRESULT CSwordWind::Render()
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

HRESULT CSwordWind::Render_Distortion()
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
void CSwordWind::OnActivate(void* pArg)
{
	m_IsActivate = true;
	SWORDWIND_ACTIVATE_DESC* pDesc = static_cast<SWORDWIND_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();

	m_fMoveDuration = pDesc->fMoveDuration;
	m_fStayDuration = pDesc->fStayDuration;
	m_fDecreaseDuration = pDesc->fDecreaseDuration;
	m_fDisplayTime = m_fMoveDuration + m_fStayDuration + m_fDecreaseDuration;
	m_fDuration = m_fMoveDuration + m_fStayDuration + m_fDecreaseDuration;

	// ⭐ Transform 정보 초기화
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vStartPos));

	// ⭐ 카메라를 바라보도록 초기 회전 설정
	_matrix viewMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
	_matrix viewInverse = XMMatrixInverse(nullptr, viewMatrix);
	_vector vCamPos = viewInverse.r[3];
	_vector vToCamera = XMVector3Normalize(vCamPos - XMLoadFloat3(&pDesc->vStartPos));
	_float fYaw = atan2f(XMVectorGetX(vToCamera), XMVectorGetZ(vToCamera));
	m_pTransformCom->Add_Rotation(0.f, fYaw, 0.f); // Y축 회전만 적용

	// ⭐ 크기 설정: 작게 시작해서 목표 크기로 커짐
	_float3 vInitialScale = { 2.0f, 2.0f, 2.0f };
	m_pTransformCom->Set_Scale(vInitialScale);
	m_vStartScale = vInitialScale;
	m_vTargetScale = pDesc->vStartScale;

	// ⭐ 상태 및 시간 초기화
	m_eState = STATE_STAY;
	m_fMoveTime = 0.f;
	m_fCurrentTime = 0.f;
	m_fDissolveTime = 0.f;
	m_fDissolveThreshold = 0.f;
}



void CSwordWind::OnDeActivate()
{
	
}

#pragma endregion


void CSwordWind::Calc_Timer(_float fTimeDelta)
{
	
}


void CSwordWind::Update_Stay(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= m_fStayDuration)
	{
		m_eState = STATE_MOVE;
		m_fCurrentTime = 0.f;
		return;
	}
	// ⭐ 제자리에서 계속 회전
	//m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 5.f);
}

void CSwordWind::Update_RotateMove(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;

	if (m_fMoveTime >= m_fMoveDuration)
	{
		m_eState = STATE_DECREASE;
		m_fCurrentTime = 0.f;
		m_pTransformCom->Set_Scale(m_vTargetScale);
		return;
	}

	_float fRatio = m_fMoveTime / m_fMoveDuration;

	_matrix viewMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
	_matrix viewInverse = XMMatrixInverse(nullptr, viewMatrix);
	_vector vCamPos = viewInverse.r[3];
	_vector vCamLook = XMVector3Normalize(viewInverse.r[2]);

	_vector vStartPos = XMLoadFloat3(&m_ActivateDesc.vStartPos);

	_float fBehindDistance = 7.0f;
	_vector vTargetPos = vCamPos - (vCamLook * fBehindDistance);

	_float fStartY = XMVectorGetY(vStartPos);
	_float fCamY = XMVectorGetY(vCamPos);
	_float fTargetY = fStartY + (fCamY - fStartY) * 0.3f; // 약간만 높이 변화
	vTargetPos = XMVectorSetY(vTargetPos, fTargetY);

	_float fEasedRatio = 1.f - powf(1.f - fRatio, 3.f);
	_vector vCurrentPos = XMVectorLerp(vStartPos, vTargetPos, fEasedRatio);
	m_pTransformCom->Set_State(STATE::POSITION, vCurrentPos);

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 2.f);

	_float3 vCurrentScale;
	_float fScaleRatio = 1.f - powf(1.f - fRatio, 2.f);
	XMStoreFloat3(&vCurrentScale, XMVectorLerp(XMLoadFloat3(&m_vStartScale), XMLoadFloat3(&m_vTargetScale), fScaleRatio));
	m_pTransformCom->Set_Scale(vCurrentScale);
}




void CSwordWind::Update_Decrease(_float fTimeDelta)
{
	m_fDissolveTime += fTimeDelta;

	if (m_fDissolveTime >= m_fDecreaseDuration)
	{
		m_eState = STATE_END;
		m_IsActivate = false;
		return;
	}

	_float fRatio = m_fDissolveTime / m_fDecreaseDuration;

	//// ⭐ 크기가 점차 작아지면서 사라짐
	//_float fScaleRatio = 1.f - (fRatio * fRatio);
	//_float3 vNewScale = {
	//	m_vTargetScale.x * fScaleRatio,
	//	m_vTargetScale.y * fScaleRatio,
	//	m_vTargetScale.z * fScaleRatio
	//};
	//m_pTransformCom->Set_Scale(vNewScale);

	// ⭐ 계속 회전
	//m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 10.f);

	m_fDissolveThreshold = fRatio; // 셰이더용 Dissolve 값
}





HRESULT CSwordWind::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
	{
		CRASH("Failed Bind World Matrix");
		return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
	//{
	//	CRASH("Failed Bind World Matrix");
	//	return E_FAIL;
	//}


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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveThresold", &m_fDissolveThreshold, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	_float fRatio = m_fTime / m_fDuration;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	_float fMoveRatio = m_fMoveTime / m_fMoveDuration;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMoveTime", &fMoveRatio, sizeof(_float))))
	{
		CRASH("Failed Bind g_fGrowTime");
		return E_FAIL;
	}


	_float fDissolveRatio = m_fDissolveTime / m_fDecreaseDuration;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveRatio, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}


	_float4 vDynamicFresnelColor = {
		1.0f,
		0.2f + sin(m_fTime * 2.0f) * 0.1f,  // 초록 성분 변화
		0.1f + cos(m_fTime * 1.5f) * 0.05f, // 파랑 성분 변화
		1.0f
	};

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFresnelColor", &vDynamicFresnelColor, sizeof(_float4))))
		return E_FAIL;
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

HRESULT CSwordWind::Bind_ShaderResources_Distortion()
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

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
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

HRESULT CSwordWind::Ready_Components(SWORDWIND_DESC* pDesc)
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



CSwordWind* CSwordWind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordWind* pInstance = new CSwordWind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordWind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordWind::Clone(void* pArg)
{
	CSwordWind* pInstance = new CSwordWind(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordWind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordWind::Free()
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
void CSwordWind::ImGui_Render()
{

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowPos = ImVec2(0.f, 0.f);
	ImVec2 windowSize = ImVec2(300.f, 300.f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	ImGui::Begin("Sword Wind Debug", nullptr, ImGuiWindowFlags_NoCollapse);

	_float3 vPos = {};
	memcpy(&vPos, m_CombinedWorldMatrix.m[3], sizeof(_float3));

	ImGui::Text("Sword Wind Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

	ImGui::End();
}
#endif // _DEBUG




