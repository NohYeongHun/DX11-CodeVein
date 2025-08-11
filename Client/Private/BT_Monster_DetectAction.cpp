CBT_Monster_DetectAction::CBT_Monster_DetectAction(CMonster* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"MonsterIdle_IsDectectActionNode";
}

BT_RESULT CBT_Monster_DetectAction::Perform_Action(_float fTimeDelta)
{
    

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
    m_bIsStopped = false; // 상태 초기화
}

BT_RESULT CBT_Monster_DetectAction::EnterDetect(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eDetectPhase != DETECT_PHASE::NONE)
    {
        CRASH("Failed Tree Detect Enter Logic");
    }

    // 1. 다음 단계로 진행
    m_eDetectPhase = DETECT_PHASE::ROTATING;

    m_pOwner->Set_RootMotionTranslate(true);

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DetectAction::UpdateRotating(_float fTimeDelta)
{
    m_pOwner->Rotate_ToTarget(fTimeDelta);
    if (m_pOwner->IsRotateFinished(XMConvertToRadians(10.f))) // 라디안 10도 차이까지만 허용
    {
        m_eDetectPhase = DETECT_PHASE::LOOP;

        // 1. 탐지 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DETECT");

        // 2. 탐지 상태로 변경
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx, true);
    }


    return BT_RESULT::RUNNING;
}

/* 루트모션 없는 이동만 취급. */
BT_RESULT CBT_Monster_DetectAction::UpdateWalk(_float fTimeDelta)
{
    _float fDistanceToTarget = CalculateDistanceToTarget();
    _float fMinRange = m_pOwner->Get_MinDetectionRange();
    _float fStopRange = fMinRange * 0.8f;  // 멈추는 거리 (더 가깝게)
    _float fResumeRange = fMinRange * 1.2f; // 다시 추적 시작하는 거리 (히스테리시스)
    
    // 멤버 변수로 상태 유지 (각 몬스터별로 독립적)
    
    // 히스테리시스를 사용한 거리 판단
    if (!m_bIsStopped && fDistanceToTarget <= fStopRange)
    {
        // 가까우면 멈춤 상태로 전환
        m_bIsStopped = true;
    }
    else if (m_bIsStopped && fDistanceToTarget >= fResumeRange)
    {
        // 멀어지면 추적 상태로 전환
        m_bIsStopped = false;
    }
    
    // 너무 가까우면 다음 단계로 (공격 범위)
    if (fDistanceToTarget <= fStopRange * 0.7f)
    {
        m_eDetectPhase = DETECT_PHASE::END;
        m_bIsStopped = false; // 상태 초기화
        return BT_RESULT::RUNNING;
    }
    
    // 멈춤 상태가 아닐 때만 이동
    if (!m_bIsStopped)
    {
        // 회전이 필요한지 먼저 확인
        if (!m_pOwner->IsRotateFinished(XMConvertToRadians(20.f))) // 20도 이상 차이나면 회전
        {
            // 회전 중일 때는 이동하지 않음 (떨림 방지)
            m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta);
        }
        else
        {
            // 회전이 완료되면 이동 (속도 감소)
            _vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
            m_pOwner->Move_Direction(vLook, fTimeDelta * 0.15f); // 속도를 더 느리게
        }
    }
    else
    {
        // 멈춤 상태에서도 플레이어 방향으로 회전은 유지 (자연스러운 추적)
        if (!m_pOwner->IsRotateFinished(XMConvertToRadians(30.f))) // 더 큰 임계값으로 불필요한 회전 방지
        {
            m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta);
        }
    }

    return BT_RESULT::RUNNING;
}

// 왜이게 반복돼?
BT_RESULT CBT_Monster_DetectAction::EndDetect(_float fTimeDleta)
{
    m_eDetectPhase = DETECT_PHASE::COMPLETED;
    // 1. Animation 탐색 시작
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

    // 2. 현재 애니메이션으로 NON 블렌딩하면서 변경. => Idle은 NonBlend로 변경.
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);

    return BT_RESULT::RUNNING;
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
