
CBT_QueenKnight_FirstPhase_AttackAction::CBT_QueenKnight_FirstPhase_AttackAction(CQueenKnight* pOwner)
    : m_pOwner(pOwner)
{
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::Perform_Action(_float fTimeDelta)
{
    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return EnterAttack(fTimeDelta);
    case ATTACK_PHASE::ROTATING:
        return UpdateRotating(fTimeDelta);
    case ATTACK_PHASE::FIRST_ATTACK:
        return UpdateFirstAttack(fTimeDelta);
    case ATTACK_PHASE::SECOND_ATTACK:
        return UpdateSecondAttack(fTimeDelta);
    case ATTACK_PHASE::LAST_ATTACK:
        return UpdateLastAttack(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return Complete(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

void CBT_QueenKnight_FirstPhase_AttackAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::EnterAttack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }

    // 현재 상태가 PHASE_ATTACK을 사용하지 못한다면 => 쿨타임이 돌고 있다면
    if (m_pOwner->HasBuff(CQueenKnight::QUEEN_BUFF_PHASE_ATTACK_COOLDOWN))
    {
        return BT_RESULT::FAILURE;
    }

    // 1. 다음 단계로 진행
    m_eAttackPhase = ATTACK_PHASE::ROTATING;
    m_pOwner->AddBuff(CQueenKnight::QUEEN_BUFF_PHASE_ATTACK_COOLDOWN);

    // 2. 루트모션 설정.
    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::UpdateRotating(_float fTimeDelta)
{
    // 0. 한번에 회전.
    m_pOwner->Rotate_ToTarget(fTimeDelta);

    if (m_pOwner->IsRotateFinished(XMConvertToRadians(10.f))) // 라디안 10도 차이까지만 허용
    {
        m_eAttackPhase = ATTACK_PHASE::FIRST_ATTACK;

        m_pOwner->Set_RootMotionTranslate(false);
        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"STRONG_ATTACK1");

        // 2. 공격 상태로 변경
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

        // 3. Collider 활성화 필요. => 공격용 콜라이더만 활성화.(Weapon?)
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::UpdateFirstAttack(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::SECOND_ATTACK;

        m_pOwner->Set_RootMotionTranslate(false);
        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"STRONG_ATTACK2");

        // 2. 공격 상태로 변경
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

        m_pOwner->RotateTurn_ToTarget();
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::UpdateSecondAttack(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::LAST_ATTACK;

        m_pOwner->Set_RootMotionTranslate(false);
        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"STRONG_ATTACK3");

        // 2. 공격 상태로 변경
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);
        m_pOwner->RotateTurn_ToTarget();
    }
    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::UpdateLastAttack(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;

        m_pOwner->Set_RootMotionTranslate(false);
        // 1. IDLE 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. IDLE 상태로 변경
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_FirstPhase_AttackAction::Complete(_float fTimeDleta)
{
    return BT_RESULT::SUCCESS;
}

CBT_QueenKnight_FirstPhase_AttackAction* CBT_QueenKnight_FirstPhase_AttackAction::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_FirstPhase_AttackAction");
        return nullptr;
    }

    return new CBT_QueenKnight_FirstPhase_AttackAction(pOwner);

}
void CBT_QueenKnight_FirstPhase_AttackAction::Free()
{
    __super::Free();
    m_pOwner = nullptr;
}
