CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
	: CGameObject(Prototype)
{
}


HRESULT CEffect::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
	{
		CRASH("Failed Initialize Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect::Initialize_Clone(void* pArg)
{
	/* 1. Owner의 Transform을 Effect 객체마다 매핑합니다. */
	if (FAILED(CGameObject::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
	CGameObject::Priority_Update(fTimeDelta);
}

void CEffect::Update(_float fTimeDelta)
{
	CGameObject::Update(fTimeDelta);
}

void CEffect::Late_Update(_float fTimeDelta)
{
	CGameObject::Late_Update(fTimeDelta);


}

HRESULT CEffect::Render()
{
	return S_OK;
}



#pragma region POOLING 객체마다 다르게 정의해야하는 것들
void CEffect::OnActivate(void* pArg)
{

}

void CEffect::OnDeActivate()
{
	m_pOwnerTransform = nullptr;
}
void CEffect::OnMoved_ToObjectManager()
{

}
#pragma endregion


void CEffect::Destroy()
{
	CGameObject::Destroy();
}

void CEffect::Free()
{
	CGameObject::Free();
}
