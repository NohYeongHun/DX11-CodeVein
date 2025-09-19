CPlayer_FirstSkillState::CPlayer_FirstSkillState()
{
}

HRESULT CPlayer_FirstSkillState::Initialize(_uint iStateNum, void* pArg)
{
    if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
        return E_FAIL;

#pragma region 콜라이더 관리.
    // 1. 앞찌르기
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 30.f / 232.f, 40.f / 232.f, true, CPlayer::PART_WEAPON, 0 });

    // 2. 베기
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 50.f / 232.f, 60.f / 232.f, true, CPlayer::PART_WEAPON, 1 });

    // 3. 왼쪽 아래부터 오른쪽 위로 베기
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 65.f / 232.f, 75.f / 232.f, true, CPlayer::PART_WEAPON, 2 });

    // 4. 왼쪽 위부터 오른쪽 아래로 베기
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 90.f / 232.f, 100.f / 232.f, true, CPlayer::PART_WEAPON, 3 });

    // 5. 왼쪽 위부터 중간 아래로 베기.
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 110.f / 232.f, 120.f / 232.f, true, CPlayer::PART_WEAPON, 4 });


    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , COLLIDER_ACTIVE_INFO{ 0.f / 232.f, 230.f / 232.f, false, CPlayer::PART_BODY, 0 });


#pragma endregion

#pragma region Animation Trail Info
    Add_AnimationTrail_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , TRAIL_ACTIVE_INFO{ 30.f / 232.f, 45.f / 232.f
        , m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE")), true });

    Add_AnimationTrail_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , TRAIL_ACTIVE_INFO{ 48.f / 232.f, 60.f / 232.f
        , m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE")), true });

    Add_AnimationTrail_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , TRAIL_ACTIVE_INFO{ 65.f / 232.f, 105.f / 232.f
        , m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE")), true });

    Add_AnimationTrail_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , TRAIL_ACTIVE_INFO{ 108.f / 232.f, 120.f / 232.f
        , m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE")), true });
#pragma endregion



#pragma region 애니메이션 스피드 제어
    _float fOriginSpeed = m_pModelCom->Get_AnimSpeed(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE")));
    // 앞 찌르기
    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , ANIMATION_SPEED_INFO{1.f / 232.f, 40.f / 232.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , fOriginSpeed, 2.f});

    // 회전 공격 
    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , ANIMATION_SPEED_INFO{ 41.f / 232.f, 120.f / 232.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , fOriginSpeed, 1.5f });

    // 끝.
    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , ANIMATION_SPEED_INFO{ 121.f / 232.f, 232.f / 232.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"))
        , fOriginSpeed, 1.8f });

#pragma endregion




	m_fIncreaseDamage = 10.f; // 기본 공격력 증가량 설정

    // 1. 스테미나 감소.
    STEMINA_CHANGE_DESC SteminaDesc{};
    SteminaDesc.bIncrease = false;
    SteminaDesc.fStemina = 35.f;
    SteminaDesc.fTime = 1.f;
    m_pGameInstance->Publish(EventType::STEMINA_CHANGE, &SteminaDesc);

    return S_OK;
}

void CPlayer_FirstSkillState::Enter(void* pArg)
{
    FIRSTSKILL_ENTER_DESC* pDesc = static_cast<FIRSTSKILL_ENTER_DESC*>(pArg);
    CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

    m_pModelCom->Set_RootMotionTranslate(true);

    m_isLoop = false;

    m_iNextAnimIdx = -1;
    m_iCurAnimIdx = pDesc->iAnimation_Idx;
    m_eDir = pDesc->eDirection;
    m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

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
    Desc.fSkillCoolTime = m_pFsm->Get_StateCoolTime(CPlayer::SKILL_1);
    m_pGameInstance->Publish(EventType::SKILL_EXECUTE, &Desc);

    
    /* 기본 플레이어 공격 데미지 증가. */
    m_pPlayer->Increase_Damage(m_fIncreaseDamage);


    //// 2. 검풍 생성 부분 수정
    //CEffect_Wind::EFFECTWIND_ACTIVATE_DESC WindActivate_Desc{};
    //WindActivate_Desc.eCurLevel = m_pPlayer->Get_CurrentLevel();
    //WindActivate_Desc.fDuration = 1.f; // 지속시간,,

    //WindActivate_Desc.vStartPos = { 0.f, 2.f, 5.f }; // {오른쪽, 위, 앞}

    ////WindActivate_Desc.vStartRotation = { 0.f, 270.f, 0.f }; // 각도 단위

    //_vector vCamLook = m_pGameInstance->Get_MainCamera()->Get_Transform()->Get_State(STATE::LOOK);
    //XMStoreFloat3(&WindActivate_Desc.vRotationAxis, XMVector3Normalize(vCamLook));

    //WindActivate_Desc.pParentMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrixPtr();
    //WindActivate_Desc.pTargetTransform = m_pPlayer->Get_Transform();

    //m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_pGameInstance->Get_CurrentLevelID())
    //    , TEXT("SWORD_WIND"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CEffect_Wind::EffectType), &WindActivate_Desc);
}

void CPlayer_FirstSkillState::Update(_float fTimeDelta)
{

    //Handle_Input();
    Handle_Unified_Direction_Input(fTimeDelta);
    Change_State();
    CPlayerState::Handle_Collider_State();
    CPlayerState::Handle_AnimationSpeed_State();
    CPlayerState::Handle_AnimationTrail_State();
}

void CPlayer_FirstSkillState::Exit()
{
    // 다시 감소.
    m_pPlayer->Decrease_Damage(m_fIncreaseDamage);

    // 무기 콜라이더 강제 비활성화
    Force_Disable_All_Colliders();

    // 애니메이션 맵 정상화
    Reset_AnimationSpeedInfo();

    // Trail Event 정보 초기화
    Reset_AnimationTrailInfo();

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
