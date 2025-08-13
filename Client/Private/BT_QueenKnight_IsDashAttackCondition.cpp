
CBT_QueenKnight_IsDashAttackCondition::CBT_QueenKnight_IsDashAttackCondition(CQueenKnight* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_QueenKnightDashAttackConditionNode";
}

_bool CBT_QueenKnight_IsDashAttackCondition::Check_Condition()
{
    if (m_pOwner->HasAnyBuff(CQueenKnight::QUEEN_BUFF_DASH_ATTACK_COOLDOWN | CMonster::BUFF_DEAD))
        return false;

    return m_pOwner->Is_TargetDashRange();
}

CBT_QueenKnight_IsDashAttackCondition* CBT_QueenKnight_IsDashAttackCondition::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_IsDashAttackCondition");
        return nullptr;
    }

    return new CBT_QueenKnight_IsDashAttackCondition(pOwner);
}

void CBT_QueenKnight_IsDashAttackCondition::Free()
{
    CBTCondition::Free();
    m_pOwner = nullptr;
}
