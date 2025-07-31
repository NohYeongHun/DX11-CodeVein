
CBT_Monster_IsAlive::CBT_Monster_IsAlive(CMonster* pOwner)
	: m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
     Safe_AddRef(m_pGameInstance);
}

/* 몬스터가 살았는가? */
_bool CBT_Monster_IsAlive::Check_Condition()
{
    return !m_pOwner->Is_Dead();
}


CBT_Monster_IsAlive* CBT_Monster_IsAlive::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsAlive");
        return nullptr;
    }
        


    return new CBT_Monster_IsAlive(pOwner);
}

void CBT_Monster_IsAlive::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
