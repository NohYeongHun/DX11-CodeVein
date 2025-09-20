#include "Effect_BodyAura.h"
#pragma region  기본 함수들
CEffect_BodyAura::CEffect_BodyAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CEffect_BodyAura::CEffect_BodyAura(const CEffect_BodyAura& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CEffect_BodyAura::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_BodyAura::Initialize_Clone(void* pArg)
{
	EFFECTBODYAURA_DESC* pDesc = static_cast<EFFECTBODYAURA_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);
	pDesc->fSpeedPerSec = 10.f;


#pragma region 기본 변수들 초기화
	m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
	m_pOwner = pDesc->pOwner;
	
#pragma endregion

	if (FAILED(CGameObject::Initialize_Clone(pDesc)))
		CRASH("Failed Clone Sword Wind");

	// 초기 지정 정보. 
	if (FAILED(Ready_Components(pDesc)))
		CRASH("Failed Clone Sword Wind");
	


	return S_OK;
}





void CEffect_BodyAura::Priority_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Priority_Update(fTimeDelta);


}

void CEffect_BodyAura::Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Update(fTimeDelta);
	m_fTime += fTimeDelta; // 시간은 계속 셰이더로 보내야 합니다.

	/* 전체 회전시키기? */
	Shape_Control(fTimeDelta);

	/* 위치를 설정. */
	_vector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pTransformCom->Turn(vAxis, fTimeDelta * 10.f);


	/* 오프셋 위치를 초기에 설정하고 Target에 바인딩합니다. */
	if (m_pTargetTransform)
	{
		m_pTransformCom->Update_WorldMatrix();
		XMStoreFloat4x4(&m_CombinedWorldMatrix
			, m_pTransformCom->Get_WorldMatrix() * m_pTargetTransform->Get_WorldMatrix());
	}


}


void CEffect_BodyAura::Late_Update(_float fTimeDelta)
{
	if (!m_IsActivate)
		return;

	CPartObject::Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;

	/* Distortion은 Renderer의 Combine이후에 모든 과정이 끝나고 나서 출력.*/

}

HRESULT CEffect_BodyAura::Render()
{
	
#ifdef _DEBUG
	ImGui_Render();
#endif // _DEBUG

	if (FAILED(Bind_ShaderResources()))
	{
		CRASH("Ready Render Resource Failed");
		return E_FAIL;
	}

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
	/*	if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;*/

		m_pShaderCom->Begin(m_iShaderPath);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

#pragma endregion


#pragma region  풀링시 지정할 정보들
void CEffect_BodyAura::OnActivate(void* pArg)
{
	m_IsActivate = true;
	EFFECTFBODYAURA_ACTIVATE_DESC* pDesc = static_cast<EFFECTFBODYAURA_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();


	m_bIsGrowing = true;
	m_fStayDuration = pDesc->fStayDuration;
	m_fDissolveDuration = pDesc->fDissolveDuration;

	m_fDisplayTime = m_fStayDuration + m_fDissolveDuration;
	m_fDissolveTime = 0.f;
	m_fDissolveThreshold = 0.f; // 초기화

	m_pTargetTransform = pDesc->pTargetTransform;

	m_vStartScale = pDesc->vStartScale;



	m_pTransformCom->Set_Scale(m_vStartScale);

	m_pTransformCom->Add_Rotation(
		XMConvertToRadians(pDesc->vStartRotation.x),
		XMConvertToRadians(pDesc->vStartRotation.y),
		XMConvertToRadians(pDesc->vStartRotation.z));

	m_eState = STATE_STAY;
}


void CEffect_BodyAura::OnDeActivate()
{
	
}

#pragma endregion

#pragma region 모양 형태 변경.
void CEffect_BodyAura::Shape_Control(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	switch(m_eState)
	{
	case STATE_STAY:
		Update_Stay(fTimeDelta);
		break;
	case STATE_DISSOLVE:
		Update_Dissolve(fTimeDelta);
		break;
	}
}

void CEffect_BodyAura::Update_Grow(_float fTimeDelta)
{
	if (m_fCurrentTime >= m_fGrowDuration)
	{
		m_eState = STATE_STAY; // '중간' 상태로 변경
		m_fCurrentTime = 0.f;     // 타이머 리셋!
	}
}

void CEffect_BodyAura::Update_Stay(_float fTimeDelta)
{
	if (m_fCurrentTime >= m_fStayDuration)
	{
		m_eState = STATE_DISSOLVE; // '감소' 상태로 변경
		m_fCurrentTime = 0.f;     // 타이머 리셋!
	}

	m_fStayTime += fTimeDelta;
}
void CEffect_BodyAura::Update_Dissolve(_float fTimeDelta)
{
	// 감소 완료 시, 종료 상태로 전환
	if (m_fCurrentTime >= m_fDissolveDuration)
	{
		m_eState = STATE_END; // '종료' 상태로 변경
		// Set_Dead() 같은 함수로 객체를 비활성화하거나 풀에 반납
		m_IsActivate = false;
		return;
	}

	m_fDissolveTime += fTimeDelta; // Dissolve
}
#pragma endregion



void CEffect_BodyAura::Reset_Timer()
{
	m_fCurrentTime = 0.0f;
	m_fDisplayTime = 0.0f;
	m_fDissolveTime = 0.f;
	m_fTime = 0.f;
}

void CEffect_BodyAura::Calc_Timer(_float fTimeDelta)
{
	
}



HRESULT CEffect_BodyAura::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollSpeed", &m_fScrollSpeed, sizeof(_float))))
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

	_float fStayRatio = m_fStayTime / m_fStayDuration;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fGrowTime", &fStayRatio, sizeof(_float))))
	{
		CRASH("Failed Bind g_fGrowTime");
		return E_FAIL;
	}


	_float fDissolveRatio = m_fDissolveTime / m_fDissolveDuration;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveRatio, sizeof(_float))))
	{
		CRASH("Failed Bind Cam Position");
		return E_FAIL;
	}

#pragma endregion

	

	
	
#pragma endregion

#pragma region TEXTURE 바인딩.
	// 피색 Diffuse
	if (FAILED(m_pTextureCom[TEXTURE::TEXTURE_DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
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
	
#pragma endregion


	return S_OK;
}


HRESULT CEffect_BodyAura::Ready_Components(EFFECTBODYAURA_DESC* pDesc)
{
	/* 사용할 쉐이더.*/
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		CRASH("Failed SwordWind Components");


	/* 사용할 모델.*/
	CLoad_Model::LOADMODEL_DESC Desc{};
	Desc.pGameObject = this;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Effect_BloodBodyAura"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
		CRASH("Failed BloodPillarA Components");

	/* 사용할 텍스쳐들 ?*/

	// 1. Diffuse 바인딩 => 늘수도있음. 아마 이건 0번 쓰겠지? 인덱스로
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BloodAuraDiffuse")
		, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_DIFFUSE]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	// 2. Other 바인딩
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BloodAuraOther")
		, TEXT("Com_Other"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_OTHER]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	// 3. Noise 바인딩
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BloodAuraNoise")
		, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_NOISE]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}


	return S_OK;
}



CEffect_BodyAura* CEffect_BodyAura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_BodyAura* pInstance = new CEffect_BodyAura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_BodyAura"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_BodyAura::Clone(void* pArg)
{
	CEffect_BodyAura* pInstance = new CEffect_BodyAura(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_BodyAura"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_BodyAura::Free()
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
void CEffect_BodyAura::ImGui_Render()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowPos = ImVec2(0.f, 0.f);
	ImVec2 windowSize = ImVec2(300.f, 300.f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	ImGui::Begin("Effect PlayerAura Debug", nullptr, ImGuiWindowFlags_NoCollapse);

	_float4 vPos = {};
	//XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
	
	memcpy(&vPos, &m_CombinedWorldMatrix.m[3], sizeof(_float4));
	//XMStoreFloat3(&vPos, XMLoadFloat4(m_CombinedWorldMatrix.m[3]));


	ImGui::Text("Effect PlayerAura Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

	ImGui::End();
}
#endif // _DEBUG

