#include "Collider.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
    : CComponent(Prototype)
    , m_eColliderShape{ Prototype.m_eColliderShape }
#ifdef _DEBUG
    , m_pBatch{ Prototype.m_pBatch }
    , m_pEffect{ Prototype.m_pEffect }
    , m_pInputLayout { Prototype.m_pInputLayout }
#endif
{

#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

#pragma region 0. 초기화 함수.
HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
    m_eColliderShape = eType;

    /* Render용 추가 .*/
#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t		iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
        pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif
    return S_OK;
}

HRESULT CCollider::Initialize_Clone(void* pArg)
{
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);
    m_pOwner = pBoundingDesc->pOwner;
    m_eCollisionType = pBoundingDesc->eCollisionType;
    m_eMyLayer = pBoundingDesc->eMyLayer;
    m_TargetLayers = pBoundingDesc->eTargetLayer;

    // BODY 타입은 무조건 구로만 생성 (침투 깊이 연산 통일)
    if (m_eCollisionType == CCollider::COLLISION_BODY)
    {
        if (m_eColliderShape != COLLIDER::SPHERE)
            CRASH("Body 타입은 무조건 Sphere 로 생성하시오.");
    }

    switch (m_eColliderShape)
    {
    case COLLIDER::AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case COLLIDER::OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case COLLIDER::SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    }

    CBounding_AABB::BOUNDING_AABB_DESC Desc{};
    Desc.vCenter = pBoundingDesc->vCenter;
    Desc.vExtents = { 10.f, 10.f, 10.f };
    m_pWorldBounding = CBounding_AABB::Create(m_pDevice, m_pContext, &Desc);


    m_IsActive = true;
    return S_OK;
}



#pragma endregion


#pragma region  0. 충돌체가 가져야 하는 식별 정보와 관련 함수들.
// 1. 주인 체크
CGameObject* CCollider::Get_Owner()
{
    return m_pOwner;
}

//2. 활성화 상태 체크
const _bool CCollider::Is_Active()
{
    return m_IsActive;
}

/* 활성화 상태 변경. */
void CCollider::Set_Active(_bool IsActive)
{
    m_IsActive = IsActive;
    m_vColor = !m_IsActive ? DirectX::Colors::Aqua : DirectX::Colors::Black;
}

// 3. Layer 체크 => 해당 안되면 바로 무시.
_bool CCollider::Has_TargetLayer(CCollider* pRight)
{
    // 비트 연산해서 비트가 하나라도 0이 아니라면. True 반환.
    return (m_TargetLayers & pRight->m_eMyLayer) != 0;
}

_float3 CCollider::Get_Center()
{
   return m_pBounding->Get_WorldCenter();
}




#pragma endregion



#pragma region 3. Collider Manager에서 호출하는 충돌 관리용 함수.

_bool CCollider::Has_TargetLayerCheck(CCollider* pRight)
{
    return Has_TargetLayer(pRight);
}

const _bool CCollider::Find_ColliderObject(CGameObject* pColliderObject)
{
    auto iter = m_ColliderObjects.find(pColliderObject);

    return iter != m_ColliderObjects.end();
}

void CCollider::Erase_ColliderObject(CGameObject* pColliderObject)
{
    m_ColliderObjects.erase(pColliderObject);
}

void CCollider::Insert_ColliderObject(CGameObject* pColliderObject)
{
    m_ColliderObjects.insert(pColliderObject);
}

#ifdef _DEBUG

void* CCollider::Get_BoundingDesc()
{
    switch (m_eColliderShape)
    {
        case COLLIDER::AABB:
        {
            CBounding_AABB* pAABB = static_cast<CBounding_AABB*>(m_pBounding);
            return static_cast<void*>(pAABB->Get_DebugDesc());
        }

        case COLLIDER::OBB:
        {
            CBounding_OBB* pOBB = static_cast<CBounding_OBB*>(m_pBounding);
            return static_cast<void*>(pOBB->Get_DebugDesc());
        }

        case COLLIDER::SPHERE:
        {
            CBounding_Sphere* pOBB = static_cast<CBounding_Sphere*>(m_pBounding);
            return static_cast<void*>(pOBB->Get_DebugDesc());
        }
    }
    return nullptr;
}



void CCollider::Change_BoundingDesc(void* pBoundingDesc)
{
    m_pBounding->Change_BoundingDesc(static_cast<CBounding::BOUNDING_DESC*>(pBoundingDesc));
}

void CCollider::Reset_Bounding()
{
    m_pBounding->Reset_Bounding();
}
#endif // _DEBUG



#pragma endregion



#pragma region 5. 매프레임 업데이트 되는 함수
void CCollider::Update(_fmatrix WorldMatrix)
{
    m_pBounding->Update(WorldMatrix);
    m_pWorldBounding->Update(WorldMatrix);
}

/* 2차 충돌 감지 */
_bool CCollider::Intersect(const CCollider* pTargetCollider)
{
    return m_pBounding->Intersect(pTargetCollider->m_eColliderShape, pTargetCollider->m_pBounding);
}

/* 1차 충돌 감지 */
_bool CCollider::BroadIntersect(const CCollider* pTargetCollider)
{
    return m_pWorldBounding->Intersect(COLLIDER::AABB, pTargetCollider->m_pWorldBounding);
}

_float CCollider::Calculate_PenetrationDepthSpehre(const CCollider* pTargetCollider)
{
    return m_pBounding->Calculate_PenetrationDepthSpehre(pTargetCollider->m_pBounding);
}


#pragma endregion


#ifdef _DEBUG
HRESULT CCollider::Render()
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pEffect->Apply(m_pContext);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch, m_vColor);

    //m_pWorldBounding->Render(m_pBatch, m_vColor);

    m_pBatch->End();

    return S_OK;
}
#endif // _DEBUG




CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eColliderType)
{
    CCollider* pInstance = new CCollider(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
    {
        MSG_BOX(TEXT("Create Failed : Collider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
    CCollider* pInstance = new CCollider(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : Collider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    CComponent::Free();
    m_ColliderObjects.clear();
    if (false == m_isCloned)
    {
        Safe_Delete(m_pEffect);
        Safe_Delete(m_pBatch);
    }
    
#ifdef _DEBUG
    Safe_Release(m_pInputLayout);
#endif // _DEBUG


    Safe_Release(m_pBounding);
    Safe_Release(m_pWorldBounding);
}
