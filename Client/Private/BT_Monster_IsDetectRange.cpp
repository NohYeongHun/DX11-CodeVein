CBT_Monster_IsDetectRange::CBT_Monster_IsDetectRange(CMonster* pOwner)
	: m_pOwner(pOwner)
{
    m_strTag = L"MonsterIdle_IsDectectConditionNode";
}

/* 플레이어가 해당 거리에 와있는가? */
_bool CBT_Monster_IsDetectRange::Check_Condition()
{
    
    return m_pOwner->Is_TargetDetectionRange();
}


CBT_Monster_IsDetectRange* CBT_Monster_IsDetectRange::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsDetectRange");
        return nullptr;
    }
    return new CBT_Monster_IsDetectRange(pOwner);
}

void CBT_Monster_IsDetectRange::Free()
{
    CBTCondition::Free();
    m_pOwner = nullptr;
}
