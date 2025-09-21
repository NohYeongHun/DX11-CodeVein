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
    // 안전 코드 추가
    if (m_eDetectPhase != DETECT_PHASE::NONE)
    {
        CRASH("Failed Tree Detect Enter Logic");
    }
    
    // 큰 각도 차이가 있으면 먼저 회전, 없으면 바로 추적
   
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
    // 애니메이션이 끝나거나 탐지 범위를 벗어나면 종료
    if (m_pOwner->Is_Animation_Finished() || !m_pOwner->Is_TargetDetectionRange())
    {
        m_eDetectPhase = DETECT_PHASE::END;
        return BT_RESULT::RUNNING;
    }

    //// **핵심**: 회전 각도가 너무 크면 DetectAction을 종료하여 RotateAction이 실행되도록 함
    //if (m_pOwner->Is_RotateCondition())
    //{
    //    // DetectAction 강제 종료 -> Selector가 다음 노드(RotateAction) 실행
    //    m_eDetectPhase = DETECT_PHASE::END;
    //    return BT_RESULT::RUNNING;
    //}

    // 1. 플레이어를 향해 부드럽게 회전 (작은 각도만)
    if (!m_pOwner->IsRotateFinished(XMConvertToRadians(10.f)))
    {
        m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * 2.0f); // 회전 속도 증가
    }
    
    // 2. 플레이어와의 거리 확인
    _float fDistanceToTarget = CalculateDistanceToTarget();
    
    // 3. 거리에 따라 이동 속도 조절
    if (fDistanceToTarget > m_pOwner->Get_MinDetectionRange() * 1.2f) // 공격 범위보다 멀면
    {
        // 플레이어를 향해 천천히 이동
        _vector vTargetDirection = XMVector3Normalize(m_pOwner->Get_TargetVector());
        m_pOwner->Move_Direction(vTargetDirection, fTimeDelta * 0.2f);
    }
    else
    {
        // 적절한 거리에서는 제자리에서 회전만
        _vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
        m_pOwner->Move_Direction(vLook, fTimeDelta * 0.1f);
    }

    return BT_RESULT::RUNNING;
}

// DetectAction 종료 처리
BT_RESULT CBT_Monster_DetectAction::EndDetect(_float fTimeDleta)
{
    m_eDetectPhase = DETECT_PHASE::COMPLETED;
    // 1. Animation 탐색 시작
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

    // 2. 현재 애니메이션으로 NON 블렌딩하면서 변경. => Idle은 NonBlend로 변경.
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);

    // 3. DETECT가 끝났는데 무한으로 Detect 애니메이션 실행을 방지하기 위한 Buff 추가.
    m_pOwner->AddBuff(CMonster::BUFF_DETECT);

    return BT_RESULT::SUCCESS; // SUCCESS로 변경하여 Selector가 Reset 호출하도록 함
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
