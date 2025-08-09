﻿#include "Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}


const _float3 CBounding_AABB::Get_WorldCenter()
{
	return m_pDesc->Center;
}

HRESULT CBounding_AABB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);
	
#ifdef _DEBUG
	m_DebugDesc = *pDesc;
#endif // _DEBUG

	
	m_pOriginalDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);

}

void CBounding_AABB::Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return;

	BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pOriginalDesc->Center = pDesc->vCenter;
	m_pOriginalDesc->Extents = pDesc->vExtents;
}

void CBounding_AABB::Reset_Bounding()
{
	m_pDesc->Center = m_pOriginalDesc->Center;
	m_pDesc->Extents = m_pOriginalDesc->Extents;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);
	return S_OK;
}
#endif 

_bool CBounding_AABB::Intersect(COLLIDER eColliderType, CBounding* pBounding)
{
	_bool		isColl = { false };

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




CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Create Failed : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);

}
