#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

const _float3 CBounding_Sphere::Get_WorldCenter()
{
	return m_pDesc->Center;
}

HRESULT CBounding_Sphere::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);
	
	m_pLocalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pLocalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}
_bool CBounding_Sphere::Intersect(CCollider::TYPE eColliderType, CBounding* pBounding)
{
	_bool		isColl = { false };
	_float		fPenetrationDepth = 0.0f; // 침투 깊이를 담을 변수

	switch (eColliderType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc()));
		break;

	case CCollider::TYPE_OBB:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc()));
		break;

	case CCollider::TYPE_SPHERE:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc()));
		break;
	}

	return isColl;

}

void CBounding_Sphere::Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return;

	BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);

	m_pDesc->Center = pDesc->vCenter;
	m_pDesc->Radius = pDesc->fRadius;
}

void CBounding_Sphere::Reset_Bounding()
{
	m_pDesc->Center = m_pLocalDesc->Center;
	m_pDesc->Radius = m_pLocalDesc->Radius;
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Create Failed : CBounding_Sphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pLocalDesc);
}
