
CBT_SkyBoss_NormalHitReaction::CBT_SkyBoss_NormalHitReaction(CSkyBoss* pOwner)
    : m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

/* 일반 피격 반응 수행 */
BT_RESULT CBT_SkyBoss_NormalHitReaction::Perform_Action(_float fTimeDelta)
{
    // 첫 실행시 일반 피격 애니메이션 시작
    if (!m_bAnimationStarted)
    {
        // 1. 타겟과 나의 Direction을 계산해서 방향을 구한다.
        // 
        // 
        //m_pOwner->Get_Target();
        //m_pOwner->Play_Animation(m_iAnimationIndex, false); // 일반 피격 애니메이션 재생
        //m_pOwner->Add_Knockback_Force(5.f);   // 약한 넉백 효과
        m_bAnimationStarted = true;

        // 일반 피격 이펙트 재생 => 피격 시 이펙트 발생. 아직 미구현.
        //m_pOwner->Play_Hit_Effect(false); // false = 일반 피격
    }

    m_fReactionTime += fTimeDelta;

    // 피격 반응 시간 완료
    if (m_fReactionTime >= m_fMaxReactionTime)
    {
        // 피격 상태 초기화
        //m_pOwner->Set_Hit();
        return BT_RESULT::SUCCESS;
    }

    return BT_RESULT::RUNNING;
}

void CBT_SkyBoss_NormalHitReaction::Reset()
{
    m_fReactionTime = 0.f;
    m_bAnimationStarted = false;
}

CBT_SkyBoss_NormalHitReaction* CBT_SkyBoss_NormalHitReaction::Create(CSkyBoss* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create Node");
        return nullptr;
    }
    return new CBT_SkyBoss_NormalHitReaction(pOwner);
}

void CBT_SkyBoss_NormalHitReaction::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}