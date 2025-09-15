#include "Blood_PillarA.h"
#pragma region  기본 함수들
CBlood_PillarA::CBlood_PillarA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBlood_PillarA::CBlood_PillarA(const CBlood_PillarA& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CBlood_PillarA::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood_PillarA::Initialize_Clone(void* pArg)
{
	BLOODPILLAR_DESC* pDesc = static_cast<BLOODPILLAR_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);
	pDesc->fSpeedPerSec = 10.f;

#pragma region 기본 변수들 초기화
	m_fDisplayTime = pDesc->fDisplayTime;
	m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
	m_pParentMatrix = pDesc->pParentMatrix;
	m_pOwner = pDesc->pOwner;
	
#pragma endregion

	if (FAILED(CGameObject::Initialize_Clone(pDesc)))
		CRASH("Failed Clone Blood PillarA");

	// 초기 지정 정보. 
	if (FAILED(Ready_Components(pDesc)))
		CRASH("Failed Clone Blood PillarA");

	


	return S_OK;
}

void CBlood_PillarA::Priority_Update(_float fTimeDelta)
{
	CPartObject::Priority_Update(fTimeDelta);


}

void CBlood_PillarA::Update(_float fTimeDelta)
{
	CPartObject::Update(fTimeDelta);

	
	/* 전체 회전시키기? */
	Shape_Control(fTimeDelta);

	/* 따로 소켓은 없으니까 소켓은 곱하지 않습니다. => m_CombinedWorld Matrix가 최종 객체. */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		m_pTransformCom->Get_WorldMatrix() *
		XMLoadFloat4x4(m_pParentMatrix));

	
}

void CBlood_PillarA::Late_Update(_float fTimeDelta)
{
	CPartObject::Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
		return;
}

HRESULT CBlood_PillarA::Render()
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
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NoiseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPath);
		m_pModelCom->Render(i);
	}

	return S_OK;
}
#pragma endregion


#pragma region  풀링시 지정할 정보들
void CBlood_PillarA::OnActivate(void* pArg)
{
	PILLARA_ACTIVATE_DESC* pDesc = static_cast<PILLARA_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();
	
	m_bIsGrowing = true;
	m_fGrowDuration = pDesc->fGrowDuration;
	m_fStayDuration = pDesc->fStayDuration;
	m_fDecreaseDuration = pDesc->fDecreaseDuration;
	m_fTargetRadius = pDesc->fTargetRadius;
	m_fDecreaseTargetRadius = pDesc->fDecreaseTargetRadius;
	m_fTargetHeight = pDesc->fTargetHeight;
	m_fDisplayTime = m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration;


	/* 시작 시 */
	_float3 vScale = { 0.01f, 0.f, 0.01f };

	m_eState = STATE_GROW;
	m_pTransformCom->Set_Scale(vScale);
}

void CBlood_PillarA::OnDeActivate()
{

}

#pragma endregion


void CBlood_PillarA::Calc_Timer(_float fTimeDelta)
{
	
}

void CBlood_PillarA::Shape_Control(_float fTimeDelta)
{
	RotateTurn_ToYaw(fTimeDelta);

	m_fCurrentTime += fTimeDelta;
	switch (m_eState)
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
	}

	if (!m_bIsGrowing)
		return;

	
}

void CBlood_PillarA::Update_Grow(_float fTimeDelta)
{
	// 성장 완료 시, 다음 상태로 전환
	if (m_fCurrentTime >= m_fGrowDuration)
	{
		m_eState = STATE_STAY; // '유지' 상태로 변경
		m_fCurrentTime = 0.f;  // 타이머 리셋!
		m_pTransformCom->Set_Scale({ m_fTargetRadius, m_fTargetHeight, m_fTargetRadius }); // 최종 크기로 고정
		return;
	}

	_float fRatio = m_fCurrentTime / m_fGrowDuration;

	_float fCurrentRadius = 0.01f + (m_fTargetRadius - 0.01f) * fRatio;
	_float fCurrentHeight = 0.f + (m_fTargetHeight - 0.f) * fRatio;

	m_pTransformCom->Set_Scale({ fCurrentRadius, fCurrentHeight, fCurrentRadius });
}

void CBlood_PillarA::Update_Stay(_float fTimeDelta)
{
	// 유지 시간 완료 시, 다음 상태로 전환
	if (m_fCurrentTime >= m_fStayDuration)
	{
		m_eState = STATE_DECREASE; // '감소' 상태로 변경
		m_fCurrentTime = 0.f;     // 타이머 리셋!
	}
	// 이 상태에서는 아무것도 하지 않고 그냥 크기를 유지합니다.
}

void CBlood_PillarA::Update_Decrease(_float fTimeDelta)
{
	// 감소 완료 시, 종료 상태로 전환
	if (m_fCurrentTime >= m_fDecreaseDuration)
	{
		m_eState = STATE_END; // '종료' 상태로 변경
		// Set_Dead() 같은 함수로 객체를 비활성화하거나 풀에 반납
		return;
	}

	_float fRatio = m_fCurrentTime / m_fDecreaseDuration;

	// [수정] 선형 보간 공식을 사용하여 반지름을 계산합니다.
	// 시작값: m_fTargetRadius
	// 목표값: m_fDecreaseTargetRadius
	_float fCurrentRadius = m_fTargetRadius + (m_fDecreaseTargetRadius - m_fTargetRadius) * fRatio;

	// Y축 스케일(높이)은 이전 요청대로 m_fTargetHeight 값으로 고정합니다.
	m_pTransformCom->Set_Scale({ fCurrentRadius, m_fTargetHeight, fCurrentRadius });
}




/* Combined 곱하기 전에 실행. => Update*/
void CBlood_PillarA::RotateTurn_ToYaw(_float fTimeDelta)
{
	m_pTransformCom->Add_Rotation( 0.f, fTimeDelta * XMConvertToRadians(90.f), 0.f );
}


HRESULT CBlood_PillarA::Bind_ShaderResources()
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
	
#pragma endregion

#pragma region TEXTURE 바인딩.
	if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTexture")))
	{
		CRASH("Failed Bind Texture Diffuse Texture ");
		return E_FAIL;
	}
#pragma endregion


	return S_OK;
}

HRESULT CBlood_PillarA::Ready_Components(BLOODPILLAR_DESC* pDesc)
{
	/* 사용할 쉐이더.*/
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		CRASH("Failed BloodPillarA Components");

	

	/* 사용할 모델.*/
	CLoad_Model::LOADMODEL_DESC Desc{};
	Desc.pGameObject = this;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Effect_BloodPillarA"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
		CRASH("Failed BloodPillarA Components");

	/* 사용할 텍스쳐들 ?*/

	// 1. Diffuse 바인딩 => 늘수도있음. 아마 이건 0번 쓰겠지? 인덱스로
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BloodPillarDiffuse")
		, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE::TEXTURE_DIFFUSE]))))
	{
		CRASH("Failed LoadTexture");
		return E_FAIL;
	}

	// 사용할 인덱스 지정.?
	for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
		m_iTextureIndexArray[i] = pDesc->iTextureIndexArray[i];

	return S_OK;
}



CBlood_PillarA* CBlood_PillarA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood_PillarA* pInstance = new CBlood_PillarA(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBlood_PillarA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlood_PillarA::Clone(void* pArg)
{
	CBlood_PillarA* pInstance = new CBlood_PillarA(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBlood_PillarA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood_PillarA::Free()
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
void CBlood_PillarA::ImGui_Render()
{

}
#endif // _DEBUG

