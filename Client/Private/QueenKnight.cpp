#include "QueenKnight.h"
CQueenKnight::CQueenKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CQueenKnight::CQueenKnight(const CQueenKnight& Prototype)
    : CMonster(Prototype)
{
}
#pragma region 0. 기본 함수들 정의
HRESULT CQueenKnight::Initialize_Prototype()
{
    if (FAILED(CMonster::Initialize_Prototype()))
        return E_FAIL;

    m_strObjTag = TEXT("QueenKnight");

    return S_OK;
}

HRESULT CQueenKnight::Initialize_Clone(void* pArg)
{
    QUEENKNIGHT_DESC* pDesc = static_cast<QUEENKNIGHT_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(CMonster::Initialize_Clone(pDesc)))
    {
        CRASH("Ready Clone Failed");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Ready Components Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders(pDesc)))
    {
        CRASH("Ready Colliders Failed");
        return E_FAIL;
    }

    if (FAILED(Initialize_Stats()))
    {
        CRASH("Init Stats Failed");
        return E_FAIL;
    }

    if (FAILED(Initailize_UI()))
    {
        CRASH("Init UI Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_Navigations()))
    {
        CRASH("Ready Navigations Failed");
        return E_FAIL;
    }

    if (FAILED(InitializeAction_ToAnimationMap()))
    {
        CRASH("Failed Ready InitializeAction CQueenKnight");
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
    {
        CRASH("Failed Ready Ready PartObject CQueenKnight");
        return E_FAIL;
    }

    if (FAILED(Ready_BehaviorTree()))
    {
        CRASH("Failed Ready BehaviourTree QueenKnight");
        return E_FAIL;
    }

    if (FAILED(Ready_Effects(pDesc)))
    {
        CRASH("Failed Ready Effects QueenKnight");
        return E_FAIL;
    }

    if (FAILED(Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDuration WolfDevil");
        return E_FAIL;
    }

    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    _float3 vScale = { 1.5f, 1.5f, 1.5f };
    m_pTransformCom->Set_Scale(vScale);



    /* 현재 Object Manager에 담기 전에는 모든 Collider를 충돌 비교 하지 않습니다. */
    Collider_All_Active(false);

    
	m_pModelCom->Set_Animation(m_Action_AnimMap[TEXT("IDLE")], true); // 초기 애니메이션은 IDLE로 설정.
    m_pModelCom->Set_Animation(m_Action_AnimMap[TEXT("RUN")], true);

    return S_OK;
}

void CQueenKnight::Priority_Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pBossHpBarUI->Priority_Update(fTimeDelta);

    CMonster::Priority_Update(fTimeDelta);
        
}

void CQueenKnight::Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pBossHpBarUI->Update(fTimeDelta);

    CMonster::Update(fTimeDelta);


    Update_AI(fTimeDelta);

    // 스킬 체크
    Update_BloodPillar(fTimeDelta);

    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    

    Finalize_Update(fTimeDelta);
}

void CQueenKnight::Finalize_Update(_float fTimeDelta)
{
    CMonster::Finalize_Update(fTimeDelta);
}

void CQueenKnight::Late_Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pBossHpBarUI->Late_Update(fTimeDelta);


    if (!HasBuff(CMonster::BUFF_NAVIGATION_OFF))
    {
        m_pTransformCom->Set_State(STATE::POSITION
            , m_pNavigationCom->Compute_OnCell(
                m_pTransformCom->Get_State(STATE::POSITION)));
    }
    

    if (Is_Visible())
    {
        // 보이는 상태일 때만 렌더 그룹에 추가
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
            return;
    }
    

    

    CMonster::Late_Update(fTimeDelta);
}

HRESULT CQueenKnight::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
#endif // _DEBUG

    if (FAILED(Bind_Shader_Resource()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            CRASH("Ready Bine Materials Failed");

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            CRASH("Ready Bone Matrices Failed");

        if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
            CRASH("Ready Shader Begin Failed");

        if (FAILED(m_pModelCom->Render(i)))
            CRASH("Ready Render Failed");
    }
    return S_OK;
}
#pragma endregion




#pragma region 1. 충돌 함수 정의

void CQueenKnight::On_Collision_Enter(CGameObject* pOther)
{
    // 1. 충돌 가능한 타입의 포인터들을 나열.
    CPlayerWeapon* pPlayerWeapon = dynamic_cast<CPlayerWeapon*>(pOther);

    // PlayerWeapon에 충돌 되었을 경우
    if (nullptr != pPlayerWeapon)
    {
        // 0. 무적 상태 체크 
        if (!HasBuff(BUFF_INVINCIBLE))
        {
            // 1. 데미지를 입고.
            Take_Damage(pPlayerWeapon->Get_AttackPower(), pPlayerWeapon);

            // 2. 해당 위치에 검흔 Effect 생성?

            // 3. 무적 버프 추가.
            AddBuff(BUFF_INVINCIBLE);

            AddBuff(BUFF_HIT);
        }
    }

    if (m_MonsterStat.fHP <= 0.f)
        AddBuff(BUFF_DEAD);
}

void CQueenKnight::On_Collision_Stay(CGameObject* pOther)
{
}

void CQueenKnight::On_Collision_Exit(CGameObject* pOther)
{
}

void CQueenKnight::Collider_All_Active(_bool bActive)
{
    m_pColliderCom->Set_Active(bActive);

    if (bActive)
    {
        m_pWeapon->Activate_Collider();
        m_pShield->Activate_Collider();
    }
    else
    {
        m_pWeapon->Deactivate_Collider();
        m_pShield->Deactivate_Collider();
    }
}
void CQueenKnight::Reset_Part_Colliders()
{
    Disable_Collider(PART_WEAPON);

    // 3. 콜라이더 활성화 정보 초기화
    Reset_Collider_ActiveInfo();
}
void CQueenKnight::WeaponOBB_ChangeExtents(_float3 vExtents)
{
    m_pWeapon->OBBCollider_ChangeExtents(vExtents);
}
_float3 CQueenKnight::Get_WeaponOBBExtents()
{
     return m_pWeapon->Get_OriginExtents();
}
#pragma endregion


#pragma region 2. 몬스터 AI 관리
void CQueenKnight::Update_AI(_float fTimeDelta)
{
    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    Tick_BuffTimers(fTimeDelta);

    /* Bind 용 Dissolve Time 계산.*/
    //if (HasBuff(CMonster::BUFF_DISSOLVE))
    //m_fCurDissolveTime = Get_DefaultBuffTime(BUFF_DISSOLVE) - Get_BuffTime(BUFF_DISSOLVE); // 점차 보내는 값이 증가.

    if (HasBuff(CMonster::BUFF_DISSOLVE))
        m_fCurDissolveTime = m_fMaxDissolveTime - Get_BuffTime(BUFF_DISSOLVE); // 점차 보내는 값이 증가.
    else if (HasBuff(CMonster::BUFF_REVERSEDISSOLVE))
        m_fCurDissolveTime = Get_BuffTime(BUFF_REVERSEDISSOLVE); // 점차 값이 감소.
    else
        m_fCurDissolveTime = 0.f; // 버프가 사라지면서 잔여값이 남아서 Texture Reverse Dissolve가 모두 동작하지 않음.
    

    /* 콜라이더 활성화 구간 확인 */
    CMonster::Handle_Collider_State();
    /* Trail 활성화 구간 확인. */
    CMonster::Handle_Trail_State();

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }
}
#pragma endregion



#pragma region 3. 몬스터는 자신에게 필요한 수치값들을 초기화해야 합니다.
HRESULT CQueenKnight::Initialize_Stats()
{
    m_fMinDetectionDistance = 5.f;
    m_fDashMaxDistance = 25.f;
    m_fDashMinDistance = 5.f;
    m_fDashDodgeDistance = 10.f;
    m_fDownStrikeDistance = 30.f;
    m_fEncounterDistance = 30.f;

    return S_OK;
}
_bool CQueenKnight::Is_TargetDashRange()
{
    if (nullptr == m_pTarget)
        CRASH("Failed Target Search");

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 두 위치의 차이벡터 계산
    _vector vDirection = vTargetPos - vMyPos;

    // 거리 계산
    _float fDistance = XMVectorGetX(XMVector3Length(vDirection));

    return fDistance <= m_fDashMaxDistance && fDistance >= m_fDashMinDistance;
}
_bool CQueenKnight::Is_TargetDodgeRange()
{
    if (nullptr == m_pTarget)
        CRASH("Failed Target Search");

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 두 위치의 차이벡터 계산
    _vector vDirection = vTargetPos - vMyPos;

    // 거리 계산
    _float fDistance = XMVectorGetX(XMVector3Length(vDirection));

    return fDistance <= m_fDashDodgeDistance;
}
_bool CQueenKnight::Is_TargetDownStrikeRange()
{
    if (nullptr == m_pTarget)
        CRASH("Failed Target Search");

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 두 위치의 차이벡터 계산
    _vector vDirection = vTargetPos - vMyPos;

    // 거리 계산
    _float fDistance = XMVectorGetX(XMVector3Length(vDirection));

    return fDistance <= m_fDownStrikeDistance;
}
#pragma endregion

#pragma region 4. 몬스터는 자신의 애니메이션을 관리해야한다.
HRESULT CQueenKnight::InitializeAction_ToAnimationMap()
{
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

    m_Action_AnimMap.emplace(L"IDLE", AS_TStdKnight_TLanceGCS_Idle_N_Loop);
    //m_Action_AnimMap.emplace(L"IDLE", AS_TStdKnight_TLSword_Idle_N_Loop);
    m_Action_AnimMap.emplace(L"IDLE_L180", AS_TStdKnight_TShieldSword_Guard_IdleTurn_L180);
    m_Action_AnimMap.emplace(L"IDLE_L90", AS_TStdKnight_TShieldSword_Guard_IdleTurn_L90);
    m_Action_AnimMap.emplace(L"IDLE_R180", AS_TStdKnight_TShieldSword_Guard_IdleTurn_R180);
    m_Action_AnimMap.emplace(L"IDLE_R90", AS_TStdKnight_TShieldSword_Guard_IdleTurn_R90);
    m_Action_AnimMap.emplace(L"GUARDHIT", AS_TStdKnight_TShieldSword_GuardHit01_N);
    m_Action_AnimMap.emplace(L"DAMAGE", AS_TStdKnight_TCmn_Damage01_BR);
    m_Action_AnimMap.emplace(L"HIT", AS_TStdKnight_TCmn_Damage01_BR);


    m_Action_AnimMap.emplace(L"ATTACK", AS_TStdKnight_TLanceGCS_AttackNormal01_N);
    m_Action_AnimMap.emplace(L"ATTACK1", AS_TStdKnight_TLanceGCS_AttackNormal01_N);
    m_Action_AnimMap.emplace(L"ATTACK2", AS_TStdKnight_TLanceGCS_AttackNormal02_N);
    m_Action_AnimMap.emplace(L"ATTACK3", AS_TStdKnight_TLanceGCS_AttackNormal02_N_Combo);

    // 중간에 사라지게 해서 플레이어 위에서 나타나서 아래로 내다꼽게.
    m_Action_AnimMap.emplace(L"ATTACK_JUMP", AS_TStdKnight_TSword_AttackJump01_N);


    m_Action_AnimMap.emplace(L"RUN", AS_TStdKnight_TLanceGCS_Run_F_Loop);
    //m_Action_AnimMap.emplace(L"WALK", AS_TStdKnight_TShieldSword_Guard_Walk_F_Loop);
    m_Action_AnimMap.emplace(L"WALK_B", AS_TStdKnight_TLanceGCS_Guard_Walk_B_Loop);

    // 같은 애니메이션이지만 다른 이름으로 설정해서 Node에서 사용할 수 있게함.
    m_Action_AnimMap.emplace(L"DETECT", AS_TStdKnight_TLanceGCS_Run_F_Loop);
    m_Action_AnimMap.emplace(L"DOWN_START", AS_TStdKnight_TCmn_Down_P_Loop);
    m_Action_AnimMap.emplace(L"DOWN_END", AS_TStdKnight_TCmn_Down_P_End);
    m_Action_AnimMap.emplace(L"DEATH_NORMAL", AS_TStdKnight_TLanceGCS_Death_N);
    m_Action_AnimMap.emplace(L"DEATH", AS_TStdKnight_TLanceGCS_Death_N);
    
    /* 워프 애니메이션. */
    m_Action_AnimMap.emplace(L"WARP_START", AS_TStdKnight_TLanceGCS_AttackWarp_Start);
    m_Action_AnimMap.emplace(L"WARP_END", AS_TStdKnight_TLanceGCS_AttackWarp_End01);
    m_Action_AnimMap.emplace(L"WARP_SKILL", AS_TStdKnight_TLanceGCS_AttackWarp_End02);
    m_Action_AnimMap.emplace(L"WARP_JUMP_ATTACK", AS_TStdKnight_TLanceGCS_AttackWarpJump01_N);

    /* 점프 어택. (사라졌다가 나오는 모션.) */
    m_Action_AnimMap.emplace(L"DISAPPEAR_ATTACK", AS_TStdKnight_TSword_AttackJump01_N);
   
    
#pragma region 조우 애니메이션
    // 조우 했을때
    m_Action_AnimMap.emplace(L"ENCOUNTER", AS_TStdKnight_TCmn_NoneFightIdleCrouch_N);
    // 조우 이전.
    m_Action_AnimMap.emplace(L"PREV_ENCOUNTER", AS_TStdKnight_TCmn_NoneFightIdleCrouch_N_Loop);

#pragma endregion



#pragma region 특수 공격

    /* 연속 3번 공격. => 방패 위치가 반대임 바꿔야댐47 */
    //m_Action_AnimMap.emplace(L"PHASE_ATTACK1", AS_TStdKnight_TShieldSword_AttackShield02B_N);
    //m_Action_AnimMap.emplace(L"PHASE_ATTACK2", AS_TStdKnight_TShieldSword_AttackShield02C_N);
    //m_Action_AnimMap.emplace(L"PHASE_ATTACK3", AS_TStdKnight_TShieldSword_AttackShield02A_N);
    m_Action_AnimMap.emplace(L"PHASE_ATTACK1", AS_TStdKnight_TLanceGCS_AttackNormal01_N);
    m_Action_AnimMap.emplace(L"PHASE_ATTACK2", AS_TStdKnight_TLanceGCS_AttackNormal02_N);

    /* 돌진 애니메이션. */
    //m_Action_AnimMap.emplace(L"DODGE_B", AS_TStdKnight_TCmn_Dodge_B);
    m_Action_AnimMap.emplace(L"DODGE_B", AS_TStdKnight_TLanceGCS_AttackBackStep01_N);
    m_Action_AnimMap.emplace(L"DASH_ATTACK_START", AS_TStdKnight_TSword_AttackSpecial03_N_Start);
    m_Action_AnimMap.emplace(L"DASH_ATTACK_LOOP", AS_TStdKnight_TSword_AttackSpecial03_N_Loop);
    m_Action_AnimMap.emplace(L"DASH_ATTACK_END", AS_TStdKnight_TSword_AttackSpecial03_N_End);

    /* 삼연 내려찍기 패턴 */
    m_Action_AnimMap.emplace(L"DOWN_STRIKE", AS_TStdKnight_TSword_AttackJump01_N);
    m_Action_AnimMap.emplace(L"DOWN_STRIKE_SKILL", AS_TStdKnight_TSword_AttackRange01_N);
#pragma endregion


    
#pragma region 재생 속도 증가.

    /* 삼연 내려찍기 */
    /* Down Strike 시 애니메이션 별 재생 구간이 다름. => Node에서 제어. */
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DOWN_STRIKE"], 1.8f);
    
    // 250frame.
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DOWN_STRIKE_SKILL"], 1.5f);


    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"WARP_START"], 1.8f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"WARP_AEND"], 1.6f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"WARP_SKILL"], 1.8f);


    /* Phase Attack 1 ~ 3 */
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK1"], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK2"], 2.5f);
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK3"], 2.5f);

    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DODGE_B"], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DASH_ATTACK_START"], 2.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DASH_ATTACK_END"], 4.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"IDLE_L180"], 10.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"IDLE_L90"], 10.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"IDLE_R180"], 10.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"IDLE_R90"], 10.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"IDLE"], 10.f);
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"ATTACK"], 1.5f);
#pragma endregion




#pragma region COllider 활성화 프레임 관리
    // 100 ~ 136 Frame 활성화
    Add_Collider_Frame(m_Action_AnimMap[TEXT("DASH_ATTACK_START")], 100.f / 136.f, 136.f / 136.f, PART_WEAPON);     // Dash Attack
    Add_Collider_Frame(m_Action_AnimMap[TEXT("DASH_ATTACK_END")], 0.f / 130.f, 120.f / 130.f, PART_WEAPON);     // Dash Attack


    Add_Collider_Frame(m_Action_AnimMap[TEXT("WARP_END")], 20.f / 137.f, 40.f / 137.f, PART_WEAPON);     // Dash Attack
    // 공격 프레임 60 ~ 100프레임.1
    Add_Collider_Frame(m_Action_AnimMap[TEXT("DOWN_STRIKE")], 60.f / 224.f, 85.f / 224.f, PART_WEAPON);     // Dash Attack
    
    Add_Collider_Frame(m_Action_AnimMap[TEXT("ATTACK")], 54.f / 194.f, 75.f / 194.f, PART_WEAPON);       // Weapon attack

    Add_Collider_Frame(m_Action_AnimMap[TEXT("PHASE_ATTACK1")], 54.f / 194.f, 75.f / 194.f, PART_WEAPON);// Weapon attack
    Add_Collider_Frame(m_Action_AnimMap[TEXT("PHASE_ATTACK2")], 40.f / 241.f, 60.f / 241.f, PART_WEAPON);// Weapon attack
#pragma endregion

#pragma region TRAIL 활성화 프레임 관리.
    Add_Trail_Frame(m_Action_AnimMap[TEXT("WARP_END")], 17.f / 136.f, 50.f / 136.f, PART_WEAPON);     // Dash Attack
    Add_Trail_Frame(m_Action_AnimMap[TEXT("DODGE_B")],10.f / 136.f, 65.f / 136.f, PART_WEAPON);     // Dash Attack


    Add_Trail_Frame(m_Action_AnimMap[TEXT("PHASE_ATTACK1")], 20.f / 194.f, 80.f / 194.f, PART_WEAPON);// Weapon attack
    Add_Trail_Frame(m_Action_AnimMap[TEXT("PHASE_ATTACK2")], 20.f / 241.f, 60.f / 241.f, PART_WEAPON);// Weapon attack

    Add_Trail_Frame(m_Action_AnimMap[TEXT("ATTACK")], 30.f / 195.f, 70.f / 195.f, PART_WEAPON);       // Weapon attack

    Add_Trail_Frame(m_Action_AnimMap[TEXT("ENCOUNTER")], 80.f / 180.f, 130.f / 180.f, PART_WEAPON);// Weapon attack
#pragma endregion


    return S_OK;
}
#pragma endregion


#pragma region 5. 사용되는 버프 / 디버프에 대한 타이머를 관리해야합니다.
HRESULT CQueenKnight::Initialize_BuffDurations()
{
    // 기본 쿨다운.
    m_BuffDefault_Durations[BUFF_HIT] = 0.3f;        // 피격: 0.6초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_CORPSE] = 2.0f;       // 시체 : 2.0초
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 0.3f; // 무적 시간.
    m_BuffDefault_Durations[BUFF_DEAD] = 10.f; // 사망 시간.
    m_BuffDefault_Durations[BUFF_DETECT] = 1.f; // 탐지 쿨타임: 0.5초
    m_BuffDefault_Durations[BUFF_ATTACK_TIME] = 0.5f; // 탐지 쿨타임: 0.5초
    m_BuffDefault_Durations[BUFF_NAVIGATION_OFF] = 0.2f; // 왠만하면 기본 쿨타임이아니라 지정 쿨타임.
    
    
    // 10 초마다 해당 페이즈 시퀀스 공격 반복
    m_BuffDefault_Durations[QUEEN_BUFF_PHASE_ATTACK_COOLDOWN] = 10.f;

    // Dissovle 타임.
    m_BuffDefault_Durations[BUFF_DISSOLVE] = 1.f;
    m_BuffDefault_Durations[BUFF_REVERSEDISSOLVE] = 0.5f;
    // 무기의 Dissolve 타임도 지정.
    m_pWeapon->Set_DissolveTime(1.f);
    m_pWeapon->Set_ReverseDissolveTime(0.3f);
    m_pShield->Set_DissolveTime(1.f);
    m_pShield->Set_ReverseDissolveTime(0.3f);


    //m_BuffDefault_Durations[QUEEN_BUFF_PHASE_ATTACK_COOLDOWN] = 999.f;

    // 20초마다 돌진 공격 시퀀스 반복.
    //m_BuffDefault_Durations[QUEEN_BUFF_DASH_ATTACK_COOLDOWN] = 999.f; // 돌진 공격 쿨타임.
    m_BuffDefault_Durations[QUEEN_BUFF_DASH_ATTACK_COOLDOWN] = 20.f; // 돌진 공격 쿨타임.

    // 25초마다 내려찍기 공격 시퀀스 반복.
    m_BuffDefault_Durations[QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN] = 30.f; // 세번 연속 내려찍기 공격 쿨타임.

    return S_OK;
}

#pragma endregion

#pragma region 6. 특수한 상태를 제어하기 위한 함수입니다.
void CQueenKnight::Enable_Collider(_uint iType)
{
    /* PART_WEAPON이면 WEAPON Colider Enable */
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Activate_Collider();
        break;
    case PART_SHIELD:
        m_pShield->Activate_Collider();
        break;
    case PART_BODY:
        m_pColliderCom->Set_Active(true);
        break;
    default:
        break;
    }
}

void CQueenKnight::Disable_Collider(_uint iType)
{
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Deactivate_Collider();
        break;
    case PART_SHIELD:
        m_pShield->Deactivate_Collider();
        break;
    case PART_BODY:
        m_pColliderCom->Set_Active(false);
        break;
    default:
        break;
    }
}

void CQueenKnight::Weapon_Rotation(_uint iPartType, _float3 vRadians, _bool bInverse)
{

    if (bInverse)
    {
        switch (iPartType)
        {
        case PART_WEAPON:
            m_pWeapon->Get_Transform()->Add_Inverse_Rotation(vRadians.x, vRadians.y, vRadians.z);
            break;
        case PART_SHIELD:
            m_pShield->Get_Transform()->Add_Inverse_Rotation(vRadians.x, vRadians.y, vRadians.z);
            break;
        default:
            break;
        }
        return;
    }


    switch (iPartType)
    {
    case PART_WEAPON:
        m_pWeapon->Get_Transform()->Add_Rotation(vRadians.x, vRadians.y, vRadians.z);
        break;
    case PART_SHIELD:
        m_pShield->Get_Transform()->Add_Rotation(vRadians.x, vRadians.y, vRadians.z);
        break;
    default:
        break;
    }
}

void CQueenKnight::Encounter_Action()
{
    Weapon_Rotation(PART_TYPE::PART_SHIELD
        , { XMConvertToRadians(180.f), XMConvertToRadians(0.f), XMConvertToRadians(-90.f)}, true);
}

#pragma endregion

#pragma region 7. 보스몹 UI 관리
void CQueenKnight::Take_Damage(_float fDamage, CGameObject* pGameObject)
{
    CMonster::Take_Damage(fDamage, pGameObject);
    Decrease_HpUI(fDamage, 0.1f);
}
void CQueenKnight::Increase_HpUI(_float fHp, _float fTime)
{
    m_pBossHpBarUI->Increase_Hp(fHp, fTime);
}

void CQueenKnight::Decrease_HpUI(_float fHp, _float fTime)
{
    m_pBossHpBarUI->Decrease_Hp(fHp, fTime);
}

HRESULT CQueenKnight::Initailize_UI()
{
    // 위에는 보스몹

    CBossHpBarUI::BOSS_HPBAR_DESC Desc{};
    // 정중앙 위치
    Desc.fX = g_iWinSizeX * 0.5f;
    Desc.fY = 100.f;
    Desc.fSizeX = 1200.f;
    Desc.fSizeY = 40.f;
    Desc.fMaxHp = m_MonsterStat.fMaxHP;
    Desc.strName = TEXT("여왕의 기사");
    Desc.eShaderPath = POSTEX_SHADERPATH::HPPROGRESSBAR;

    CUIObject* pUIObject = nullptr;

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_BossHPBar"), &Desc));

    m_pBossHpBarUI = dynamic_cast<CBossHpBarUI*>(pUIObject);
    if (nullptr == m_pBossHpBarUI)
    {
        CRASH("Failed Clone BossHPBar UI");
        return E_FAIL;
    }

    return S_OK;
}

void CQueenKnight::Set_Visible(_bool bVisible)
{
    m_bVisible = bVisible;

    m_pWeapon->Set_Visible(m_bVisible);
    m_pShield->Set_Visible(m_bVisible);
}


#pragma endregion

#pragma region 9. Effect 객체 제어
void CQueenKnight::Create_QueenKnightWarp_Effect_Particle(_float3 vDir)
{

    CEffectParticle::EFFECTPARTICLE_ENTER_DESC Desc{};
    Desc.eParticleType = CEffectParticle::PARTICLE_TYPE::PARTICLE_TYPE_QUEEN_WARP;
    Desc.vStartPos = m_pTransformCom->Get_State(STATE::POSITION); // 몬스터 현재위치로 생성.
    Desc.particleInitInfo.lifeTime = 5.f;
    Desc.particleInitInfo.dir = vDir;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.fChaseTime = m_pGameInstance->Get_TimeDelta() * 30.f;
    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("QUEENKNIGHT_WARP"), TEXT("Layer_Effect"), 1, ENUM_CLASS(EFFECTTYPE::PARTICLE), &Desc);

}

void CQueenKnight::Create_QueenKnightWarp_Effect_Particle_Spawn(_float3 vDir, _uint iSpawnCount)
{
    CEffectParticle::EFFECTPARTICLE_ENTER_DESC Desc{};
    Desc.vStartPos = m_pTransformCom->Get_State(STATE::POSITION); // 몬스터 현재위치로 생성.
    Desc.particleInitInfo.lifeTime = 5.f;
    Desc.particleInitInfo.dir = vDir;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.fSpawnInterval = m_pGameInstance->Get_TimeDelta() * 20.f;
    Desc.iSpawnCount = iSpawnCount;
    Desc.IsSpawn = true;
    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("QUEENKNIGHT_WARP"), TEXT("Layer_Effect"), 1, ENUM_CLASS(EFFECTTYPE::PARTICLE), &Desc);


}

void CQueenKnight::Create_QueenKnightWarp_Effect_Particle_Explosion(_float3 vDir)
{
    CEffectParticle::EFFECTPARTICLE_ENTER_DESC Desc{};
    Desc.vStartPos = m_pTransformCom->Get_State(STATE::POSITION); // 몬스터 현재위치로 생성.
    Desc.particleInitInfo.lifeTime = 5.f;
    Desc.particleInitInfo.dir = vDir;
    Desc.particleInitInfo.fExplositionTime = 1.f;
    Desc.particleInitInfo.fRadius = 5.f;
    
    Desc.pTargetTransform = m_pTransformCom;
    Desc.fSpawnInterval = m_pGameInstance->Get_TimeDelta() * 20.f;
    Desc.IsSpawn = true;
    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("QUEENKNIGHT_EXPLOSION"), TEXT("Layer_Effect"), 1, ENUM_CLASS(EFFECTTYPE::PARTICLE), &Desc);
}

#pragma region 새로 만드는 파티클 객체
void CQueenKnight::Create_QueenKnightWarp_Effect(_float3 vDir)
{
    CEffectParticle::EFFECTPARTICLE_ENTER_DESC Desc{};
    Desc.vStartPos = m_pTransformCom->Get_State(STATE::POSITION); // 몬스터 현재위치로 생성.
    Desc.particleInitInfo.lifeTime = 5.f;
    Desc.particleInitInfo.dir = vDir;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.fChaseTime = 5.f; // 50 프레임만 추적
    m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("QUEENKNIGHT_PARTICLE"), TEXT("Layer_Effect"), 1, ENUM_CLASS(EFFECTTYPE::PARTICLE), &Desc);
}

// Pillar 시작 시.
void CQueenKnight::Start_PillarSkill()
{
    m_bIsSkillActive = true;
    m_fSkillElapsedTime = 0.f; // 타이머 리셋
    XMStoreFloat3(&m_vSkillCenterPos, m_pTransformCom->Get_State(STATE::POSITION)); // 스킬 중심 위치 저장
    std::fill(m_vecIsPillarActivated.begin(), m_vecIsPillarActivated.end(), false);
}

void CQueenKnight::Update_BloodPillar(_float fTimeDelta)
{
    //if (!m_bIsSkillActive)
    //    return;

    //m_fSkillElapsedTime += fTimeDelta;

    //for (_uint i = 0; i < m_vecPillarPositions.size(); ++i)
    //{
    //    // 이미 소환된 Pillar는 건너뜀
    //    if (m_vecIsPillarActivated[i])
    //        continue;

    //    // 이 Pillar가 소환되어야 하는 시간을 계산
    //    _vector vPillarDir = XMLoadFloat3(&m_vecPillarPositions[i]) - XMLoadFloat3(&m_vSkillCenterPos);
    //    _float fDistance = XMVectorGetX(XMVector3Length(vPillarDir));
    //    _float fRequiredTime = fDistance / m_fRippleSpeed;

    //    // 스킬 경과 시간이 소환 필요 시간을 지났다면
    //    if (m_fSkillElapsedTime >= fRequiredTime)
    //    {
    //        // 드디어 풀에서 Pillar를 꺼내와 활성화!
    //        CEffect_Pillar::PILLAR_ACTIVATE_DESC EffectPillarDesc{};
    //        EffectPillarDesc.eCurLevel = m_eCurLevel;
    //        EffectPillarDesc.vStartPos = XMLoadFloat3(&m_vecPillarPositions[i]); // 계산된 위치를 넣어줌
    //        EffectPillarDesc.fDuration = 2.2f;
    //        EffectPillarDesc.fAttackPower = static_cast<_float>(m_pGameInstance->Rand_UnsignedInt(150, 200));
    //        // ... 나머지 Desc 내용 채우기 ...

    //        m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
    //            , TEXT("BLOOD_PILLAR"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CEffect_Pillar::EffectType), &EffectPillarDesc);

    //        // 소환되었다고 표시
    //        m_vecIsPillarActivated[i] = true;
    //    }
    //}

    if (!m_bIsSkillActive)
        return;

    m_fSkillElapsedTime += fTimeDelta;

    for (_uint i = 0; i < m_vecPillarPositions.size(); ++i)
    {
        if (m_vecIsPillarActivated[i])
            continue;

        // [수정] 시간 계산 로직 변경
        _vector vPillarLocalPos = XMLoadFloat3(&m_vecPillarPositions[i]);
        _float fDistance = XMVectorGetX(XMVector3Length(vPillarLocalPos));

        // 거리를 0.0 ~ 1.0 사이 값으로 정규화
        _float fNormalizedDistance = fDistance / m_fMaxPillarDistance;

        // Ease-Out 효과 (처음엔 빠르고 나중엔 느려짐)
        // fNormalizedDistance를 제곱하여 곡선적인 시간 변화를 줌
        _float fEasedTime = fNormalizedDistance * fNormalizedDistance;
        _float fRequiredTime = fEasedTime * m_fMaxSkillDuration;

        if (m_fSkillElapsedTime >= fRequiredTime)
        {
            // 드디어 풀에서 Pillar를 꺼내와 활성화!
            CEffect_Pillar::PILLAR_ACTIVATE_DESC EffectPillarDesc{};
            EffectPillarDesc.eCurLevel = m_eCurLevel;
            EffectPillarDesc.vStartPos = XMLoadFloat3(&m_vecPillarPositions[i]); // 계산된 위치를 넣어줌
            EffectPillarDesc.fDuration = m_fMaxSkillDuration;
            EffectPillarDesc.fAttackPower = static_cast<_float>(m_pGameInstance->Rand_UnsignedInt(150, 200));
            // ... 나머지 Desc 내용 채우기 ...

            m_pGameInstance->Move_Effect_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
                , TEXT("BLOOD_PILLAR"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CEffect_Pillar::EffectType), &EffectPillarDesc);

            // 소환되었다고 표시
            m_vecIsPillarActivated[i] = true;
        }
    }

    // [추가] 스킬 종료 처리
    if (m_fSkillElapsedTime >= m_fMaxSkillDuration)
    {
        m_bIsSkillActive = false;
    }
}
void CQueenKnight::Reset_PillarSkill()
{
}
#pragma endregion



// Dissolve 진행.
void CQueenKnight::Start_Dissolve(_float fDuration)
{
    if(fDuration == 0.f)
        AddBuff(BUFF_DISSOLVE);

    AddBuff(BUFF_DISSOLVE, fDuration);
    m_fMaxDissolveTime = Get_BuffTime(BUFF_DISSOLVE);
    m_fCurDissolveTime = 0.f;
    //m_fCurDissolveTime = m_fMaxDissovleTime;
    
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DISSOLVE);
    m_pWeapon->Start_Dissolve(fDuration);
    m_pShield->Start_Dissolve(fDuration);
}


void CQueenKnight::ReverseStart_Dissolve(_float fDuration)
{
    if (fDuration == 0.f)
        AddBuff(BUFF_REVERSEDISSOLVE);

    AddBuff(BUFF_REVERSEDISSOLVE, fDuration);
    m_fMaxDissolveTime = Get_DefaultBuffTime(BUFF_DISSOLVE);
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DISSOLVE);
    m_pWeapon->ReverseStart_Dissolve(fDuration);
    m_pShield->ReverseStart_Dissolve(fDuration);
}

void CQueenKnight::End_Dissolve()
{
    RemoveBuff(BUFF_DISSOLVE);
    RemoveBuff(BUFF_REVERSEDISSOLVE);  // 이 라인 추가 필요
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DEFAULT);
    m_pWeapon->End_Dissolve();
    m_pShield->End_Dissolve();
}

void CQueenKnight::Enable_Trail(_uint iPartType)
{

    switch (iPartType)
    {
    case PART_WEAPON:
        m_pWeapon->Set_Trail(true);
        break;
    default:
        break;
    }
}

void CQueenKnight::Disable_Trail(_uint iPartType)
{

    switch (iPartType)
    {
    case PART_WEAPON:
        m_pWeapon->Set_Trail(false);
        break;
    default:
        break;
    }
}


#pragma endregion




#pragma region 0. 기본 함수들 정의 => 콜라이더도 정의
HRESULT CQueenKnight::Ready_Components(QUEENKNIGHT_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CMonster::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_QueenKnight")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

   
    // Dissolve Texture
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve"),
        TEXT("Com_Dissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTexture), nullptr)))
    {
        CRASH("Failed Load DissolveTexture");
        return E_FAIL;
    }
    m_fEndReverseDissolveTime = 2.f;


    return S_OK;
}

HRESULT CQueenKnight::Ready_Colliders(QUEENKNIGHT_DESC* pDesc)
{
    // 오프셋 지정.

    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = max(max(box.vExtents.x, box.vExtents.y), box.vExtents.z);
    SphereDesc.vCenter = _float3(0.f, box.vExtents.y, 0.f); // 중점.

    SphereDesc.pOwner = this;
    SphereDesc.eCollisionType = CCollider::COLLISION_BODY;
    SphereDesc.eMyLayer = CCollider::MONSTER;
    SphereDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON
        | CCollider::MONSTER | CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;


    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_SPHERE")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
    {
        CRASH("Failed Clone Collider SPHERE");
        return E_FAIL;
    }

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));


    return S_OK;
}

HRESULT CQueenKnight::Ready_Navigations()
{
    CNavigation::NAVIGATION_DESC        NaviDesc{};
    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
    {
        CRASH("Failed Clone Navigation");
        return E_FAIL;
    }

    _float3 vPos = { };
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    _float3 vFinalPos = {};
    _int iNearCell = m_pNavigationCom->Find_NearCellIndex(vPos);

    if (iNearCell == -1)
    {
        CRASH("Failed Search Navigation Cell");
        return E_FAIL;
    }

    m_pNavigationCom->Set_CurrentCellIndex(iNearCell);
    XMStoreFloat3(&vFinalPos, m_pNavigationCom->Get_CellPos(iNearCell));
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vFinalPos));


    return S_OK;
}

HRESULT CQueenKnight::Ready_BehaviorTree()
{
    CQueenKnightTree::QUEEN_KNIGHT_BT_DESC BT{};
    BT.pOwner = this;

    // Tree에 대한 초기화는 트리 내부에서하기.
    m_pTree = CQueenKnightTree::Create(m_pDevice, m_pContext, &BT);

    // Phase 설정
    AddBuff(QUEEN_BUFF_PHASE_FIRST);

    return S_OK;
}

HRESULT CQueenKnight::Ready_PartObjects()
{
    CKnightLance::KNIGHT_LANCE_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("RightHandAttachSocket");
    Weapon.pOwner = this;
    Weapon.eCurLevel = m_eCurLevel;
    Weapon.fAttackPower = m_MonsterStat.fAttackPower;
    Weapon.eShaderPath = MESH_SHADERPATH::DEFAULT;


    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_GodChildLance")
        , reinterpret_cast<CPartObject**>(&m_pWeapon), &Weapon)))
    {
        CRASH("Failed Create Queen Weapon");
        return E_FAIL;
    }

    CKnightShield::KNIGHT_SHIELD_DESC Shield{};
    Shield.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Shield.pSocketMatrix = m_pModelCom->Get_BoneMatrix("LeftHandAttachSocket");
    Shield.eCurLevel = m_eCurLevel;
    Shield.pOwner = this;
    Shield.eShaderPath = MESH_SHADERPATH::DEFAULT;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Shield"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_GodChildShield")
        , reinterpret_cast<CPartObject**>(&m_pShield), &Shield)))
    {
        CRASH("Failed Create Queen Shield");
        return E_FAIL;
    }

    return S_OK;
}

#pragma region Effect 생성관련 정보 정의해 두기.
HRESULT CQueenKnight::Ready_Effects(QUEENKNIGHT_DESC* pDesc)
{
    /* 스킬 생성 정보*/
    m_vecPillarPositions.clear();
    m_vecIsPillarActivated.clear();

    const _uint iNumArms = 4;                // 십자 모양의 팔 개수 (상하좌우 = 4)
    const _uint iNumPillarsPerArm = 4;       // 각 팔(선)에 생성될 Pillar 개수
    const _float fSpacing = 7.f;             // Pillar 사이의 간격 (미터 단위)
    const _uint iTotalPillars = iNumArms * iNumPillarsPerArm;

    m_vecPillarPositions.reserve(iTotalPillars);
    m_vecIsPillarActivated.resize(iTotalPillars);

    for (_uint i = 0; i < iNumArms; ++i)
    {
        _float3 vDirection = { 0.f, 0.f, 0.f };

        switch (i)
        {

        case 0: vDirection = { 0.f, 0.f, 1.f };  break; // 위
        case 1: vDirection = { 1.f, 0.f, 0.f };  break; // 오른쪽
        case 2: vDirection = { 0.f, 0.f, -1.f }; break; // 아래
        case 3: vDirection = { -1.f, 0.f, 0.f }; break; // 왼쪽
        }

        _float3 vPos = {};
        for (_uint j = 1; j <= iNumPillarsPerArm; ++j)
        {
            _float fDistance = fSpacing * j;
            
            XMStoreFloat3(&vPos, XMLoadFloat3(&vDirection) * fDistance); // 방향 벡터 * 거리 = 최종 위치
            vPos.y += 2.f;
            

            m_vecPillarPositions.push_back(vPos);
        }
    }

    m_fMaxPillarDistance = 0.f;

    // 추가
    for (const auto& pos : m_vecPillarPositions)
    {
        _float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pos)));
        if (fDist > m_fMaxPillarDistance)
            m_fMaxPillarDistance = fDist;
    }

    return S_OK;
}
#pragma endregion



HRESULT CQueenKnight::Bind_Shader_Resource()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pDissolveTexture->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 2)))
    {
        CRASH("Failed Dissolve Texture");
        return E_FAIL;
    }
    
    _float fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxDissolveTime);
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
    {
        CRASH("Failed Dissolve Texture");
        return E_FAIL;
    }


    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
    //{
    //    CRASH("Failed Bind CamPosition");
    //    return E_FAIL;
    //}
        
    return S_OK;
}





CQueenKnight* CQueenKnight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQueenKnight* pInstance = new CQueenKnight(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CQueenKnight"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CQueenKnight::Clone(void* pArg)
{
    CQueenKnight* pInstance = new CQueenKnight(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CQueenKnight"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CQueenKnight::Destroy()
{
    CMonster::Destroy();
}

void CQueenKnight::Free()
{
    CMonster::Free();
    Safe_Release(m_pBossHpBarUI);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pShield);
    Safe_Release(m_pTree);
    Safe_Release(m_pDissolveTexture);
}


#pragma endregion

#ifdef _DEBUG
void CQueenKnight::ImGui_Render()
{

    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window

    ImVec2 windowSize = ImVec2(300.f, 300.f);
    ImVec2 windowPos = ImVec2(windowSize.x, 0.f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = "QueenKnight Debug";
    ImGui::Begin(strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("HP : (%.2f)", m_MonsterStat.fHP);
    ImGui::Text("MAX HP : (%.2f)", m_MonsterStat.fMaxHP);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("POS : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    _vector vMyPos =  m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vDistance = vTargetPos - vMyPos;
    _float fDistance = XMVectorGetX(XMVector3Length(vDistance));
    ImGui::Text("Target Distance : (%.2f)", fDistance);

    ImGui::Separator();

    _uint iIndex = m_pModelCom->Get_CurrentAnimationIndex();
    
    ImGui::Text("Current Animation Index : %d ", iIndex);


    _wstring strAnimIndex = TEXT("Current Animation Index : ") + to_wstring(iIndex) + TEXT("\n");
    OutputDebugWstring(strAnimIndex);

    _float fCurrentAnimSpeed = m_pModelCom->Get_AnimSpeed(iIndex);
    ImGui::InputFloat("Anim Speed", &fCurrentAnimSpeed);
    ImGui::Separator();

    if (ImGui::Button("Apply"))
    {
        m_pModelCom->Set_AnimSpeed(iIndex, fCurrentAnimSpeed);
    }

    ImGui::End();
}
#endif // _DEBUG