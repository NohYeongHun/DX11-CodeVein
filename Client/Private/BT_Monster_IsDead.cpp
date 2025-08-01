#include "BT_Monster_IsDead.h"

CBT_Monster_IsDead::CBT_Monster_IsDead(CMonster* pOwner)
	: m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
     Safe_AddRef(m_pGameInstance);
}

/* 몬스터가 살았는가? */
_bool CBT_Monster_IsDead::Check_Condition()
{
    return m_pOwner->Is_Dead();
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
}


