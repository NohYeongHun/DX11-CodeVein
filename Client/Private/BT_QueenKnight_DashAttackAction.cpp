CBT_QueenKnight_DashAttackAction::CBT_QueenKnight_DashAttackAction(CQueenKnight* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"QueenKnight_FirstDashAttackNodeAction";
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Perform_Action(_float fTimeDelta)
{
    // DEAD 상태라면 Update 도중에 멈추고 다음 프레임으로 넘아가야함.
    if (m_pOwner->HasBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return Enter_Attack(fTimeDelta);
    case ATTACK_PHASE::ROTATING:
        return Update_Rotating(fTimeDelta);
    case ATTACK_PHASE::DODGE_B:
        return Update_Dodge(fTimeDelta);
    case ATTACK_PHASE::FIRST_ATTACK:
        return Update_FirstAttack(fTimeDelta);
    case ATTACK_PHASE::LAST_ATTACK:
        return Update_LastAttack(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return Complete(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

void CBT_QueenKnight_DashAttackAction::Reset()
{
    /* 콜라이더 원본 크기로 되돌리기. */
    //if (m_IsColliderChange)
    //{
    //    CCollider* pColliderCom = dynamic_cast<CCollider*>(m_pOwner->Get_Component(L"Com_Collider"));
    //    CBounding_OBB::BOUNDING_OBB_DESC Desc{};
    //    Desc = *m_OriginDesc;
    //    pColliderCom->Change_BoundingDesc(&Desc);
    //    m_IsColliderChange = false;
    //}

    /* 원본 크기로 되돌리기. */
    m_pOwner->WeaponOBB_ChangeExtents(m_pOwner->Get_WeaponOBBExtents());

    m_eAttackPhase = ATTACK_PHASE::NONE;
    m_pOwner->Reset_Collider_ActiveInfo();
    m_IsChangeSpeed = false;
    

}

BT_RESULT CBT_QueenKnight_DashAttackAction::Enter_Attack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }

    // 현재 상태가 PHASE_ATTACK을 사용하지 못한다면 => 쿨타임이 돌고 있다면
    if (m_pOwner->HasBuff(CQueenKnight::QUEEN_BUFF_DASH_ATTACK_COOLDOWN))
    {
        return BT_RESULT::FAILURE;
    }

    // 1. 다음 단계로 진행
    m_eAttackPhase = ATTACK_PHASE::ROTATING;


    // 2. 버프 추가.
    m_pOwner->AddBuff(CQueenKnight::QUEEN_BUFF_DASH_ATTACK_COOLDOWN);

    // 3. 크기 증가.
    _float3 vExtents = m_pOwner->Get_WeaponOBBExtents();
    m_pOwner->WeaponOBB_ChangeExtents({ 1.f, 1.f, vExtents.z });
    
    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Update_Rotating(_float fTimeDelta)
{
    // 1. 타겟을 향해 Rotate
    m_pOwner->RotateTurn_ToTargetYaw();

    // 라디안 5도 차이까지만 허용
    if (m_pOwner->IsRotateFinished(XMConvertToRadians(5.f)))
    {

        m_pOwner->Set_RootMotionTranslate(true);

        // 2. 여기서 Dodge를 선택할 것인지 바로 돌진할 것인지 결정.
        if (m_pOwner->Is_TargetDodgeRange())
        {
            m_eAttackPhase = ATTACK_PHASE::DODGE_B;
            // 1. 후방 이동 애니메이션 선택
            _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DODGE_B");

            m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);
        }
        else
        {
            m_eAttackPhase = ATTACK_PHASE::FIRST_ATTACK;

            m_pOwner->Set_RootMotionTranslate(true);

            // 1. 공격 애니메이션 선택
            _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DASH_ATTACK_START");

            m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);

            // 2. 콜라이더 상태 초기화
            m_pOwner->Reset_Collider_ActiveInfo();

            // 3. 현재 애니메이션 스피드 저장.
            m_fOriginSpeed = m_pOwner->Get_Animation_Speed(m_pOwner->Get_CurrentAnimation());

            // 4. 스피드 증가.
            m_pOwner->Set_Animation_AddSpeed(
                m_pOwner->Get_CurrentAnimation(), 1.2f);

            return BT_RESULT::RUNNING;
        }
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Update_Dodge(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::FIRST_ATTACK;

        m_pOwner->Set_RootMotionTranslate(true);

        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DASH_ATTACK_START");

        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);


        // 2. 콜라이더 상태 초기화
        m_pOwner->Reset_Collider_ActiveInfo();

        // 3. 현재 애니메이션 스피드 저장.
        m_fOriginSpeed = m_pOwner->Get_Animation_Speed(m_pOwner->Get_CurrentAnimation());

        // 4. 스피드 증가.
        m_pOwner->Set_Animation_AddSpeed(
            m_pOwner->Get_CurrentAnimation(), 1.2f);

        return BT_RESULT::RUNNING;
    }
    else
    {
        if (!m_pOwner->IsRotateFinished(XMConvertToRadians(5.f)))
        {
            m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta);
        }
        else if (m_pOwner->Get_CurrentAnimationRatio() < (60.f / 114.f))
        {
            _vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
            m_pOwner->Move_Direction(vLook * -1.f, fTimeDelta * 0.1f);
        }
    }
        

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Update_FirstAttack(_float fTimeDelta)
{
    // 공격 모션에 빠르게 이동.
    if (m_pOwner->Get_CurrentAnimationRatio() > 0.8f)
    {
        if (!m_IsChangeSpeed)
        {
            m_IsChangeSpeed = true;
            m_pOwner->Set_Animation_Speed(
                m_pOwner->Get_CurrentAnimation(), m_fOriginSpeed);
        }

        _vector vLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
        m_pOwner->Move_Direction(vLook, fTimeDelta * 2.f); // 완료 시점만 속도를 빠르게.
    }

    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::LAST_ATTACK;

        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DASH_ATTACK_END");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);
    }


    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Update_LastAttack(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;

        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_DashAttackAction::Complete(_float fTimeDleta)
{

    m_pOwner->WeaponOBB_ChangeExtents(m_pOwner->Get_WeaponOBBExtents());

    return BT_RESULT::SUCCESS;
}

CBT_QueenKnight_DashAttackAction* CBT_QueenKnight_DashAttackAction::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_DashAttackAction");
        return nullptr;
    }

    return new CBT_QueenKnight_DashAttackAction(pOwner);
}

void CBT_QueenKnight_DashAttackAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
