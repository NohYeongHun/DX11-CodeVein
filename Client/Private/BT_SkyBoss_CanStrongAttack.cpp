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

    // 💀 죽음이나 무력화 상태면 불가능
    if (m_pOwner->Is_Dead() || m_pOwner->Is_Disabled())
        return false;

    // 🎯 타겟과의 거리 체크
    _float fDistanceToTarget = m_pOwner->Get_Distance_To_Target();
    if (fDistanceToTarget < STRONG_ATTACK_MIN_RANGE || fDistanceToTarget > STRONG_ATTACK_MAX_RANGE)
        return false;

    //// ⏰ 쿨타임 체크 (BT 쿨다운 시스템 활용)
    //static const std::wstring STRONG_ATTACK_COOLDOWN_KEY = L"StrongAttack";
    //if (!m_pOwner->IsCooldownReady(STRONG_ATTACK_COOLDOWN_KEY, STRONG_ATTACK_COOLDOWN))
    //    return false;

    //// 🎲 확률 기반 조건들
    //_float fStrongAttackChance = CalculateStrongAttackChance();
    //_float fRandomValue = static_cast<_float>(rand()) / RAND_MAX; // 0.0f ~ 1.0f

    //return fRandomValue <= fStrongAttackChance;

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
    __super::Free();
    Safe_Release(m_pGameInstance);
}