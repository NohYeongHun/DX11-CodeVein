CPlayer_FirstSkillState::CPlayer_FirstSkillState()
{
}

HRESULT CPlayer_FirstSkillState::Initialize(_uint iStateNum, void* pArg)
{
    if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
        return E_FAIL;

    /* Active와 동시에 Collider ActiveMap에 넣어둡니다. */
    m_ColliderActiveMap.emplace(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 30.f / 232.f, 140.f / 232.f, false });

    return S_OK;
}

void CPlayer_FirstSkillState::Enter(void* pArg)
{
    FIRSTSKILL_ENTER_DESC* pDesc = static_cast<FIRSTSKILL_ENTER_DESC*>(pArg);
    CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.




    m_isLoop = false;

    m_iNextAnimIdx = -1;
    m_iCurAnimIdx = pDesc->iAnimation_Idx;
    m_eDir = pDesc->eDirection;
    m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

    // 락온 중이면 타겟을 향해 즉시 회전
    if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
    {
        _vector vLockOnDirection = m_pPlayer->Calculate_LockOn_Direction();
        if (!XMVector3Equal(vLockOnDirection, XMVectorZero()))
        {
            // 락온 방향으로 즉시 회전
            _float x = XMVectorGetX(vLockOnDirection);
            _float z = XMVectorGetZ(vLockOnDirection);
            _float fTargetYaw = atan2f(x, z);

            _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetYaw);
            m_pPlayer->Get_Transform()->Set_Quaternion(qNewRot);
        }
    }

    /* UI CoolTime 연동*/
    SKILLEXECUTE_DESC Desc{};
    Desc.iSkillPanelIdx = CHUD::SKILLPANEL::SKILL_PANEL_TOP;
    Desc.iSlotIdx = 0;
    Desc.fSkillCoolTime = 3.f;
    m_pGameInstance->Publish(EventType::SKILL_EXECUTE, &Desc);

    /* 쓰는 도중에 콜라이더 무시해야하나? */
}

void CPlayer_FirstSkillState::Update(_float fTimeDelta)
{
    Handle_Input();
    Handle_Unified_Direction_Input(fTimeDelta);
    Change_State();
    CPlayerState::Handle_Collider_State();
}

void CPlayer_FirstSkillState::Exit()
{
    // 무기 콜라이더 강제 비활성화
    Force_Disable_All_Colliders();
    if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
    {
        m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
    }
}

void CPlayer_FirstSkillState::Reset()
{
    m_eDir = { ACTORDIR::END };
    m_iCurAnimIdx = -1;
    m_iNextAnimIdx = -1;
}

void CPlayer_FirstSkillState::Change_State()
{
    CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
    CPlayer_RunState::RUN_ENTER_DESC Run{};

    if (m_pModelCom->Is_Finished())
    {
        if (m_pPlayer->Is_MovementKeyPressed())
        {
            m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
            m_iNextState = CPlayer::PLAYER_STATE::RUN;
            Run.iAnimation_Idx = m_iNextAnimIdx;
            m_pFsm->Change_State(m_iNextState, &Run);
            return;
        }
        else
        {
            m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
            Idle.iAnimation_Idx = m_iNextAnimIdx;
            m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
            return;
        }
    }

}

CPlayer_FirstSkillState* CPlayer_FirstSkillState::Create(_uint iStateNum, void* pArg)
{
    CPlayer_FirstSkillState* pInstance = new CPlayer_FirstSkillState();
    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Create Failed : Player CPlayer_SkillState"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_FirstSkillState::Free()
{
    CPlayerState::Free();
}
