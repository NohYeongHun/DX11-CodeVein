#pragma region  기본 함수들
CBlood_PillarC::CBlood_PillarC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBlood_PillarC::CBlood_PillarC(const CBlood_PillarC& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CBlood_PillarC::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood_PillarC::Initialize_Clone(void* pArg)
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

void CBlood_PillarC::Priority_Update(_float fTimeDelta)
{
	CPartObject::Priority_Update(fTimeDelta);
}

void CBlood_PillarC::Update(_float fTimeDelta)
{
	CPartObject::Update(fTimeDelta);

	/* 따로 소켓은 없으니까 소켓은 곱하지 않습니다. => m_CombinedWorld Matrix가 최종 객체. */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		m_pTransformCom->Get_WorldMatrix() *
		XMLoadFloat4x4(m_pParentMatrix));
}

void CBlood_PillarC::Late_Update(_float fTimeDelta)
{
	CPartObject::Late_Update(fTimeDelta);

	//if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
	//	return;
}

HRESULT CBlood_PillarC::Render()
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
void CBlood_PillarC::OnActivate(void* pArg)
{
	PILLARC_ACTIVATE_DESC* pDesc = static_cast<PILLARC_ACTIVATE_DESC*>(pArg);
	m_ActivateDesc = *pDesc;
	Reset_Timer();
	
}

void CBlood_PillarC::OnDeActivate()
{

}

#pragma endregion


void CBlood_PillarC::Calc_Timer(_float fTimeDelta)
{
	if (m_fCurrentTime < m_fDisplayTime)
		m_fCurrentTime += fTimeDelta;

	// 처리할 게 있다면? 시간다돼면
}


HRESULT CBlood_PillarC::Bind_ShaderResources()
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

HRESULT CBlood_PillarC::Ready_Components(BLOODPILLAR_DESC* pDesc)
{
	/* 사용할 쉐이더.*/
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		CRASH("Failed BloodPillarA Components");

	

	/* 사용할 모델.*/
	CLoad_Model::LOADMODEL_DESC Desc{};
	Desc.pGameObject = this;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Effect_BloodPillarC"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
		CRASH("Failed BloodPillarC Components");

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



CBlood_PillarC* CBlood_PillarC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood_PillarC* pInstance = new CBlood_PillarC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBlood_PillarC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlood_PillarC::Clone(void* pArg)
{
	CBlood_PillarC* pInstance = new CBlood_PillarC(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CBlood_PillarC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood_PillarC::Free()
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
void CBlood_PillarC::ImGui_Render()
{

}
#endif // _DEBUG

