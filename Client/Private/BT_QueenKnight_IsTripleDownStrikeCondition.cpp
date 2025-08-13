
CBT_QueenKnight_IsTripleDownStrikeCondition::CBT_QueenKnight_IsTripleDownStrikeCondition(CQueenKnight* pOwner)
	: m_pOwner(pOwner)
{
	m_strTag = L"CBT_QueenKnight_IsTripleDownStrikeCondition";
}

_bool CBT_QueenKnight_IsTripleDownStrikeCondition::Check_Condition()
{
	if (m_pOwner->HasAnyBuff(CQueenKnight::QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN | CMonster::BUFF_DEAD))
		return false;

	return m_pOwner->Is_TargetDownStrikeRange();
}

CBT_QueenKnight_IsTripleDownStrikeCondition* CBT_QueenKnight_IsTripleDownStrikeCondition::Create(CQueenKnight* pOwner)
{
	if (nullptr == pOwner)
	{
		CRASH("Failed Create CBT_QueenKnight_IsTripleDownStrikeCondition");
		return nullptr;
	}

	return new CBT_QueenKnight_IsTripleDownStrikeCondition(pOwner);
}

void CBT_QueenKnight_IsTripleDownStrikeCondition::Free()
{
	CBTCondition::Free();
	m_pOwner = nullptr;
}
