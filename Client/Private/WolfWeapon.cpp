#include "WolfWeapon.h"

CWolfWeapon::CWolfWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CWolfWeapon::CWolfWeapon(const CWolfWeapon& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CWolfWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWolfWeapon::Initialize(void* pArg)
{
    WOLF_WEAPON_DESC* pDesc = static_cast<WOLF_WEAPON_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(CWeapon::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    // 기본 false;
    m_pColliderCom->Set_Active(false);

    return S_OK;
}

void CWolfWeapon::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);
}

void CWolfWeapon::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);

    if (nullptr != m_pSocketMatrix && nullptr != m_pParentMatrix)
    {
        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() *
            XMLoadFloat4x4(m_pSocketMatrix) *
            XMLoadFloat4x4(m_pParentMatrix));
    }
    else
    {
        OutputDebugStringA("WolfWeapon: Socket or Parent matrix is nullptr!\n");
    }

    CWeapon::Finalize_Update(fTimeDelta);
}

void CWolfWeapon::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CWolfWeapon::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif // _DEBUG

    return S_OK;
}

void CWolfWeapon::On_Collision_Enter(CGameObject* pOther)
{
}

void CWolfWeapon::On_Collision_Stay(CGameObject* pOther)
{
}

void CWolfWeapon::On_Collision_Exit(CGameObject* pOther)
{
}

HRESULT CWolfWeapon::Ready_Components()
{
    return S_OK;
}

HRESULT CWolfWeapon::Ready_Colliders()
{
    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f); // 중점.


    OBBDesc.vRotation = _float3(
        XMConvertToRadians(-95.f),  // Pitch: 아래쪽으로 기울어짐
        XMConvertToRadians(45.f),   // Yaw: 오른쪽으로 회전
        XMConvertToRadians(0.f)     // Roll: 회전 없음
    );

    OBBDesc.pOwner = this;
    OBBDesc.eCollisionType = CCollider::COLLISION_TRIGGER;
    OBBDesc.eMyLayer = CCollider::MONSTER_WEAPON;
    OBBDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON |
        CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_OBB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
    {
        CRASH("Failed Clone Collider OBB");
        return E_FAIL;
    }

    if (nullptr == m_pColliderCom)
    {
        OutputDebugStringA("WolfWeapon: m_pColliderCom is nullptr after creation!\n");
        return E_FAIL;
    }

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));
    OutputDebugStringA("WolfWeapon: Collider created and registered successfully\n");

    return S_OK;
}

HRESULT CWolfWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CWolfWeapon* CWolfWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWolfWeapon* pInstance = new CWolfWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWolfWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWolfWeapon::Clone(void* pArg)
{
    CWolfWeapon* pInstance = new CWolfWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWolfWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWolfWeapon::Free()
{
    CWeapon::Free();
}
