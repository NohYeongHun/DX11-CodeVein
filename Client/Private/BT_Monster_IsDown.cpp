#include "BT_Monster_IsDown.h"

CBT_Monster_IsDown::CBT_Monster_IsDown(CMonster* pOwner)
	: m_pOwner(pOwner)
{
    m_strTag = L"MonsterIdle_HitDownConditionNode";
}

/* 몬스터가 Down 상태인가? */
_bool CBT_Monster_IsDown::Check_Condition()
{
    // 현재 Down CoolDown이 돌고 있다면? 다시 못해야됨.

	return m_pOwner->HasBuff(CMonster::BUFF_DOWN);
}

CBT_Monster_IsDown* CBT_Monster_IsDown::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsDown");
        return nullptr;
    }
    return new CBT_Monster_IsDown(pOwner);
}

void CBT_Monster_IsDown::Free()
{
    CBTCondition::Free();
    m_pOwner = nullptr;
}
