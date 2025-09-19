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
	/*m_pParentMatrix = pDesc->pParentMatrix;*/
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

	m_fTime += fTimeDelta;

	// 1. 크기 확장 애니메이션 (전진 없음)
	_float3 vScale = m_pTransformCom->Get_Scale();

	vScale.x += fTimeDelta;
	vScale.y += fTimeDelta;
	vScale.z += fTimeDelta;
	m_pTransformCom->Set_Scale(vScale);
	//if (m_fTime < m_fGrowDuration) // 성장 단계
	//{
	//	_float fGrowRatio = m_fTime / m_fGrowDuration;
	//	// Ease-out 곡선으로 부드러운 확장
	//	fGrowRatio = 1.0f - pow(1.0f - fGrowRatio, 3.0f);

	//	// X축(가로)은 크게, Y,Z축은 적당히
	//	vScale.x = m_vStartScale.x + (m_vStartScale.x * 4.0f * fGrowRatio); // 최대 5배
	//	vScale.y = m_vStartScale.y + (m_vStartScale.y * 1.5f * fGrowRatio); // 최대 2.5배
	//	vScale.z = m_vStartScale.z + (m_vStartScale.z * 0.5f * fGrowRatio); // 최대 1.5배

	//	m_pTransformCom->Set_Scale(vScale);
	//}

	//// 2. 약간의 전진만 (옵션)
	//if (m_fTime < m_fGrowDuration * 0.5f) // 초반에만 약간 전진
	//{
	//	_vector vForward = m_pTransformCom->Get_State(STATE::LOOK);
	//	_float fMoveSpeed = 2.0f * (1.0f - m_fTime / (m_fGrowDuration * 0.5f)); // 점점 느려짐
	//	m_pTransformCom->Move_Direction(vForward, fMoveSpeed * fTimeDelta);
	//}

	//// 3. 디졸브 처리 (빠르게 사라짐)
	//if (m_fTime > m_fDisplayTime * 0.5f) // 50% 시점부터 사라지기 시작
	//{
	//	float fDissolveProgress = (m_fTime - m_fDisplayTime * 0.5f) / (m_fDisplayTime * 0.5f);
	//	m_fDissolveThreshold = fDissolveProgress;
	//}

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
#pragma endregion


#pragma region  풀링시 지정할 정보들
void CSwordWind::OnActivate(void* pArg)
{
	m_IsActivate = true;
	SWORDWIND_ACTIVATE_DESC* pDesc = static_cast<SWORDWIND_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();

	m_bIsGrowing = true;
	m_fGrowDuration = pDesc->fGrowDuration;
	m_fStayDuration = pDesc->fStayDuration;
	m_fDecreaseDuration = pDesc->fDecreaseDuration;
	m_vRotation = pDesc->vStartRotation;
	m_fDisplayTime = m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration;
	m_vRotationAxis = pDesc->vRotationAxis;
	m_fRotationSpeed = pDesc->fRotationSpeed;
	m_fDissolveTime = 0.f;

	m_pParentMatrix = pDesc->pParentMatrix;
	m_pTargetTransform = pDesc->pTargetTransform;

	m_fCreateTime = pDesc->fCreateTime; // Dissolve Threshold 타임.
	m_vStartScale = pDesc->vStartScale;



	// ✅ 로컬 위치 설정 (부모 기준 상대 위치)
	//m_pTransformCom->Set_Position(pDesc->vStartPos);


// 1. 타겟(플레이어)의 현재 상태(위치, 방향)를 가져옵니다.
	CTransform* pCameraTransform = m_pGameInstance->Get_MainCamera()->Get_Transform();

	_vector vTargetPos = pCameraTransform->Get_State(STATE::POSITION);
	_vector vTargetRight = pCameraTransform->Get_State(STATE::RIGHT);
	_vector vTargetUp = pCameraTransform->Get_State(STATE::UP); // '위' 방향 벡터
	_vector vTargetLook = pCameraTransform->Get_State(STATE::LOOK);

	// 1-1. 눈 높이(Y축 오프셋)를 설정합니다. (캐릭터 모델에 맞게 이 값을 조절해야 합니다)
	_float fEyeHeight = 0.f; // 예시: 1.6미터 또는 160유닛

	// 1-2. 플레이어 발밑 위치에 눈 높이만큼의 오프셋을 더해 '눈 위치'를 계산합니다.
	_vector vEyeLevelPos = vTargetPos + (XMVector3Normalize(vTargetUp) * fEyeHeight);

	// 2. 타겟 정면으로 얼마나 멀리 이펙트를 생성할지 거리를 설정합니다.
	_float fDistance = 7.f;

	// 3. 방향 벡터 정규화 (안전장치)
	vTargetLook = XMVector3Normalize(vTargetLook);

	// 4. 최종 위치 계산: 이제 '눈 위치'에서 정면으로 fDistance만큼 떨어진 곳을 계산합니다.
	_vector vFinalPos = vEyeLevelPos + (vTargetLook * fDistance);

	// 5. 이펙트의 방향(회전)을 타겟과 동일하게 설정합니다.
	m_pTransformCom->Set_State(STATE::RIGHT, vTargetRight);
	m_pTransformCom->Set_State(STATE::UP, vTargetUp);
	m_pTransformCom->Set_State(STATE::LOOK, vTargetLook);

	// 6. 이펙트의 위치를 최종 계산된 위치로 설정합니다.
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);


	m_pTransformCom->Set_Scale(pDesc->vStartScale);


	m_pTransformCom->Add_Rotation(
		XMConvertToRadians(pDesc->vStartRotation.x),
		XMConvertToRadians(pDesc->vStartRotation.y),
		XMConvertToRadians(pDesc->vStartRotation.z)
	);

	m_eState = STATE_GROW;
}

//void CSwordWind::OnActivate(void* pArg)
//{
//	m_IsActivate = true;
//	SWORDWIND_ACTIVATE_DESC* pDesc = static_cast<SWORDWIND_ACTIVATE_DESC*>(pArg);
//	m_ActivateDesc = *pDesc;
//	Reset_Timer();
//
//	m_bIsGrowing = true;
//	m_fGrowDuration = pDesc->fGrowDuration;
//	m_fStayDuration = pDesc->fStayDuration;
//	m_fDecreaseDuration = pDesc->fDecreaseDuration;
//	m_vRotation = pDesc->vStartRotation;
//	m_fDisplayTime = m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration;
//	m_vRotationAxis = pDesc->vRotationAxis;
//	m_fRotationSpeed = pDesc->fRotationSpeed;
//	m_fDissolveTime = 0.f;
//	
//	m_pParentMatrix = pDesc->pParentMatrix;
//	m_pTargetTransform = pDesc->pTargetTransform;
//
//	m_fCreateTime = pDesc->fCreateTime; // Dissolve Threshold 타임.
//	m_vStartScale = pDesc->vStartScale;
//
//	
//
//	// ✅ 로컬 위치 설정 (부모 기준 상대 위치)
//	//m_pTransformCom->Set_Position(pDesc->vStartPos);
//
//	
//// 1. 타겟(플레이어)의 현재 상태(위치, 방향)를 가져옵니다.
//	CTransform* pCameraTransform = m_pGameInstance->Get_MainCamera()->Get_Transform();
//
//	_vector vTargetPos = pCameraTransform->Get_State(STATE::POSITION);
//	_vector vTargetRight = pCameraTransform->Get_State(STATE::RIGHT);
//	_vector vTargetUp = pCameraTransform->Get_State(STATE::UP); // '위' 방향 벡터
//	_vector vTargetLook = pCameraTransform->Get_State(STATE::LOOK);
//
//	//_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
//	//_vector vTargetRight = m_pTargetTransform->Get_State(STATE::RIGHT);
//	//_vector vTargetUp = m_pTargetTransform->Get_State(STATE::UP); // '위' 방향 벡터
//	//_vector vTargetLook = m_pTargetTransform->Get_State(STATE::LOOK);
//
//	// ✅ --- START: 눈 높이 보정 ---
//	// 1-1. 눈 높이(Y축 오프셋)를 설정합니다. (캐릭터 모델에 맞게 이 값을 조절해야 합니다)
//	_float fEyeHeight = 1.f; // 예시: 1.6미터 또는 160유닛
//
//	// 1-2. 플레이어 발밑 위치에 눈 높이만큼의 오프셋을 더해 '눈 위치'를 계산합니다.
//	_vector vEyeLevelPos = vTargetPos - (XMVector3Normalize(vTargetUp) * fEyeHeight);
//	// ✅ --- END: 눈 높이 보정 ---
//
//	// 2. 타겟 정면으로 얼마나 멀리 이펙트를 생성할지 거리를 설정합니다.
//	_float fDistance = 7.f;
//
//	// 3. 방향 벡터 정규화 (안전장치)
//	vTargetLook = XMVector3Normalize(vTargetLook);
//
//	// 4. 최종 위치 계산: 이제 '눈 위치'에서 정면으로 fDistance만큼 떨어진 곳을 계산합니다.
//	_vector vFinalPos = vEyeLevelPos + (vTargetLook * fDistance);
//
//	// 5. 이펙트의 방향(회전)을 타겟과 동일하게 설정합니다.
//	m_pTransformCom->Set_State(STATE::RIGHT, vTargetRight);
//	m_pTransformCom->Set_State(STATE::UP, vTargetUp);
//	m_pTransformCom->Set_State(STATE::LOOK, vTargetLook);
//
//	// 6. 이펙트의 위치를 최종 계산된 위치로 설정합니다.
//	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);
//
//
//	m_pTransformCom->Set_Scale(pDesc->vStartScale);
//
//
//	m_eState = STATE_GROW;
//}

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

	if (!m_bIsGrowing)
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


	_float fDisolveRatio = m_fDissolveTime / m_fDecreaseDuration;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &m_fDissolveThreshold, sizeof(_float))))
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

HRESULT CSwordWind::Ready_Components(SWORDWIND_DESC* pDesc)
{
	/* 사용할 쉐이더.*/
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		CRASH("Failed BloodPillarA Components");

	

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

