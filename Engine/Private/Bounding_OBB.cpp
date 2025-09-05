CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}



// OBB 이므로 이미 회전한 상태로 받아와야합니다. (Collider를)
HRESULT CBounding_OBB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pBoundingDesc);

	m_OriginDesc = *pDesc;

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	// Local 기준으로 World 행렬 변환을 m_pDesc에 적용.
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}


const _float3 CBounding_OBB::Get_WorldCenter()
{
	return m_pDesc->Center;
}

/* 디버그 용도로 필수. */
void CBounding_OBB::Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return;

	BOUNDING_OBB_DESC* pDesc = static_cast<BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4			vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginalDesc->Center = pDesc->vCenter;
	m_pOriginalDesc->Orientation = vQuaternion;
	m_pOriginalDesc->Extents = pDesc->vExtents;
}

void CBounding_OBB::Reset_Bounding()
{
	m_pDesc->Center = m_pOriginalDesc->Center;
	m_pDesc->Orientation = m_pOriginalDesc->Orientation;
	m_pDesc->Extents = m_pOriginalDesc->Extents;
}



#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}
#endif // _DEBUG


_bool CBounding_OBB::Intersect(COLLIDER eColliderType, CBounding* pBounding)
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
	CBounding::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
