namespace
{
    constexpr _float ROTATE_DEGREE = 10.f;
    constexpr _float ROTATE_WEIGHT = 2.f;
    constexpr _float DETECT_WEIGHT = 1.2f;
    constexpr _float MOVE_WEIGHT_MAX = 0.2f;
    constexpr _float MOVE_WEIGHT_MIN = 0.1f;
}

CBT_Monster_DetectAction::CBT_Monster_DetectAction(CMonster* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"MonsterIdle_IsDectectActionNode";
}

BT_RESULT CBT_Monster_DetectAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_DETECT))
        return BT_RESULT::FAILURE;
    
    switch (m_eDetectPhase)
    {
    case DETECT_PHASE::NONE:
        return EnterDetect(fTimeDelta);
    case DETECT_PHASE::ROTATING:
        return UpdateRotating(fTimeDelta);
    case DETECT_PHASE::LOOP:
        return UpdateWalk(fTimeDelta);
    case DETECT_PHASE::END:
        return EndDetect(fTimeDelta);
    case DETECT_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_DetectAction::Reset()
{
    m_eDetectPhase = DETECT_PHASE::NONE;
    m_IsStopped = false; // 상태 초기화
}

BT_RESULT CBT_Monster_DetectAction::EnterDetect(_float fTimeDelta)
{
    if (m_eDetectPhase != DETECT_PHASE::NONE)
    {
        CRASH("Failed Tree Detect Enter Logic");
    }
    
    m_pOwner->Set_RootMotionTranslate(true);
   
    m_eDetectPhase = DETECT_PHASE::LOOP;
    // 탐지 애니메이션 시작
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DETECT");
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, true);
    
    

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DetectAction::UpdateRotating(_float fTimeDelta)
{
    // 플레이어를 향해 빠르게 회전
    m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * 3.0f);
    
    // 회전이 완료되면 추적 단계로 이동
    if (m_pOwner->IsRotateFinished(XMConvertToRadians(10.f)))
    {
        m_eDetectPhase = DETECT_PHASE::LOOP;

        // 탐지 애니메이션 시작
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DETECT");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, true);
    }
    
    return BT_RESULT::RUNNING;
}

/* 회전하면서 추적하는 DetectAction */
BT_RESULT CBT_Monster_DetectAction::UpdateWalk(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished() || !m_pOwner->Is_TargetDetectionRange())
    {
        m_eDetectPhase = DETECT_PHASE::END;
        return BT_RESULT::RUNNING;
    }

    if (!m_pOwner->IsRotateFinished(XMConvertToRadians(ROTATE_DEGREE)))
    {
        m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * ROTATE_WEIGHT);
    }

    _float fDistanceToTarget = CalculateDistanceToTarget();
    
    if (fDistanceToTarget > m_pOwner->Get_MinDetectionRange() * DETECT_WEIGHT)
    {
        _vector vTargetDirection = XMVector3Normalize(m_pOwner->Get_TargetVector());
        m_pOwner->Move_Direction(vTargetDirection, fTimeDelta * MOVE_WEIGHT_MAX);
    }
    else
    {
        _vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
        m_pOwner->Move_Direction(vLook, fTimeDelta * MOVE_WEIGHT_MIN);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DetectAction::EndDetect(_float fTimeDleta)
{
    m_eDetectPhase = DETECT_PHASE::COMPLETED;
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);

    m_pOwner->AddBuff(CMonster::BUFF_DETECT);

    return BT_RESULT::SUCCESS;
}

_float CBT_Monster_DetectAction::CalculateDistanceToTarget()
{
    if (!m_pOwner->Get_Target())
        return FLT_MAX;

    _vector vMyPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::POSITION);
    _vector vDistance = vTargetPos - vMyPos;

    return XMVectorGetX(XMVector3Length(vDistance));
}


CBT_Monster_DetectAction* CBT_Monster_DetectAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_DetectAction");
        return nullptr;
    }

    return new CBT_Monster_DetectAction(pOwner);
}



void CBT_Monster_DetectAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
