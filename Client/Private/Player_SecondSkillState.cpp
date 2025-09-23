#include "Player_SecondSkillState.h"
CPlayer_SecondSkillState::CPlayer_SecondSkillState()
{
}

HRESULT CPlayer_SecondSkillState::Initialize(_uint iStateNum, void* pArg)
{
    if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
        return E_FAIL;

#pragma region 콜라이더 관리
    /* Active와 동시에 Collider ActiveMap에 넣어둡니다. */
    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , COLLIDER_ACTIVE_INFO{ 65.f / 289.f, 80.f / 289.f,true, CPlayer::PART_WEAPON, 0 });

    Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , COLLIDER_ACTIVE_INFO{ 100.f / 289.f, 130.f / 289.f,true, CPlayer::PART_WEAPON, 1 });

    m_fIncreaseDamage = 10.f; // 기본 공격력 증가량 설정
#pragma endregion




#pragma region 애니메이션 스피드 제어
    _float fOriginSpeed = m_pModelCom->Get_AnimSpeed(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE")));
    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , ANIMATION_SPEED_INFO{ 0.f / 289.f, 70.f / 289.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , fOriginSpeed, 3.f });

    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , ANIMATION_SPEED_INFO{ 71.f / 289.f, 120.f / 289.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , fOriginSpeed, 2.5f });

    // 애니메이션 끝.
    Add_AnimationSpeed_Info(m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , ANIMATION_SPEED_INFO{ 121.f / 289.f, 289.f / 289.f, 0, m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"))
        , fOriginSpeed, 3.f });

#pragma endregion

    m_fFirstEventFrame = 43.f / 289.f;

    return S_OK;
}

void CPlayer_SecondSkillState::Enter(void* pArg)
{
    m_pPlayer->AddBuff(CPlayer::BUFF_INVINCIBLE, 20.f);
    m_pPlayer->Increase_Damage(m_fIncreaseDamage);

    SECONDSKILL_ENTER_DESC* pDesc = static_cast<SECONDSKILL_ENTER_DESC*>(pArg);
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

    SKILLEXECUTE_DESC Desc{};
    Desc.iSkillPanelIdx = CHUD::SKILLPANEL::SKILL_PANEL_TOP;
    Desc.iSlotIdx = 1;
    Desc.fSkillCoolTime = m_pFsm->Get_StateCoolTime(CPlayer::SKILL_2);
    m_pGameInstance->Publish(EventType::SKILL_EXECUTE, &Desc);

    // 스테미나
    STEMINA_CHANGE_DESC SteminaDesc{};
    SteminaDesc.bIncrease = false;
    SteminaDesc.fStemina = 40.f;
    SteminaDesc.fTime = 1.f;
    m_pGameInstance->Publish(EventType::STEMINA_CHANGE, &SteminaDesc);


    // 오라 이펙트
    CEffect_PlayerSkill::EFFECT_PLAYERSKILL_ACTIVATE_DESC Effect_PlayerSkillDesc{};
    Effect_PlayerSkillDesc.eCurLevel = m_pPlayer->Get_CurrentLevel();
    Effect_PlayerSkillDesc.pTargetTransform = m_pPlayer->Get_Transform();
    Effect_PlayerSkillDesc.fDuration = 10.f; // 지속시간,,
    Effect_PlayerSkillDesc.vStartPos = { 0.f, 0.f, 0.f }; // { 발 }
    
    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_pGameInstance->Get_CurrentLevelID())
    	, TEXT("PLAYER_AURA"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CEffect_PlayerSkill::EffectType), &Effect_PlayerSkillDesc);

    m_IsFirstEvent = false;
    m_IsSecondEvent = false;
    m_IsThirdEvent = false;
    m_IsFourthEvent = false;
    m_IsFifthEvent = false;
    
}

void CPlayer_SecondSkillState::Update(_float fTimeDelta)
{
    
    Handle_Input();
    Handle_Unified_Direction_Input(fTimeDelta);
    Change_State();

    Update_Event(fTimeDelta);
    // 콜라이더 맵 관리.
    CPlayerState::Handle_Collider_State();
    // 스피드 관리
    CPlayerState::Handle_AnimationSpeed_State();
}

void CPlayer_SecondSkillState::Exit()
{
    m_pPlayer->RemoveBuff(CPlayer::BUFF_INVINCIBLE, true);
    // 무기 콜라이더 강제 비활성화
    Force_Disable_All_Colliders();

    // 애니메이션 맵 정상화
    Reset_AnimationSpeedInfo();

    if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
    {
        m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
    }

    m_pPlayer->Decrease_Damage(m_fIncreaseDamage);
}

void CPlayer_SecondSkillState::Reset()
{
    m_eDir = { ACTORDIR::END };
    m_iCurAnimIdx = -1;
    m_iNextAnimIdx = -1;
}

void CPlayer_SecondSkillState::Change_State()
{
    CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
    CPlayer_RunState::RUN_ENTER_DESC Run{};


    if (m_pModelCom->Get_Current_Ratio() >= (240.f / 289.f))
    {
        /* 240 ~ 250 중간에 Run키 누르면. 탈출. */
        if (m_pPlayer->Is_MovementKeyPressed())
        {
            m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
            m_iNextState = CPlayer::PLAYER_STATE::RUN;
            Run.iAnimation_Idx = m_iNextAnimIdx;
            m_pFsm->Change_State(m_iNextState, &Run);
            return;
        }

        /* 250 넘었으면 강제 탈출. */
        if (m_pModelCom->Get_Current_Ratio() >= (250.f / 289.f))
        {
            m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
            Idle.iAnimation_Idx = m_iNextAnimIdx;
            m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
            return;
        }
            
    }


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

void CPlayer_SecondSkillState::Create_WindEffect(void* pArg)
{
    // 2. 검풍 생성 부분 수정
      // 플레이어 정면 계산
    _matrix playerWorld = m_pPlayer->Get_Transform()->Get_WorldMatrix();
    _vector vPlayerPos = playerWorld.r[3];
    _vector vPlayerForward = XMVector3Normalize(playerWorld.r[2]);
    _vector vPlayerUp = XMVector3Normalize(playerWorld.r[1]);

    // 플레이어 앞 (타격 지점)
    _float fHitDistance = 6.0f;
    _vector vHitPos = vPlayerPos + (vPlayerForward * fHitDistance);
    vHitPos += vPlayerUp * 0.7f;

    CEffect_Wind::EFFECTWIND_ACTIVATE_DESC WindActivate_Desc{};
    WindActivate_Desc.eCurLevel = m_pPlayer->Get_CurrentLevel();
    WindActivate_Desc.fDuration = 1.5f;

    _float3 vHitPoint;
    XMStoreFloat3(&vHitPoint, vHitPos);
    WindActivate_Desc.vStartPos = vHitPoint;  // ⭐ 타격 지점을 시작 위치로 지정
    WindActivate_Desc.bUseWorldPosition = true;
    WindActivate_Desc.fCreateDelay = 0.12f;
    WindActivate_Desc.iWindCount = 3;
    WindActivate_Desc.vStartScale = { 5.f, 5.f, 5.f };


    m_pGameInstance->Move_Effect_ToObjectLayer(
        ENUM_CLASS(m_pGameInstance->Get_CurrentLevelID()),
        TEXT("SWORD_WIND"), TEXT("Layer_Effect"), 1,
        ENUM_CLASS(CEffect_Wind::EffectType), &WindActivate_Desc);
    m_pGameInstance->PlaySoundEffect(L"CirculatePulse1.ogg", 0.2f);
}

void CPlayer_SecondSkillState::Update_Event(_float fTimeDelta)
{
    _float fCurrentRatio = m_pModelCom->Get_Current_Ratio();

    if (!m_IsFirstEvent && fCurrentRatio >= m_fFirstEventFrame)
    {
        m_IsFirstEvent = true;
        Create_WindEffect(nullptr);
    }


}

CPlayer_SecondSkillState* CPlayer_SecondSkillState::Create(_uint iStateNum, void* pArg)
{
    CPlayer_SecondSkillState* pInstance = new CPlayer_SecondSkillState();
    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Create Failed : Player CPlayer_SkillState"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_SecondSkillState::Free()
{
    CPlayerState::Free();
}
