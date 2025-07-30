#include "Monster.h"



CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void* pArg)
{
	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
}

void CMonster::Update(_float fTimeDelta)
{
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::On_Collision_Enter(CGameObject* pOther)
{
}

void CMonster::On_Collision_Stay(CGameObject* pOther)
{
}

void CMonster::On_Collision_Exit(CGameObject* pOther)
{
}

void CMonster::Take_Damage(_float fDamage, CGameObject* pAttacker)
{
}

void CMonster::Heal(_float fHealAmount)
{
}

_float CMonster::Get_Distance_To_Target() const
{
	return _float();
}

void CMonster::Enter_Combat(CGameObject* pTarget)
{
}

void CMonster::Exit_Combat()
{
}

void CMonster::Free()
{
	__super::Free();
}
