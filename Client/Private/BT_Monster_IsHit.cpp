CBT_Monster_IsHit::CBT_Monster_IsHit(CMonster* pOwner)
	: m_pOwner(pOwner)
{
}

/* 몬스터가 살았는가? */
_bool CBT_Monster_IsHit::Check_Condition()
{
    return m_pOwner->HasBuff(CMonster::BUFF_HIT);
}


CBT_Monster_IsHit* CBT_Monster_IsHit::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsHit");
        return nullptr;
    }

    return new CBT_Monster_IsHit(pOwner);
}

void CBT_Monster_IsHit::Free()
{
    __super::Free();
    m_pOwner = nullptr;
}


