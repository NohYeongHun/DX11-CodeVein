#include "SlaveVampire.h"
CSlaveVampire::CSlaveVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{pDevice, pContext}
{
}

CSlaveVampire::CSlaveVampire(const CSlaveVampire& Prototype)
    : CMonster(Prototype)
{
}

#pragma region 기본 함수들
HRESULT CSlaveVampire::Initialize_Prototype()
{
    if (FAILED(CMonster::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSlaveVampire::Initialize_Clone(void* pArg)
{

    SLAVE_VAMPIRE_DSEC* pDesc = static_cast<SLAVE_VAMPIRE_DSEC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(CMonster::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Initialize_Stats()))
    {
        CRASH("Init Stats Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_Navigations()))
    {
        CRASH("Ready Navigations Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_BehaviorTree()))
    {
        CRASH("Failed Ready BehaviourTree WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Initialize_Stats()))
    {
        CRASH("Failed InitializeStat SlaveVampire");
        return E_FAIL;
    }
    

    if (FAILED(InitializeAction_ToAnimationMap()))
    {
        CRASH("Failed InitializeAction SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
    {
        CRASH("Failed Ready Components SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDuration SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Initialize_ColliderFrames()))
    {
        CRASH("Failed Init ColliderFrames SlaveVampire");
        return E_FAIL;
    }

    if (FAILED(Initialize_UI()))
    {
        CRASH("Failed Initialize UI");
        return E_FAIL;
    }

    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);
  
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(false);
    m_pModelCom->Set_Animation(SLAVEVAMPIRE_LSWORD_IDLE_LOOP, true);  // 기본 애니메이션 설정


    /* 현재 Object Manager에 담기 전에는 모든 Collider를 충돌 비교 하지 않습니다. */
    Collider_All_Active(false);

    return S_OK;
}

void CSlaveVampire::Priority_Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pMonsterHpBar->Priority_Update(fTimeDelta);

    CMonster::Priority_Update(fTimeDelta);


}

void CSlaveVampire::Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pMonsterHpBar->Update(fTimeDelta);
    // 이 순서대로 AI 작업을 호출해라.
    Update_AI(fTimeDelta);
#ifdef _DEBUG
    //m_pModelCom->Play_Animation(fTimeDelta);
#endif // _DEBUG


    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    CMonster::Update(fTimeDelta);

    // 가장 마지막에 수행되어야 하는 업데이트. 함수들 => 콜라이더 추가 등등.
    Finalize_Update(fTimeDelta);
}

void CSlaveVampire::Finalize_Update(_float fTimeDelta)
{
    CMonster::Finalize_Update(fTimeDelta);
    
    // 콜라이더 상태 처리 (애니메이션 기반)
    
}

void CSlaveVampire::Late_Update(_float fTimeDelta)
{
 

    m_pTransformCom->Set_State(STATE::POSITION
        , m_pNavigationCom->Compute_OnCell(
            m_pTransformCom->Get_State(STATE::POSITION)));

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
        return;
#endif // _DEBUG
    CMonster::Late_Update(fTimeDelta);

    if (m_IsEncountered)
    {
        m_pMonsterHpBar->Calculate_Screen_Position(
            m_pTransformCom->Get_State(STATE::POSITION));
        m_pMonsterHpBar->Late_Update(fTimeDelta);
    }
        
}

HRESULT CSlaveVampire::Render()
{


    if (FAILED(Ready_Render_Resources()))
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


#pragma region 0.  몬스터는 충돌에 대한 상태제어를 할 수 있어야한다. => 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
void CSlaveVampire::On_Collision_Enter(CGameObject* pOther)
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
            m_pGameInstance->PlaySoundEffect(L"NormalAttack.wav", 0.3f);

            // 3. 무적 버프 추가.
            AddBuff(BUFF_INVINCIBLE);

            AddBuff(BUFF_HIT);

        }
    }


    if (m_MonsterStat.fHP <= 0.f)
        AddBuff(BUFF_DEAD);

}

void CSlaveVampire::On_Collision_Stay(CGameObject* pOther)
{
}

void CSlaveVampire::On_Collision_Exit(CGameObject* pOther)
{
}

void CSlaveVampire::Collider_All_Active(_bool bActive)
{
    m_pColliderCom->Set_Active(bActive);

    if (bActive)
        m_pWeapon->Activate_Collider();
    else 
        m_pWeapon->Deactivate_Collider();

}

#pragma endregion


#pragma region 1. AI 관리
/* Update 시 AI가 수행해야 하는 순서들을 함수로 표현. */
void CSlaveVampire::Update_AI(_float fTimeDelta)
{
    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    // 트리 이후에 상태 값에 대한 초기화를 담당하는 Tick_BuffTimer 실행
    Tick_BuffTimers(fTimeDelta);

    if (HasBuff(CMonster::BUFF_DISSOLVE))
        m_fCurDissolveTime = m_fMaxDissolveTime - Get_BuffTime(BUFF_DISSOLVE); // 점차 보내는 값이 증가.
    else if (HasBuff(CMonster::BUFF_REVERSEDISSOLVE))
        m_fCurDissolveTime = Get_BuffTime(BUFF_REVERSEDISSOLVE); // 점차 값이 감소.
    else
        m_fCurDissolveTime = 0.f; // 버프가 사라지면서 잔여값이 남아서 Texture Reverse Dissolve가 모두 동작하지 않음.


    /* 콜라이더 활성화 구간 확인 */
    CMonster::Handle_Collider_State();

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }

    if (HasBuff(CMonster::BUFF_DEAD))
        return;

    _float fCurrentRatio = m_pModelCom->Get_Current_Ratio();
    if (m_IsPlayWeaponSound && fCurrentRatio >= m_fPlayAttackSound)
    {
        m_IsPlayWeaponSound = false;
        m_pGameInstance->PlaySoundEffect(L"SlaveVampireAttack.wav", 0.2f);
    }

}
void CSlaveVampire::Start_Dissolve(_float fDuration)
{
    if (fDuration == 0.f)
        AddBuff(BUFF_DISSOLVE);

    AddBuff(BUFF_DISSOLVE, fDuration);
    m_fMaxDissolveTime = Get_BuffTime(BUFF_DISSOLVE);
    m_fCurDissolveTime = 0.f;
    //m_fCurDissolveTime = m_fMaxDissovleTime;

    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DISSOLVE);
    m_pWeapon->Start_Dissolve(fDuration);
}
void CSlaveVampire::ReverseStart_Dissolve(_float fDuration)
{
    if (fDuration == 0.f)
        AddBuff(BUFF_REVERSEDISSOLVE);

    AddBuff(BUFF_REVERSEDISSOLVE, fDuration);
    m_fMaxDissolveTime = Get_DefaultBuffTime(BUFF_DISSOLVE);
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DISSOLVE);
    m_pWeapon->ReverseStart_Dissolve(fDuration);
}
void CSlaveVampire::End_Dissolve()
{
    RemoveBuff(BUFF_DISSOLVE);
    RemoveBuff(BUFF_REVERSEDISSOLVE);
    m_iShaderPath = static_cast<_uint>(ANIMESH_SHADERPATH::DEFAULT); // 중요!
    m_pWeapon->End_Dissolve();
}
void CSlaveVampire::Dead_Action()
{
    CMonster::Dead_Action();
    Start_Dissolve(6.f);
}
void CSlaveVampire::Hit_Action()
{
    CMonster::Hit_Action();
    m_pGameInstance->PlaySoundEffect(L"Wolf_Hit.wav", 0.3f);
}
#pragma endregion


#pragma region 2. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
// 기본적으로 몬스터 생성시 필요한 STAT 값들을 제외하고 더 필요한 경우 정의
HRESULT CSlaveVampire::Initialize_Stats()
{
    m_fMinDetectionDistance = 4.f;
    m_fEncounterDistance = 16.f;
    return S_OK;
}
#pragma endregion

#pragma region 3. 몬스터는 내 애니메이션이 무엇인지 알아야한다.
/* STRING Index 형태로 관리.*/
HRESULT CSlaveVampire::InitializeAction_ToAnimationMap()
{
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

    m_Action_AnimMap.emplace(L"IDLE", SLAVEVAMPIRE_LSWORD_IDLE_LOOP);
    m_Action_AnimMap.emplace(L"IDLE_L180", SLAVEVAMPIRE_NONFIGHT_TURN_L180);
    m_Action_AnimMap.emplace(L"IDLE_L90", SLAVEVAMPIRE_NONFIGHT_TURN_L90);
    m_Action_AnimMap.emplace(L"IDLE_R180", SLAVEVAMPIRE_NONFIGHT_TURN_R180);
    m_Action_AnimMap.emplace(L"IDLE_R90", SLAVEVAMPIRE_NONFIGHT_TURN_R90);
    m_Action_AnimMap.emplace(L"HIT", SLAVEVAMPIRE_DAMAGE01_FR);
    m_Action_AnimMap.emplace(L"ATTACK", SLAVEVAMPIRE_LSWORD_NORMAL02);
    m_Action_AnimMap.emplace(L"DEATH_BACK", SLAVEVAMPIRE_DEATH_B);
    m_Action_AnimMap.emplace(L"DEATH_NORMAL", SLAVEVAMPIRE_DEATH_N);
    m_Action_AnimMap.emplace(L"WALK", SLAVEVAMPIRE_WALK_F_LOOP);
    m_Action_AnimMap.emplace(L"WALK_B", SLAVEVAMPIRE_WALK_B_LOOP);
    m_Action_AnimMap.emplace(L"RUN", SLAVEVAMPIRE_RUN_F_LOOP);
    // 같은 애니메이션이지만 다른 이름으로 설정해서 Node에서 사용할 수 있게함.
    m_Action_AnimMap.emplace(L"DETECT", SLAVEVAMPIRE_RUN_F_LOOP);
    
    /* 조우 애니메이션*/
    m_Action_AnimMap.emplace(L"ENCOUNTER", SLAVEVAMPIRE_NONFIGHT_THREAT_SIT_F);
    // 조우 이전.
    m_Action_AnimMap.emplace(L"PREV_ENCOUNTER", SLAVEVAMPIRE_NONFIGHT_IDLE_SIT_LOOP);
    /* 재생속도 증가. */ 
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DEATH_NORMAL"], 2.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"ATTACK"], 1.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"DETECT"], 1.5f);



    
    m_fPlayAttackSound = 68.f / 256.f;

#pragma region COllider 활성화 프레임 관리
    Add_Collider_Frame(m_Action_AnimMap[TEXT("ATTACK")], 70.f / 256.f, 85.f / 256.f, PART_WEAPON);     // Weapon attack
#pragma endregion

    return S_OK;
}

#pragma endregion


#pragma region 3. 몬스터는 자신이 어떤 버프를 소유할 수 있는지를 알아야 한다. => 그리고 그에 맞는 쿨타임도 알아야한다.(몬스터마다 달라질 수 있다.)
HRESULT CSlaveVampire::Initialize_BuffDurations()
{
    // 어찌보면 이건 그냥 쿨다운의 영역.
    m_BuffDefault_Durations[BUFF_HIT] = 0.3f;        // 피격: 0.6초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_DEAD] = 10.f;
    m_BuffDefault_Durations[BUFF_CORPSE] = 5.f;       // 시체 : 2.0초
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 0.3f; // 무적 시간.
    m_BuffDefault_Durations[BUFF_ATTACK_TIME] = 1.f; // 공격 쿨타임.
    m_BuffDefault_Durations[BUFF_DETECT] = 1.f; // 탐지 쿨타임: 0.2초


    m_BuffDefault_Durations[BUFF_DISSOLVE] = 1.f;
    m_BuffDefault_Durations[BUFF_REVERSEDISSOLVE] = 0.5f;
    // 무기의 Dissolve 타임도 지정.
    m_pWeapon->Set_DissolveTime(1.f);
    m_pWeapon->Set_ReverseDissolveTime(0.3f);

    return S_OK;
}




#pragma endregion


#pragma region 4. 특수한 상태를 제어하기 위한 함수들
void CSlaveVampire::Enable_Collider(_uint iType)
{
    /* PART_WEAPON이면 WEAPON Colider Enable */
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Activate_Collider();
        break;
    default:
        break;
    }
}

void CSlaveVampire::Disable_Collider(_uint iType)
{
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Deactivate_Collider();
        break;
    default:
        break;
    }
}
void CSlaveVampire::PlayHitSound()
{
    //m_pGameInstance->PlaySoundEffect(L"SlaveVampireHit.wav", 0.3f);
}

void CSlaveVampire::PlayWeaponSound()
{
    m_IsPlayWeaponSound = true;
}
#pragma endregion




#pragma region 5. 기본적인 WolfDevil이 생성되기 위한 컴포넌트 객체들.
HRESULT CSlaveVampire::Ready_Components(SLAVE_VAMPIRE_DSEC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CMonster::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_SlaveVampire")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    // 오프셋 지정.

    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc{};
    SphereDesc.fRadius = 1.f;
    SphereDesc.vCenter = _float3(0.f, 1.2f, 0.f); // 중점.
    SphereDesc.pOwner = this;
    SphereDesc.eCollisionType = CCollider::COLLISION_BODY;
    SphereDesc.eMyLayer = CCollider::MONSTER;
    SphereDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON
        | CCollider::MONSTER | CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;

    if (FAILED(CMonster::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_SPHERE")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
    {
        CRASH("Failed Clone Collider SPEHRE");
        return E_FAIL;
    }


    // Dissolve Texture
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve"),
        TEXT("Com_Dissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTexture), nullptr)))
    {
        CRASH("Failed Load DissolveTexture");
        return E_FAIL;
    }
    m_fEndReverseDissolveTime = 2.f;
   


    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));

    return S_OK;
}

HRESULT CSlaveVampire::Ready_Navigations()
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
    _float3 vFinalPos = {};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
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

HRESULT CSlaveVampire::Ready_BehaviorTree()
{
   
    CMonsterTree::MONSTER_BT_DESC BT{};
    BT.pOwner = this;

    // Tree에 대한 초기화는 트리 내부에서하기.
    m_pTree = CMonsterTree::Create(m_pDevice, m_pContext, &BT);


    return S_OK;
}

HRESULT CSlaveVampire::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _float fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxDissolveTime);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
    {
        CRASH("Failed Dissolve Time");
        return E_FAIL;
    }

    if (FAILED(m_pDissolveTexture->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 2)))
    {
        CRASH("Failed Dissolve Texture");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSlaveVampire::Ready_PartObjects()
{
    //Head_Jaw
    CSlaveVampireSword::SLAVEVAMPIRE_WEAPON_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("IKSocket_RightHandAttach");
    Weapon.pOwner = this;
    Weapon.eCurLevel = m_eCurLevel;
    Weapon.fAttackPower = m_MonsterStat.fAttackPower;
    Weapon.eShaderPath = MESH_SHADERPATH::DEFAULT;
    
    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_SlaveVampireSword")
        , reinterpret_cast<CPartObject**>(&m_pWeapon), &Weapon)))
    {
        CRASH("Failed Create SlaveVampire Weapon");
        return E_FAIL;
    }
    
    if (nullptr == m_pWeapon)
    {
        CRASH("Wolf Weapon is nullptr after creation");
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CSlaveVampire::Initialize_ColliderFrames()
{
    return S_OK;
}

void CSlaveVampire::Take_Damage(_float fDamage, CGameObject* pGameObject)
{
    CMonster::Take_Damage(fDamage, pGameObject);
    Decrease_HpUI(fDamage, 0.1f);
}

void CSlaveVampire::Increase_HpUI(_float fHp, _float fTime)
{
    m_pMonsterHpBar->Increase_Hp(fHp, fTime);
}

void CSlaveVampire::Decrease_HpUI(_float fHp, _float fTime)
{
    m_pMonsterHpBar->Decrease_HpUI(fHp, fTime);
}


#pragma region UI 
HRESULT CSlaveVampire::Initialize_UI()
{
    CMonsterHpBar::MONSTERHPUI_DESC Desc{};

    // 정중앙 위치
    _float fRadius = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(m_pColliderCom->Get_BoundingDesc())->fRadius;
    Desc.vOffset = { 0.f, fRadius * 2.f }; // x, y 오프셋 값.
    Desc.vScale = { 1.f, 1.f, 1.f }; // x, y, z 크기.
    Desc.eShaderPath = POSTEX_SHADERPATH::MONSTERHP_PROGRESSBAR;
    Desc.fSizeX = 150.f;
    Desc.fSizeY = 10.f;
    Desc.fMaxHp = m_MonsterStat.fMaxHP;


    m_pMonsterHpBar = static_cast<CMonsterHpBar*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::GAMEOBJECT
        , ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_GameObject_MonsterHPBar"), &Desc));

    if (!m_pMonsterHpBar)
    {
        MSG_BOX(TEXT("Failed to Create MonsterHPBar"));
        return E_FAIL;
    }

    return S_OK;
}
#pragma endregion



CSlaveVampire* CSlaveVampire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlaveVampire* pInstance = new CSlaveVampire(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSlaveVampire"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSlaveVampire::Clone(void* pArg)
{
    CSlaveVampire* pInstance = new CSlaveVampire(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSlaveVampire"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlaveVampire::Destroy()
{
    CMonster::Destroy();
}

void CSlaveVampire::Free()
{
    CMonster::Free();
    Safe_Release(m_pTree);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pMonsterHpBar);
    Safe_Release(m_pDissolveTexture);
}


#pragma endregion

