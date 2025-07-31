CBT_SkyBoss_StrongHitReaction::CBT_SkyBoss_StrongHitReaction(CSkyBoss* pOwner, _uint iAnimationIndex)
    : m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
    , m_iAnimationIndex { iAnimationIndex }
{
    Safe_AddRef(m_pGameInstance);
}

/* 강한 피격 반응 수행 */
BT_RESULT CBT_SkyBoss_StrongHitReaction::Perform_Action(_float fTimeDelta)
{
    // 첫 실행시 강한 피격 애니메이션 시작
    if (!m_bAnimationStarted)
    {
        m_pOwner->Change_State(MONSTER_HURT); // Monster 상태를 피격으로 변경
        m_pOwner->Chanage_Animation(m_iAnimationIndex, false); // 강한 피격 애니메이션 재생
        //m_pOwner->Add_Knockback_Force(15.f);  // 넉백 효과
        m_bAnimationStarted = true;

        // 강한 피격 이펙트 재생
        //m_pOwner->Play_Hit_Effect(true); // true = 강한 피격
    }

    
    m_fReactionTime += fTimeDelta;

    // 피격 반응 시간 완료
    if (!m_pOwner->Is_StrongHit())
    {
        // 피격 상태 초기화
        //m_pOwner->Clear_Hit_State();
        return BT_RESULT::SUCCESS;
    }

    // 상태 실행 중
    return BT_RESULT::RUNNING;
}

void CBT_SkyBoss_StrongHitReaction::Reset()
{
    m_fReactionTime = 0.f;
    m_bAnimationStarted = false;
}

CBT_SkyBoss_StrongHitReaction* CBT_SkyBoss_StrongHitReaction::Create(CSkyBoss* pOwner, _uint iAnimationIndex)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create Node");
        return nullptr;
    }
    return new CBT_SkyBoss_StrongHitReaction(pOwner, iAnimationIndex);
}

void CBT_SkyBoss_StrongHitReaction::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}

// ============= CBT_SkyBoss_NormalHitReaction.h =============