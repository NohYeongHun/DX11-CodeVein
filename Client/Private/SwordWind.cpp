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


	m_pTransformCom->Set_Scale(pDesc->vStartScale);
	
	
	

	return S_OK;
}


void CSwordWind::Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Update(fTimeDelta);
	m_fTime += fTimeDelta; // 시간은 계속 셰이더로 보내야 합니다.

	_float3 vScale = m_vStartScale; // 시작 스케일을 기본으로 설정
	_float fAnimTime = m_fTime - m_fCreateTime;

	if (fAnimTime < 0.f)
	{
		m_pTransformCom->Set_Scale(_float3(0.f, 0.f, 0.f));
	}
	else
	{
		if (fAnimTime < m_fGrowDuration) // 나타나는 단계
		{
			_float fRatio = fAnimTime / m_fGrowDuration;
			vScale.x *= fRatio;
			vScale.y *= fRatio;
		}
		else if (fAnimTime < m_fGrowDuration + m_fStayDuration) // 유지 단계
		{
			// 최대 크기 유지
			vScale = m_vStartScale;
		}
		else if (fAnimTime < m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration) // 사라지는 단계
		{
			_float fDecreaseTime = fAnimTime - m_fGrowDuration - m_fStayDuration;
			_float fRatio = 1.0f - (fDecreaseTime / m_fDecreaseDuration);
			vScale.x *= fRatio;
			vScale.y *= fRatio;
		}
		else
		{
			OnDeActivate();
			return;
		}
		m_pTransformCom->Set_Scale(vScale);
	}


	m_pTransformCom->Update_WorldMatrix();
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}


void CSwordWind::Priority_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Priority_Update(fTimeDelta);


}


void CSwordWind::Late_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;

	/* Distortion은 Renderer의 Combine이후에 모든 과정이 끝나고 나서 출력.*/
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DISTORTION, this)))
		return;
}

HRESULT CSwordWind::Render()
{
	

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
		// 기본 Texture를 NoiseTexture로 지정.
		if (FAILED(m_pModelCom->Bind_Materials(m_pDistortionShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;


		//m_pDistortionShaderCom->Begin(static_cast<_uint>(DISTORTION_SHADERPATH::SWORD_WIND));
		m_pDistortionShaderCom->Begin(static_cast<_uint>(DISTORTION_SHADERPATH::SWORD_WIND));
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

	m_IsGrowing = true;
	m_fGrowDuration = pDesc->fGrowDuration;
	m_fStayDuration = pDesc->fStayDuration;
	m_fDecreaseDuration = pDesc->fDecreaseDuration;
	m_vRotation = pDesc->vStartRotation;
	m_fDisplayTime = m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration;
	m_vRotationAxis = pDesc->vRotationAxis;
	m_fRotationSpeed = pDesc->fRotationSpeed;
	m_fDissolveTime = 0.f;
	m_fDissolveThreshold = 0.f; // 초기화

	m_pParentMatrix = pDesc->pParentMatrix;
	m_pTargetTransform = pDesc->pTargetTransform;

	m_fCreateTime = pDesc->fCreateTime; // 순차적 생성 시간
	m_vStartScale = pDesc->vStartScale;

	// 카메라 기준 위치 설정 (기존 코드 유지)
	CTransform* pCameraTransform = m_pGameInstance->Get_MainCamera()->Get_Transform();

	_vector vTargetPos = pCameraTransform->Get_State(STATE::POSITION);
	_vector vTargetRight = pCameraTransform->Get_State(STATE::RIGHT);
	_vector vTargetUp = pCameraTransform->Get_State(STATE::UP);
	_vector vTargetLook = pCameraTransform->Get_State(STATE::LOOK);

	_float fEyeHeight = 0.f;
	_vector vEyeLevelPos = vTargetPos + (XMVector3Normalize(vTargetUp) * fEyeHeight);

	_float fDistance = 7.f;
	vTargetLook = XMVector3Normalize(vTargetLook);
	_vector vFinalPos = vEyeLevelPos + (vTargetLook * fDistance);

	m_pTransformCom->Set_State(STATE::RIGHT, vTargetRight);
	m_pTransformCom->Set_State(STATE::UP, vTargetUp);
	m_pTransformCom->Set_State(STATE::LOOK, vTargetLook);
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);

	// 초기 크기를 0으로 설정 (순차적 생성 효과)
	if (m_fCreateTime > 0.f)
	{
		m_pTransformCom->Set_Scale(_float3(0.f, 0.f, 0.f));
	}
	else
	{
		m_pTransformCom->Set_Scale(pDesc->vStartScale);
	}

	// 초기 회전은 유지하되, Update에서는 회전시키지 않음
	m_pTransformCom->Add_Rotation(
		XMConvertToRadians(pDesc->vStartRotation.x),
		XMConvertToRadians(pDesc->vStartRotation.y),
		XMConvertToRadians(pDesc->vStartRotation.z)
	);

	m_eState = STATE_GROW;
}


void CSwordWind::OnDeActivate()
{
	
}

#pragma endregion


void CSwordWind::Calc_Timer(_float fTimeDelta)
{
	
}

void CSwordWind::Shape_Control(_float fTimeDelta)
{
	//RotateTurn_ToAxis(fTimeDelta);

	m_fCurrentTime += fTimeDelta;

	/*switch (m_eState)
	{
	case STATE_GROW:
		Update_Grow(fTimeDelta);
		break;
	case STATE_STAY:
		Update_Stay(fTimeDelta);
		break;
	case STATE_DECREASE:
		Update_Decrease(fTimeDelta);
		break;
	}*/

	_float3 vScale = m_pTransformCom->Get_Scale();

	if (!m_IsGrowing)
		return;

	
}

// z가 높이임.
void CSwordWind::Update_Grow(_float fTimeDelta)
{
	_float fRatio = m_fCurrentTime / m_fGrowDuration;
	m_fGrowTime += fTimeDelta;

}

void CSwordWind::Update_Stay(_float fTimeDelta)
{
	// 유지 시간 완료 시, 다음 상태로 전환
	if (m_fCurrentTime >= m_fStayDuration)
	{
		m_eState = STATE_DECREASE; // '감소' 상태로 변경
		m_fCurrentTime = 0.f;     // 타이머 리셋!
	}
	// 이 상태에서는 아무것도 하지 않고 그냥 크기를 유지합니다.
}

void CSwordWind::Update_Decrease(_float fTimeDelta)
{
	
	// 감소 완료 시, 종료 상태로 전환
	if (m_fCurrentTime >= m_fDecreaseDuration)
	{
		m_eState = STATE_END; // '종료' 상태로 변경
		// Set_Dead() 같은 함수로 객체를 비활성화하거나 풀에 반납
		return;
	}

	m_fDissolveTime += fTimeDelta;
	_float fRatio = m_fCurrentTime / m_fDecreaseDuration;

	fRatio = fRatio * fRatio;

	
}




/* Combined 곱하기 전에 실행. => Update*/
void CSwordWind::RotateTurn_ToAxis(_float fTimeDelta)
{
	if (m_pTransformCom)
	{
		// X축으로 -50도 기울어진 객체를 고려한 회전축 계산
		// 원래 Z축 회전이 되려면, 기울어진 만큼 보정해야 함

		// 방법 1: 월드 공간의 Y축으로 회전 (수직축 회전)
		_vector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		// 방법 2: 기울어진 객체의 로컬 Forward 방향으로 회전
		// X축 -50도 회전을 고려한 보정된 회전축
		// _float fTiltAngle = XMConvertToRadians(-50.f);
		// _vector vAxis = XMVectorSet(sinf(fTiltAngle), cosf(fTiltAngle), 0.f, 0.f);

		// 방법 3: 카메라 기준이 아닌 객체 자체의 로컬 Z축으로 회전
		// _matrix matLocal = m_pTransformCom->Get_WorldMatrix();
		// _vector vAxis = XMVector3Normalize(matLocal.r[2]);

		_float fRotationSpeed = XMConvertToRadians(120.f);
		_float fRotationAngle = fRotationSpeed * fTimeDelta;

		m_pTransformCom->Turn(vAxis, fRotationAngle);
	}
}


HRESULT CSwordWind::Bind_ShaderResources()
{
#pragma region 행렬 바인딩
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

	//_float fSmokeDensity = 1.5f; // 연기 밀도
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSmokeDensity", &fSmokeDensity, sizeof(_float))))
	//	return E_FAIL;

	//_float fSmokeSpeed = 0.5f; // 연기 이동 속도
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fSmokeSpeed", &fSmokeSpeed, sizeof(_float))))
	//	return E_FAIL;

	//// 색상 조정
	//_float4 vSmokeColor = { 0.8f, 0.85f, 0.9f, 1.0f }; // 연회색
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vSmokeColor", &vSmokeColor, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveThresold", &m_fDissolveThreshold, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	_float fRatio = m_fTime / m_fDisplayTime;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	_float fGrowRatio = m_fGrowTime / m_fGrowDuration;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGrowTime", &fGrowRatio, sizeof(_float))))
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




#pragma region SHADER변수

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fDissolveThresold", &m_fDissolveThreshold, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
	//	return E_FAIL;
	//}

	//_float fRatio = m_fTime / m_fDisplayTime;
	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
	//	return E_FAIL;
	//}

	//_float fGrowRatio = m_fGrowTime / m_fGrowDuration;
	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fGrowTime", &fGrowRatio, sizeof(_float))))
	//{
	//	CRASH("Failed Bind g_fGrowTime");
	//	return E_FAIL;
	//}


	//_float fDisolveRatio = m_fDissolveTime / m_fDecreaseDuration;


	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fDissolveTime", &m_fDissolveThreshold, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
	//	return E_FAIL;
	//}


	//_float4 vDynamicFresnelColor = {
	//	1.0f,
	//	0.2f + sin(m_fTime * 2.0f) * 0.1f,  // 초록 성분 변화
	//	0.1f + cos(m_fTime * 1.5f) * 0.05f, // 파랑 성분 변화
	//	1.0f
	//};

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_vFresnelColor", &vDynamicFresnelColor, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fDissolveThresold", &m_fDissolveThreshold, sizeof(_float))))
	//{
	//	CRASH("Failed Bind Cam Position");
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
		CRASH("Failed BloodPillarA Components");

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
	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	m_pOwner = nullptr;
	m_pParentMatrix = nullptr;
}


#ifdef _DEBUG
void CSwordWind::ImGui_Render()
{

}
#endif // _DEBUG

