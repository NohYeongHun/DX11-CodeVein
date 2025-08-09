#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
	: CGameObject( Prototype )
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartObject::Initialize_Clone(void* pArg)
{
	if (FAILED(CGameObject::Initialize_Clone(pArg)))
		return E_FAIL;

	PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
	CGameObject::Priority_Update(fTimeDelta);
}

void CPartObject::Update(_float fTimeDelta)
{
	CGameObject::Update(fTimeDelta);
}

void CPartObject::Late_Update(_float fTimeDelta)
{
	CGameObject::Late_Update(fTimeDelta);

}

HRESULT CPartObject::Render()
{
	return S_OK;
}


void CPartObject::Update_CombinedMatrix()
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CPartObject::Free()
{
	CGameObject::Free();


}
