#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}



HRESULT CBounding_Sphere::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);
	
#ifdef _DEBUG
	m_OriginDesc = *pDesc;
#endif // _DEBUG


	m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

const _float3 CBounding_Sphere::Get_WorldCenter()
{
	return m_pDesc->Center;
}

void CBounding_Sphere::Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return;

	BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);

	m_pOriginalDesc->Center = pDesc->vCenter;
	m_pOriginalDesc->Radius = pDesc->fRadius;

}

void CBounding_Sphere::Reset_Bounding()
{
	m_pDesc->Center = m_pOriginalDesc->Center;
	m_pDesc->Radius = m_pOriginalDesc->Radius;
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);
	return S_OK;
}
#endif // _DEBUG


_bool CBounding_Sphere::Intersect(COLLIDER eColliderType, CBounding* pBounding)
{
	_bool		isColl = { false };
	_float		fPenetrationDepth = 0.0f; // 침투 깊이를 담을 변수

	switch (eColliderType)
	{
	case COLLIDER::AABB:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc()));
		break;

	case COLLIDER::OBB:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc()));
		break;

	case COLLIDER::SPHERE:
		isColl = m_pDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc()));
		break;
	}

	return isColl;

}

_float CBounding_Sphere::Calculate_PenetrationDepthSpehre(CBounding* pBounding)
{
	_float		fPenetrationDepth = 0.0f; // 침투 깊이를 담을 변수

	CBounding_Sphere* pTarget = dynamic_cast<CBounding_Sphere*>(pBounding);
	if (nullptr == pTarget)
	{
		CRASH("Target Shape Not Sphere");
		return 0.f;
	}

	// 1. 중심점 구하기.
	_float3 vLeftCenter = m_pDesc->Center;
	_float3 vRightCenter = pTarget->m_pDesc->Center;

	// 2. 반지름 구하기.
	_float fLeftRadius = m_pDesc->Radius;
	_float fRightRadius = pTarget->m_pDesc->Radius;

	// 3. 중심점 간 거리.
	_vector vDistance = XMLoadFloat3(&vRightCenter) - XMLoadFloat3(&vLeftCenter);
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	// 4. 침투 깊이 구하기
	_float fPenerationDepth = (fLeftRadius + fRightRadius) - fDistance;

	return max(0.0f,fPenerationDepth);
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
	CBounding::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
