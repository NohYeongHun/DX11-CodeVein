#include "BT_SkyBoss_IsInAttackRange.h"

CBT_SkyBoss_IsInAttackRange::CBT_SkyBoss_IsInAttackRange(CSkyBoss* pOwner, _float fAttackRange)
    : m_pOwner { pOwner}
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

_bool CBT_SkyBoss_IsInAttackRange::Check_Condition()
{
    // 기본 체크 검사.
    //if (!m_pOwner || !m_pOwner->Get_Target())
    //    return false;
    //
    //_float fDistanceToTarget = m_pOwner->Get_Distance_To_Target();
    //if (fDistanceToTarget < ATTACK_MIN_RANGE || fDistanceToTarget > ATTACK_MAX_RANGE)
    //    return false;

    return true;
}

CBT_SkyBoss_IsInAttackRange* CBT_SkyBoss_IsInAttackRange::Create(CSkyBoss* pOwner, _float fAttackRange)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_SkyBoss_IsInAttackRange");
        return nullptr;
    }

    return new CBT_SkyBoss_IsInAttackRange(pOwner, fAttackRange);
}

void CBT_SkyBoss_IsInAttackRange::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
