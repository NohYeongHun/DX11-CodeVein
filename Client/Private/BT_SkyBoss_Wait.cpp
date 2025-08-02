CBT_SkyBoss_Wait::CBT_SkyBoss_Wait(_float fWaitTime)
    : m_fWaitTime(fWaitTime)
{
}

BT_RESULT CBT_SkyBoss_Wait::Execute(_float fTimeDelta)
{
    CMonster* pMonster = dynamic_cast<CMonster*>(m_pOwner);
    //if (pMonster)
    //    pMonster->Change_State(MONSTER_IDLE);
    //
    //m_fCurrentTime += fTimeDelta;
    //
    //if (m_fCurrentTime >= m_fWaitTime)
    //    return BT_RESULT::SUCCESS;

    return BT_RESULT::RUNNING;
}

void CBT_SkyBoss_Wait::Reset()
{
    m_fCurrentTime = 0.f;
}

void CBT_SkyBoss_Wait::Free()
{
    __super::Free();
}
