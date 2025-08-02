#include "BT_Monster_IsDead.h"

CBT_Monster_IsDead::CBT_Monster_IsDead(CMonster* pOwner)
	: m_pOwner(pOwner)
{
}

/* 몬스터가 살았는가? */
_bool CBT_Monster_IsDead::Check_Condition()
{
    return m_pOwner->HasBuff(CMonster::BUFF_DEAD);
}


CBT_Monster_IsDead* CBT_Monster_IsDead::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsDead");
        return nullptr;
    }

    return new CBT_Monster_IsDead(pOwner);
}

void CBT_Monster_IsDead::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    m_pOwner = nullptr;
}


