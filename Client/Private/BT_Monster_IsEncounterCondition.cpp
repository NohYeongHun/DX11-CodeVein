
CBT_Monster_IsEncounterCondition::CBT_Monster_IsEncounterCondition(CMonster* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_Monster_IsEncounterCondition";
}


_bool CBT_Monster_IsEncounterCondition::Check_Condition()
{
    /* Encounter 조건. */
	if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_ATTACK_TIME))
		return false;

    /* 한번이라도 조우했다면? false */
    if (m_pOwner->Is_Encountered())
        return false;

    /* 조우 거리 이내에 들어오면 => Action에서 판단하기. */
    return true;
}

CBT_Monster_IsEncounterCondition* CBT_Monster_IsEncounterCondition::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsEncounterCondition");
        return nullptr;
    }

    return new CBT_Monster_IsEncounterCondition(pOwner);
}

void CBT_Monster_IsEncounterCondition::Free()
{
    CBTCondition::Free();
    m_pOwner = nullptr;
}
