
CBT_GiantWhiteDevil_ComboAttackAction::CBT_GiantWhiteDevil_ComboAttackAction(CGiant_WhiteDevil* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_GiantWhiteDevil_ComboAttackAction";
}

BT_RESULT CBT_GiantWhiteDevil_ComboAttackAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return EnterAttack(fTimeDelta);
    case ATTACK_PHASE::FIRST_ATTACK:
        return UpdateFirstAttack(fTimeDelta);
    case ATTACK_PHASE::SECOND_ATTACK:
        return UpdateSecondAttack(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return Complete(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

void CBT_GiantWhiteDevil_ComboAttackAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;
    m_pOwner->Reset_Collider_ActiveInfo();
    m_IsFirstAttack = false;
    m_IsSecondAttack = false;
}

BT_RESULT CBT_GiantWhiteDevil_ComboAttackAction::EnterAttack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }

    m_eAttackPhase = ATTACK_PHASE::FIRST_ATTACK;

    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"COMBO_ATTACK1");
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);
    m_pOwner->Reset_Collider_ActiveInfo();

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_GiantWhiteDevil_ComboAttackAction::UpdateFirstAttack(_float fTimeDelta)
{
    // 0. 바로 바뀐다.
    if (!m_IsFirstAttack)
    {
        m_pOwner->RotateTurn_ToTargetYaw();
        m_IsFirstAttack = true;
    }

    if (m_pOwner->Get_CurrentAnimationRatio() > 0.5f)
    {
        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"COMBO_ATTACK2");
        
        // 2. 공격 상태로 변경
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);

        // 3. 콜라이더 상태 초기화
        m_pOwner->Reset_Collider_ActiveInfo();

        // 4. Phase 변경.
        m_eAttackPhase = ATTACK_PHASE::SECOND_ATTACK;
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_GiantWhiteDevil_ComboAttackAction::UpdateSecondAttack(_float fTimeDelta)
{
   /* if (!m_IsSecondAttack)
    {
        m_pOwner->RotateTurn_ToTargetYaw();
        m_IsSecondAttack = true;
    }*/

    if (m_pOwner->Is_Animation_Finished())
    {
        // 1. IDLE 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. IDLE 상태로 변경
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);

        // 3. PHASE 변경.
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;
    }

    return BT_RESULT::RUNNING;

}

BT_RESULT CBT_GiantWhiteDevil_ComboAttackAction::Complete(_float fTimeDleta)
{
    m_pOwner->AddBuff(CGiant_WhiteDevil::GIANT_BUFF_COMBO_ATTACK);
    return BT_RESULT::SUCCESS;
}

CBT_GiantWhiteDevil_ComboAttackAction* CBT_GiantWhiteDevil_ComboAttackAction::Create(CGiant_WhiteDevil* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_GiantWhiteDevil_ComboAttackAction");
        return nullptr;
    }

    return new CBT_GiantWhiteDevil_ComboAttackAction(pOwner);
}

void CBT_GiantWhiteDevil_ComboAttackAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
