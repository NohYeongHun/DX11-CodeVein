CBT_Monster_DetectAction::CBT_Monster_DetectAction(CMonster* pOwner, _bool IsRootMotion)
    : m_pOwner{ pOwner }
    , m_IsRootMotion{ IsRootMotion }
{
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
    case DETECT_PHASE::COMPLETED:
        return EndDetect(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_DetectAction::Reset()
{
    m_eDetectPhase = DETECT_PHASE::NONE;
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
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
    }


    return BT_RESULT::RUNNING;
}

/* 루트모션 없는 이동만 취급. */
BT_RESULT CBT_Monster_DetectAction::UpdateWalk(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
        m_eDetectPhase = DETECT_PHASE::COMPLETED;
    else
    {
        if (!m_IsRootMotion)
            m_pOwner->Get_Transform()->Go_Straight(fTimeDelta * 0.1f);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DetectAction::EndDetect(_float fTimeDleta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        // 1. Animation 탐색 시작
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. 현재 애니메이션으로 NON 블렌딩하면서 변경. => Idle은 NonBlend로 변경.
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);
    }

    return BT_RESULT::SUCCESS;
}


CBT_Monster_DetectAction* CBT_Monster_DetectAction::Create(CMonster* pOwner, _bool IsRootMotion )
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_DetectAction");
        return nullptr;
    }

    return new CBT_Monster_DetectAction(pOwner, IsRootMotion);
}

void CBT_Monster_DetectAction::Free()
{
    __super::Free();
    m_pOwner = nullptr;
}
