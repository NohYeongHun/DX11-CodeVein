// ============= CBT_SkyBoss_CanStrongAttack.cpp =============

CBT_SkyBoss_CanStrongAttack::CBT_SkyBoss_CanStrongAttack(CSkyBoss* pOwner)
    : m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

/* 강공격이 가능한 상황인가? */
_bool CBT_SkyBoss_CanStrongAttack::Check_Condition()
{
    if (!m_pOwner || !m_pOwner->Get_Target())
        return false;

 

    return true;
}



CBT_SkyBoss_CanStrongAttack* CBT_SkyBoss_CanStrongAttack::Create(CSkyBoss* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create Node");
        return nullptr;
    }
    return new CBT_SkyBoss_CanStrongAttack(pOwner);
}

void CBT_SkyBoss_CanStrongAttack::Free()
{
    CBTCondition::Free();
    Safe_Release(m_pGameInstance);
}