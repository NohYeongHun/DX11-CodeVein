#include "GameObject.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_strObjTag { Prototype.m_strObjTag }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent* CGameObject::Get_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize_Clone(void* pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize_Clone(pArg)))
		return E_FAIL;	

	m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);		

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
	m_pTransformCom->Update_PrevPosition();
	m_pTransformCom->Update_WorldMatrix();
}



void CGameObject::Late_Update(_float fTimeDelta)
{
	/* Trasnform이 없는 객체는 존재하지 않기 때문에 GameObject에서 실행해도 무방. */
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::On_Collision_Enter(CGameObject* pOther)
{

}

void CGameObject::On_Collision_Stay(CGameObject* pOther)
{

}

void CGameObject::On_Collision_Exit(CGameObject* pOther)
{

}

const _bool CGameObject::Is_Ray_LocalHit(_float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist)
{
	return false;
}

const _bool CGameObject::Is_Ray_LocalHit(MODEL_PICKING_INFO* pPickingInfo, _float* pOutDist)
{
	return false;
}




#ifdef _DEBUG
HRESULT CGameObject::Initialize_Debug()
{
	// BasicEffect 생성
	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	// Input Layout 생성
	void const* pShaderByteCode = nullptr;
	size_t ByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		pShaderByteCode, ByteCodeLength,
		&m_pInputLayout)))
		return E_FAIL;

	// PrimitiveBatch 생성
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	return S_OK;
}
//HRESULT CGameObject::BoundingBoxRender(const BOUNDING_BOX& box, _matrix WorldMatrix)
//{
//	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout)
//		return E_FAIL;
//
//	// 파이프라인에서 World, View, Projection 행렬 가져오기
//
//	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
//	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
//
//	m_pEffect->SetWorld(WorldMatrix);
//	m_pEffect->SetView(ViewMatrix);
//	m_pEffect->SetProjection(ProjMatrix);
//
//	m_pContext->IASetInputLayout(m_pInputLayout);
//
//	m_pEffect->Apply(m_pContext);
//
//	m_pBatch->Begin();
//
//	Render_AABB(box);
//	// 각 Bounding 타입별로 렌더링
//	/*switch (m_eType)
//	{
//	case TYPE_AABB:
//		Render_AABB();
//		break;
//	case TYPE_OBB:
//		Render_OBB();
//		break;
//	case TYPE_SPHERE:
//		Render_Sphere();
//		break;
//	}*/
//
//	m_pBatch->End();
//
//
//	return S_OK;
//}
//void CGameObject::Render_AABB(const BOUNDING_BOX& box)
//{
//	vector<VertexPositionColor> vertices;
//	vertices.reserve(8);
//	
//	_float3 vCenter = box.vCenter;
//	_float3 vExtents = box.vExtents;
//
//	// 앞면 4개 점
//	vertices.emplace_back(_float3(vCenter.x - vExtents.x, vCenter.y + vExtents.y, vCenter.z - vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x + vExtents.x, vCenter.y + vExtents.y, vCenter.z - vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x + vExtents.x, vCenter.y - vExtents.y, vCenter.z - vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x - vExtents.x, vCenter.y - vExtents.y, vCenter.z - vExtents.z), m_vColor);
//
//	// 뒷면 4개 점
//	vertices.emplace_back(_float3(vCenter.x - vExtents.x, vCenter.y + vExtents.y, vCenter.z + vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x + vExtents.x, vCenter.y + vExtents.y, vCenter.z + vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x + vExtents.x, vCenter.y - vExtents.y, vCenter.z + vExtents.z), m_vColor);
//	vertices.emplace_back(_float3(vCenter.x - vExtents.x, vCenter.y - vExtents.y, vCenter.z + vExtents.z), m_vColor);
//	
//	// 박스의 12개 모서리 그리기
//   // 앞면
//	m_pBatch->DrawLine(vertices[0], vertices[1]);
//	m_pBatch->DrawLine(vertices[1], vertices[2]);
//	m_pBatch->DrawLine(vertices[2], vertices[3]);
//	m_pBatch->DrawLine(vertices[3], vertices[0]);
//
//	// 뒷면
//	m_pBatch->DrawLine(vertices[4], vertices[5]);
//	m_pBatch->DrawLine(vertices[5], vertices[6]);
//	m_pBatch->DrawLine(vertices[6], vertices[7]);
//	m_pBatch->DrawLine(vertices[7], vertices[4]);
//
//	// 앞뒤 연결
//	m_pBatch->DrawLine(vertices[0], vertices[4]);
//	m_pBatch->DrawLine(vertices[1], vertices[5]);
//	m_pBatch->DrawLine(vertices[2], vertices[6]);
//	m_pBatch->DrawLine(vertices[3], vertices[7]);
//
//}
#endif

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Get_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

// 여기서 바인딩한 Component에 AddRef 설정.
HRESULT CGameObject::Change_Component(const _wstring& strComponentTag, CComponent** ppOut, CComponent* pChangeComponent)
{
	CComponent* pComponent = Get_Component(strComponentTag);

	if (nullptr != pComponent)
	{
		// 기존 컴포넌트를 지우기?
		Safe_Release(pComponent);
		m_Components.erase(strComponentTag);
	}

	m_Components.emplace(strComponentTag, pChangeComponent);

	*ppOut = pChangeComponent;
	Safe_AddRef(pChangeComponent);
	

	return S_OK;
}

/* 이동량 만큼을 더해줍니다. => 루트모션 전용  */
void CGameObject::RootMotion_Translate(_fvector vTranslate)
{
	//if (nullptr == m_pTransformCom)
	//{
	//	CRASH("Transform Component Not Exist");
	//	return;
	//}
	//
	//m_pTransformCom->Translate(vTranslate);
	
	//_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	//vPos += vTranslate;
	//vPos = XMVectorSetW(vPos, 1.f);
	//m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CGameObject::Compute_CamDistance(_fvector vWorldPos)
{
	m_fCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - vWorldPos));
}

const _wstring& CGameObject::Get_ObjectTag()
{
	return m_strObjTag;
}



void CGameObject::Destroy()
{
}

void CGameObject::Free()
{
	CBase::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
#endif // _DEBUG

}
