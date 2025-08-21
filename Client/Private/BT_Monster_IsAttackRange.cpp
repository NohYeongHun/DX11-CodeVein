CBT_Monster_IsAttackRange::CBT_Monster_IsAttackRange(CMonster* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_Monster_IsAttackRangeNode";
}

/* 플레이어를 바라보고 있고, 공격 범위안에 들어올 때 공격. */
_bool CBT_Monster_IsAttackRange::Check_Condition()
{
    // 기본 무효 조건들
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_ATTACK_TIME))
        return false;

    // 공격 범위에 있고 각도가 적절할 때만 공격
    return m_pOwner->Is_TargetAttackRange() && !m_pOwner->Is_RotateCondition();
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
    CBTCondition::Free();
    m_pOwner = nullptr;
}
