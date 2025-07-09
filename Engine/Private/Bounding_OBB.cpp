#include "Bounding_OBB.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

const _float3 CBounding_OBB::Get_WorldCenter()
{
	return _float3();
}

// OBB 이므로 이미 회전한 상태로 받아와야합니다. (Collider를)
HRESULT CBounding_OBB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pLocalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pLocalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	// Local 기준으로 World 행렬 변환을 m_pDesc에 적용.
	m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}
_bool CBounding_OBB::Intersect(CCollider::TYPE eColliderType, CBounding* pBounding)
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

void CBounding_OBB::Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc)
{
}

void CBounding_OBB::Reset_Bounding()
{
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Create Failed : CBounding_OBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pLocalDesc);
}
