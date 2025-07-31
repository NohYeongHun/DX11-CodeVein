CBT_IsTargetInRange::CBT_IsTargetInRange(CMonster* pOwner, _float fRange)
	: m_pOwner(pOwner)
	, m_fDetectionRange(fRange)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
     Safe_AddRef(m_pGameInstance);
}

/* 플레이어가 해당 거리에 와있는가? */
_bool CBT_IsTargetInRange::Check_Condition()
{
    return m_pOwner->IsTargetInRange();
}


CBT_IsTargetInRange* CBT_IsTargetInRange::Create(CMonster* pOwner, _float fRange)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create Node");
        return nullptr;
    }
        


    return new CBT_IsTargetInRange(pOwner, fRange);
}

void CBT_IsTargetInRange::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
