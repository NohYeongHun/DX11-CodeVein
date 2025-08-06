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

    if (FAILED(__super::Initialize_Clone(pDesc)))
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
    __super::Priority_Update(fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeapon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CWeapon::Render()
{
    return S_OK;
}

void CWeapon::On_Collision_Enter(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Stay(CGameObject* pOther)
{
}

void CWeapon::On_Collision_Exit(CGameObject* pOther)
{
}

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
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
