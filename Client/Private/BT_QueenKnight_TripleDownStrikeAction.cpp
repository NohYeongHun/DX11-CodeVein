
CBT_QueenKnight_TripleDownStrikeAction::CBT_QueenKnight_TripleDownStrikeAction(CQueenKnight* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"CBT_QueenKnight_TripleDownStrikeAction";

    /* 공격 애니메이션이 총 3단계로 나뉨 */

    // 1. 점프하는 판정
    m_fJump_StartRatio = 40.f / 224.f;
    m_fJump_EndRatio = 50.f / 224.f;

    // 4. 공격 프레임.
    m_fSkill_StartRatio = 125.f / 250.f;
    m_fSKill_EndRatio = 160.f / 250.f;


}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Perform_Action(_float fTimeDelta)
{
    // DEAD 상태라면 Update 도중에 멈추고 다음 프레임으로 넘아가야함.
    if (m_pOwner->HasBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return Enter_Attack(fTimeDelta);
    case ATTACK_PHASE::READY:
        return Update_Ready(fTimeDelta);
    case ATTACK_PHASE::ASCEND:
        return Update_Ascend(fTimeDelta);
    case ATTACK_PHASE::WAIT_TELEPORT:
        return Update_WaitTeleport(fTimeDelta);
    case ATTACK_PHASE::TELEPORT:
        return Update_Teleport(fTimeDelta);
    case ATTACK_PHASE::HANG:
        return Update_Hang(fTimeDelta);
    //case ATTACK_PHASE::DESCEND:
    //    return Update_Descend(fTimeDelta);
    case ATTACK_PHASE::SKILL:
        return Update_Skill(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_QueenKnight_TripleDownStrikeAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;
    m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
    m_pOwner->Reset_Collider_ActiveInfo();
    m_pOwner->Set_Visible(true);
    m_IsChangeSpeed = false;

    // 위치 변수들을 명확히 초기화
    m_vAscendTarget = _float3{ 0.f, 0.f, 0.f };
    m_vDesecndTarget = _float3{ 0.f, 0.f, 0.f };
    
    // 시간 변수 초기화
    m_fWaitTime = 0.f;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Enter_Attack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }
    // 현재 상태가 PHASE_ATTACK을 사용하지 못한다면 => 쿨타임이 돌고 있다면
    //if (m_pOwner->HasBuff(CQueenKnight::QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN))
    //{
    //    return BT_RESULT::FAILURE;
    //}


    // 1. 다음 단계로 진행 => 플레이어 머리 위에서 내려찍는 거라. 회전이 굳이 필요는 없다?
    m_eAttackPhase = ATTACK_PHASE::READY;

    // 2. 애니메이션 전환.
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE");
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);
  

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Ready(_float fTimeDelta)
{
    m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta);

    if (m_pOwner->Get_CurrentAnimationRatio() >= m_fJump_StartRatio)
    {
        // 1. Phase 전환
        m_eAttackPhase = ATTACK_PHASE::ASCEND;

        // 2. 애니메이션 전환.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"FALL_LOOP");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);

        // 3. Navigation 끄기
        m_pOwner->AddBuff(CMonster::BUFF_NAVIGATION_OFF, 10.f);

        // 4. 목표 위치 지정.
        _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
        vOwnerPos.m128_f32[1] += 10.f;

        XMStoreFloat3(&m_vAscendTarget, vOwnerPos);

        m_pOwner->RotateTurn_ToTarget();

        // 5. 렌더링 끄기.
        m_pOwner->Set_Visible(false);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Ascend(_float fTimeDelta)
{
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);

    // 1. 목표 높이까지 도달했는지 확인. 아니면 위로 이동.
    if (vOwnerPos.m128_f32[1] <= m_vAscendTarget.y)
    {
        m_pOwner->Move_Direction({ 0.f, 1.f, 0.f }, fTimeDelta * 0.3f);
    }
    else
    {
        // 2. 상승 완료 후 대기 페이즈로 전환
        m_eAttackPhase = ATTACK_PHASE::WAIT_TELEPORT;
        
        // 3. 대기 시간 초기화
        m_fWaitTime = 0.f;
    }


    return BT_RESULT::RUNNING;
}

// 상승 후 대기
BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_WaitTeleport(_float fTimeDelta)
{
    // 대기 시간 증가
    m_fWaitTime += fTimeDelta;

    // 지정된 시간이 지나면 텔레포트 페이즈로 전환
    if (m_fWaitTime >= m_fMaxWaitTime)
    {
        m_eAttackPhase = ATTACK_PHASE::TELEPORT;
    }

    return BT_RESULT::RUNNING;
}

// 맵 중앙으로 텔레포트
BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Teleport(_float fTimeDelta)
{
    // 1. 맵 중앙으로 이동 (1.f, 1.f, 1.f)
    _vector vCenterPos = XMVectorSet(1.f, 1.f, 1.f, 1.f);
    m_pOwner->Get_Transform()->Set_State(STATE::POSITION, vCenterPos);

    // 2. Navigation 활성화
    m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
    
    // 3. 맵 중앙으로 이동한 후 Visible 활성화
    m_pOwner->Set_Visible(true);

    // 4. 애니메이션 전환
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE");
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);

    // 5. 플레이어 후방으로 이동할 목표 지점 계산
    _vector vTargetPos = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::POSITION);
    _vector vTargetLook = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::LOOK);

    // Y값은 뺀 Look 값 구하기.
    vTargetLook = XMVectorSetY(vTargetLook, 0.f);

    // 후방으로 12.f 길이만큼 이동.
    vTargetPos -= vTargetLook * 12.f;

    // 목표 하강 지점 (플레이어 머리 위측)
    XMStoreFloat3(&m_vDesecndTarget, vTargetPos);
    m_vDesecndTarget.y += 1.0f;  // 머리 위 1유닛 높이로 설정

    // 6. 다음 페이즈로 전환
    m_eAttackPhase = ATTACK_PHASE::HANG;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Hang(_float fTimeDelta)
{
    m_pOwner->RotateTurn_ToTarget();

    // 1. 현재 위치
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _vector vTargetPos = XMLoadFloat3(&m_vDesecndTarget);

    // 2. Y축 거리만 확인 (수평 거리는 이미 텔레포트로 도착했음)
    _float fYDistance = abs(XMVectorGetY(vOwnerPos) - XMVectorGetY(vTargetPos));

    if (fYDistance >= 0.5f)  // Y축 거리가 0.5 이상일 때만 하강
    {
        // 목표 Y 위치보다 높을 때만 하강
        if (XMVectorGetY(vOwnerPos) > XMVectorGetY(vTargetPos))
        {
            m_pOwner->Move_Direction({ 0.f, -1.f, 0.f }, fTimeDelta * 0.2f);
        }
        else
        {
            // 목표 지점에 도착했으므로 공격 페이즈로 전환
            m_eAttackPhase = ATTACK_PHASE::SKILL;
            m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
            m_pOwner->Compute_OnCell();

            _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE_SKILL");
            m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);
            
        }
    }
    else
    {
        // 목표 지점에 가까우면 공격 페이즈로 전환
        m_eAttackPhase = ATTACK_PHASE::SKILL;
        m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
        m_pOwner->Compute_OnCell();

        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE_SKILL");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);

        
    }

    return BT_RESULT::RUNNING;
}

//BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Descend(_float fTimeDelta)
//{
//    if (m_pOwner->Get_CurrentAnimationRatio() >= m_fJump_EndRatio)
//    {
//        m_eAttackPhase = ATTACK_PHASE::SKILL;
//        // 1. 애니메이션 전환.
//        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE_SKILL");
//        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);
//    }
//
//    return BT_RESULT::RUNNING;
//}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Skill(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;

        // 1. 애니메이션 전환.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, false);

        // 2. 쿨타임 제어
        m_pOwner->AddBuff(CQueenKnight::QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN);

        m_pOwner->RotateTurn_ToTarget();
    }

    return BT_RESULT::RUNNING;
}


CBT_QueenKnight_TripleDownStrikeAction* CBT_QueenKnight_TripleDownStrikeAction::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_TripleDownStrikeAction");
        return nullptr;
    }

    return new CBT_QueenKnight_TripleDownStrikeAction(pOwner);
}

void CBT_QueenKnight_TripleDownStrikeAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
