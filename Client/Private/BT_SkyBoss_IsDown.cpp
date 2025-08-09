#include "BT_SkyBoss_IsDown.h"
CBT_SkyBoss_DownCondition::CBT_SkyBoss_DownCondition(CSkyBoss* pOwner, _float fAttackRange)
    : m_pOwner { pOwner}
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

_bool CBT_SkyBoss_DownCondition::Check_Condition()
{
    //if (nullptr == m_pOwner)
    //    return false;

    //// 다운 조건들 체크
    //// 1. 스태거 게이지가 임계점에 도달했는가?
    //if (m_pOwner->Get_StaggerGauge() >= m_fStaggerThreshold)
    //    return true;

    //// 2. 강제 다운 상태인가? (특정 공격에 의한)
    //if (m_pOwner->Is_ForceDown())
    //    return true;

    //// 3. HP가 특정 구간에서 다운되는 패턴인가?
    //_float currentHP = m_pOwner->Get_Current_HP();
    //_float maxHP = m_pOwner->Get_Max_HP();
    //_float hpRatio = currentHP / maxHP;

    //// 75%, 50%, 25% HP에서 강제 다운
    //if (m_pOwner->Should_Phase_Down(hpRatio))
    //    return true;

    return false;
}

void CBT_SkyBoss_DownCondition::Reset()
{
}

CBT_SkyBoss_DownCondition* CBT_SkyBoss_DownCondition::Create(CSkyBoss* pOwner, _float fAttackRange)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_SkyBoss_DownCondition");
        return nullptr;
    }

    return new CBT_SkyBoss_DownCondition(pOwner, fAttackRange);
}

void CBT_SkyBoss_DownCondition::Free()
{
    CBTCondition::Free();
    Safe_Release(m_pGameInstance);
}
