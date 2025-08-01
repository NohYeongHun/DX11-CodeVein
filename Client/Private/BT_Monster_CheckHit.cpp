CBT_Monster_CheckHit::CBT_Monster_CheckHit(CMonster* pOwner)
	: m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
     Safe_AddRef(m_pGameInstance);
}

/* 몬스터가 맞고 있나?*/
_bool CBT_Monster_CheckHit::Check_Condition()
{
    // 맞고 있는게 True
    return true;
    //return m_pOwner->Is_Hit();
}


CBT_Monster_CheckHit* CBT_Monster_CheckHit::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_CheckHit");
        return nullptr;
    }
        


    return new CBT_Monster_CheckHit(pOwner);
}

void CBT_Monster_CheckHit::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
