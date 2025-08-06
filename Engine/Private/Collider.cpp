#include "Collider.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
    : CComponent(Prototype)
    , m_eType{ Prototype.m_eType }
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

CGameObject* CCollider::Get_Owner()
{
    return m_pOwner;
}

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

void CCollider::Change_BoundingDesc(void* pBoundingDesc)
{
    m_pBounding->Change_BoundingDesc(static_cast<CBounding::BOUNDING_DESC*>(pBoundingDesc));
}

void CCollider::Reset_Bounding()
{
    m_pBounding->Reset_Bounding();
}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
    m_eType = eType;

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

    switch (m_eType)
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

    m_IsActive = true;
    return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
    m_pBounding->Update(WorldMatrix);

    //if (m_pOwner->Get_ObjectTag() == TEXT("PlayerWeapon"))
    //{
    //    if (m_IsActive)
    //        OutputDebugWstring(TEXT("현재 bool 값은 true 입니다."));
    //}
}

_bool CCollider::Intersect(const CCollider* pTargetCollider)
{
    return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

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
    __super::Free();
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
}
