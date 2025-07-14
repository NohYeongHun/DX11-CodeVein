#include "Collider.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
    : CComponent(Prototype)
{
}

CGameObject* CCollider::Get_Owner()
{
    return m_pOwner;
}

const _bool CCollider::Is_Active()
{
    return m_IsActive;
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

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
    m_eType = eType;

   

    return S_OK;
}

HRESULT CCollider::Initialize_Clone(void* pArg)
{
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

    switch (m_eType)
    {
    case TYPE::TYPE_AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case TYPE::TYPE_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case TYPE::TYPE_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    }

    m_IsActive = true;
    return S_OK;
}

void CCollider::Update(const _float4x4* pMatrix)
{
    m_pBounding->Update(XMLoadFloat4x4(pMatrix));
}

_bool CCollider::Intersect(const CCollider* pTargetCollider)
{
    return _bool();
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
    return S_OK;
}
#endif // _DEBUG




CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
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

}
