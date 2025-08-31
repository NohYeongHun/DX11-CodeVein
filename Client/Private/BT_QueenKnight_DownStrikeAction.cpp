/* 모두 수정 필요. */
CBT_QueenKnight_DownStrikeAction::CBT_QueenKnight_DownStrikeAction(CQueenKnight* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"CBT_QueenKnight_DownStrikeAction";

    /* 공격 애니메이션이 총 4단계로 나뉨 */
    // 1. 공격준비 땅에 있는 판정.
    m_fReady_StartRatio = 0.f / 224.f;
    m_fReady_EndRatio = 40.f / 224.f;

    // 2. 점프하는 판정
    m_fJump_StartRatio = 40.f / 224.f;
    m_fJump_EndRatio = 50.f / 224.f;

    // 3. 공격 프레임.
    m_fAttack_StartRatio = 60.f / 224.f;
    m_fAttack_EndRatio = 100.f / 224.f;
}

BT_RESULT CBT_QueenKnight_DownStrikeAction::Perform_Action(_float fTimeDelta)
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
    case ATTACK_PHASE::HANG:
        return Update_Hang(fTimeDelta);
    case ATTACK_PHASE::DESCEND:
        return Update_Descend(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_QueenKnight_DownStrikeAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;

    m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
    m_pOwner->Reset_Collider_ActiveInfo();
    m_pOwner->Set_Visible(true);
    m_IsChangeSpeed = false;
    
    // 위치 변수들을 명확히 초기화
    m_vAscendTarget = _float3{ 0.f, 0.f, 0.f };
    m_vDesecndTarget = _float3{ 0.f, 0.f, 0.f };
}

BT_RESULT CBT_QueenKnight_DownStrikeAction::Enter_Attack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }

    // 1. 다음 단계로 진행 => 플레이어 머리 위에서 내려찍는 거라. 회전이 굳이 필요는 없다?
    m_eAttackPhase = ATTACK_PHASE::READY;

    // 2. 애니메이션 전환.
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE");
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);


    

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DownStrikeAction::Update_Ready(_float fTimeDelta)
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
        vOwnerPos.m128_f32[1] += 20.f;

        XMStoreFloat3(&m_vAscendTarget, vOwnerPos);

        m_pOwner->RotateTurn_ToTarget();

        // 5. 렌더링 끄기.
        //m_pOwner->Set_Visible(false);

        // 3. Effect 생성
        m_pOwner->Create_QueenKnightWarp_Effect_Particle({ 0.f, 1.f, 0.f });
    }

    return BT_RESULT::RUNNING;
}

/* 목표 지점까지 상승 */
BT_RESULT CBT_QueenKnight_DownStrikeAction::Update_Ascend(_float fTimeDelta)
{
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);

    // 1. 목표 높이까지 도달했는지 확인. 아니면 위로 이동.
    if (vOwnerPos.m128_f32[1] <= m_vAscendTarget.y)
    {
        m_pOwner->Move_Direction({ 0.f, 1.f, 0.f }, fTimeDelta * 0.3f);
    }
    else
    {
        // 2. 높이를 유지한 채로 순간이동 페이즈로 진입.
        m_eAttackPhase = ATTACK_PHASE::HANG;

        // 3. 순간이동 페이즈에 진입할때 Player 머리 위로 위치 이동.
        _float vPosY = vOwnerPos.m128_f32[1];

        _vector vTargetPos = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::POSITION);

        _vector vTargetLook = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::LOOK);

        // Y값은 뺀 Look 값 구하기.
        vTargetLook = XMVectorSetY(vTargetLook, 0.f);

        // 후방으로 4.f 길이만큼 이동.
        vTargetPos += vTargetLook * 4.f;

        // 목표 하강 지점 (플레이어 머리 위측)
        XMStoreFloat3(&m_vDesecndTarget, vTargetPos);
        m_vDesecndTarget.y += 1.0f;  // 머리 위 1유닛 높이로 설정

        // Y값은 위로 설정.
        vTargetPos.m128_f32[1] = vPosY;

        // 후방 5.f에, y 상위 위치로이동.
        m_pOwner->Get_Transform()->Set_State(STATE::POSITION, vTargetPos);

        
        // 4. 애니메이션 전환.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_STRIKE");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);
    }


    return BT_RESULT::RUNNING;
}

/* Visible 끄기.*/
BT_RESULT CBT_QueenKnight_DownStrikeAction::Update_Hang(_float fTimeDelta)
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
            if (m_pOwner->Get_CurrentAnimationRatio() >= m_fAttack_StartRatio)
            {
                m_eAttackPhase = ATTACK_PHASE::DESCEND;
                m_pOwner->Set_Visible(true);
                m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
            }
        }
    }
    else
    {
        if (m_pOwner->Get_CurrentAnimationRatio() >= m_fAttack_StartRatio)
        {
            //m_pOwner->Disable_Collider(CQueenKnight::PART_BODY);

            // 1. 공격 시작 애니메이션에 페이즈 변경.
            m_eAttackPhase = ATTACK_PHASE::DESCEND;

            // 2. 목표 하강지점 도착했으므로 렌더링 켜기
            //m_pOwner->Set_Visible(true);

            // 3. Navigation Off 해제. => 다시 Navigation을 타게 만듭니다.
            m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
        }
    }
    

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DownStrikeAction::Update_Descend(_float fTimeDelta)
{
    if (m_pOwner->Get_CurrentAnimationRatio() >= m_fAttack_EndRatio)
    {
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;

        //m_pOwner->Enable_Collider(CQueenKnight::PART_BODY);

        // 1. 애니메이션 전환.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, false);

        // 2. 쿨타임 제어
        //m_pOwner->AddBuff(CQueenKnight::QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN);

        m_pOwner->RotateTurn_ToTarget();
    }

    return BT_RESULT::RUNNING;
}


CBT_QueenKnight_DownStrikeAction* CBT_QueenKnight_DownStrikeAction::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_DownStrikeAction");
        return nullptr;
    }

    return new CBT_QueenKnight_DownStrikeAction(pOwner);
}

void CBT_QueenKnight_DownStrikeAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
