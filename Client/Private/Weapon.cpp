#include "Weapon.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
    : CPartObject(Prototype)
{

}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_eCurLevel = pDesc->eCurLevel;
    m_pOwner = pDesc->pOwner;
    m_fAttackPower = pDesc->fAttackPower;

    if (FAILED(CPartObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone Weapon");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Clone Component");
        return E_FAIL;
    }
        

    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
}


void CWeapon::Finalize_Update(_float fTimeDelta)
{
    CGameObject::Finalize_Update(fTimeDelta);

    /* Collider의 Set Active는 Finalize_Update 이전에 호출됩니다. */
    if (m_pColliderCom)
    {
        _matrix WorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
        m_pColliderCom->Update(WorldMatrix);
    }
}

void CWeapon::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);
}

HRESULT CWeapon::Render()
{
    return S_OK;
}

// 타이머 관련한 함수들을 모아둔 클래스
void CWeapon::Update_Timer(_float fTimeDelta)
{
    Update_ColliderFrame(fTimeDelta);
}

#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.

// 기본적으로 Owner랑은 충돌 처리 자체를 안한다.
void CWeapon::On_Collision_Enter(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Stay(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Exit(CGameObject* pOther)
{
}

/* 무기 콜라이더 활성화 */
void CWeapon::Activate_ColliderFrame(_float fDuration)
{
    m_pColliderCom->Set_Active(true);
    m_IsColliderActive = true;
    m_fColliderLifeTime = fDuration;

}

void CWeapon::Activate_Collider()
{
    m_pColliderCom->Set_Active(true);
    m_IsColliderActive = true;
}

void CWeapon::Deactivate_Collider()
{
    m_pColliderCom->Set_Active(false);
    m_IsColliderActive = false;
}

/* 무기 콜라이더 업데이트 및 비활성화 관리 */
void CWeapon::Update_ColliderFrame(_float fTimeDelta)
{
    if (m_IsColliderActive)
    {
        m_fColliderLifeTime -= fTimeDelta;
        if (m_fColliderLifeTime <= 0.f)
        {
            m_pColliderCom->Set_Active(false);
            m_IsColliderActive = false;
        }
    }
}
#pragma endregion

HRESULT CWeapon::Ready_Components()
{
    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    return S_OK;
}


void CWeapon::Free()
{
    CPartObject::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
