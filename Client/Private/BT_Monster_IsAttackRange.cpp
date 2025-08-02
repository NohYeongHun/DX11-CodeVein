#include "BT_Monster_IsAttackRange.h"

CBT_Monster_IsAttackRange::CBT_Monster_IsAttackRange(CMonster* pOwner)
    : m_pOwner(pOwner)
{
}

_bool CBT_Monster_IsAttackRange::Check_Condition()
{
    return m_pOwner->Is_TargetAttackRange();
}

CBT_Monster_IsAttackRange* CBT_Monster_IsAttackRange::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsAttackRange");
        return nullptr;
    }

    return new CBT_Monster_IsAttackRange(pOwner);
}

void CBT_Monster_IsAttackRange::Free()
{
    __super::Free();
    m_pOwner = nullptr;
}
