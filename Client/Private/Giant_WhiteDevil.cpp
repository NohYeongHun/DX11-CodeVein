CGiant_WhiteDevil::CGiant_WhiteDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CGiant_WhiteDevil::CGiant_WhiteDevil(const CGiant_WhiteDevil& Prototype)
    : CMonster(Prototype)
{

}

#pragma region 0. 기본 함수들 정의
HRESULT CGiant_WhiteDevil::Initialize_Prototype()
{
    if (FAILED(CMonster::Initialize_Prototype()))
    {
        CRASH("Failed Ready Prototype Failed");
        return E_FAIL;
    }

    m_strObjTag = TEXT("GiantWhite Devil");

    return S_OK;
}

HRESULT CGiant_WhiteDevil::Initialize_Clone(void* pArg)
{
    GIANTWHITEDEVIL_DESC* pDesc = static_cast<GIANTWHITEDEVIL_DESC*>(pArg);
    if (FAILED(CMonster::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Ready Clone Failed");
        return E_FAIL;
    }


    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components Failed");
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
        CRASH("Failed Ready BehaviourTree WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDuration WolfDevil");
        return E_FAIL;
    }

    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    _float3 vScale = { 1.2f, 1.2f, 1.2f };
    m_pTransformCom->Set_Scale(vScale);

    /* 현재 Object Manager에 담기 전에는 모든 Collider를 충돌 비교 하지 않습니다. */
    Collider_All_Active(false);

    m_pModelCom->Set_Animation(m_Action_AnimMap[TEXT("IDLE")], true); // 초기 애니메이션은 IDLE로 설정.    

    return S_OK;
}

void CGiant_WhiteDevil::Priority_Update(_float fTimeDelta)
{
    CMonster::Priority_Update(fTimeDelta);

    if (m_IsEncountered)
        m_pBossHpBarUI->Priority_Update(fTimeDelta);
    
        
}

void CGiant_WhiteDevil::Update(_float fTimeDelta)
{
    // 조우 이전에는 출력하지 않습니다.
    if (m_IsEncountered)
        m_pBossHpBarUI->Update(fTimeDelta);

    Update_AI(fTimeDelta);

    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    CMonster::Update(fTimeDelta);

    Finalize_Update(fTimeDelta);
}

void CGiant_WhiteDevil::Finalize_Update(_float fTimeDelta)
{
    CMonster::Finalize_Update(fTimeDelta);
}

void CGiant_WhiteDevil::Late_Update(_float fTimeDelta)
{
    if (m_IsEncountered)
        m_pBossHpBarUI->Late_Update(fTimeDelta);


    if (!HasBuff(CMonster::BUFF_NAVIGATION_OFF))
    {
        m_pTransformCom->Set_State(STATE::POSITION
            , m_pNavigationCom->Compute_OnCell(
                m_pTransformCom->Get_State(STATE::POSITION)));
    }

    // 보이는 상태일 때만 렌더 그룹에 추가
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
    {
        CRASH("Failed Render Giant White Devil");
        return;
    }
        

    CMonster::Late_Update(fTimeDelta);
}

HRESULT CGiant_WhiteDevil::Render()
{
#ifdef _DEBUG
    ImGui_Render();
#endif // _DEBUG

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


#pragma region 1. 충돌 함수들 정의
void CGiant_WhiteDevil::On_Collision_Enter(CGameObject* pOther)
{
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

void CGiant_WhiteDevil::On_Collision_Stay(CGameObject* pOther)
{

}

void CGiant_WhiteDevil::On_Collision_Exit(CGameObject* pOther)
{

}

void CGiant_WhiteDevil::Collider_All_Active(_bool bActive)
{
    m_pColliderCom->Set_Active(bActive);

    if (bActive)
    {
        m_pWeapon->Activate_Collider();
    }
    else
    {
        m_pWeapon->Deactivate_Collider();
    }
}

#pragma endregion


#pragma region 2. 몬스터 AI 관리
void CGiant_WhiteDevil::Update_AI(_float fTimeDelta)
{
    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    Tick_BuffTimers(fTimeDelta);

    /* 콜라이더 활성화 구간 확인 */
    CMonster::Handle_Collider_State();

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }
}
#pragma endregion


#pragma region 3. 몬스터는 자신에게 필요한 수치값들을 초기화해야 합니다.

HRESULT CGiant_WhiteDevil::Initialize_Stats()
{
    m_fEncounterDistance = 30.f;

    return S_OK;
}


#pragma endregion

#pragma region 4. 몬스터는 자신의 애니메이션을 관리해야 합니다.
HRESULT CGiant_WhiteDevil::InitializeAction_ToAnimationMap()
{
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

#pragma region 특수 상태 Down, Dead, Hit
    // Hit
    m_Action_AnimMap.emplace(L"HIT", AS_TGiantWhite_THalberd_Damage01_BR);
    m_Action_AnimMap.emplace(L"DAMAGE", AS_TGiantWhite_THalberd_Damage01_BR);

    // Death
    m_Action_AnimMap.emplace(L"DEATH_NORMAL", AS_TStdSoldier_TCmn_Death_N);

    // Down
    m_Action_AnimMap.emplace(L"DOWN_START_F", AS_TStdSoldier_TCmn_DamageStrike_S);
    m_Action_AnimMap.emplace(L"DOWN_LOOP_F", AS_TStdSoldier_TCmn_Down_S_Loop);
    m_Action_AnimMap.emplace(L"DOWN_END_F", AS_TStdSoldier_TCmn_Down_S_End);


#pragma endregion
    
#pragma region 공격
    m_Action_AnimMap.emplace(L"ATTACK", AS_TGiantWhite_THalberd_AttackNormal01_N);
#pragma endregion

#pragma region 콤보 공격
    m_Action_AnimMap.emplace(L"COMBO_ATTACK1", AS_TGiantWhite_THalberd_AttackNormal01_N);
    m_Action_AnimMap.emplace(L"COMBO_ATTACK2", AS_TGiantWhite_THalberd_AttackNormal03_N);
#pragma endregion

    
#pragma region 이동
    m_Action_AnimMap.emplace(L"RUN", AS_TGiantWhite_THalberd_Run_N_Loop);
    m_Action_AnimMap.emplace(L"WALK", AS_TGiantWhite_THalberd_Walk_F_Loop);
    m_Action_AnimMap.emplace(L"WALK_B", AS_TGiantWhite_THalberd_Walk_B_Loop);

    m_Action_AnimMap.emplace(L"DETECT", AS_TGiantWhite_THalberd_Run_N_Loop);
#pragma endregion

#pragma region 조우 애니메이션
    // 조우 했을때
    m_Action_AnimMap.emplace(L"ENCOUNTER", AS_TGiantWhite_TCmn_NoneFightIdleCrouch_N);
    // 조우 이전.
    m_Action_AnimMap.emplace(L"PREV_ENCOUNTER", AS_TGiantWhite_TCmn_NoneFightIdleCrouch_N_Loop);

#pragma endregion


#pragma region IDLE
    m_Action_AnimMap.emplace(L"IDLE", AS_TGiantWhite_THalberd_Idle_N_Loop);
#pragma endregion

#pragma region 속도 관리
    //m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"ENCOUNTER"], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"ATTACK"], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"COMBO_ATTACK2"], 1.5f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"WALK_B"], 2.f);
#pragma endregion



#pragma region 콜라이더 활성화 프레임 관리.
    Add_Collider_Frame(m_Action_AnimMap[TEXT("ATTACK")], 50.f / 220.f, 70.f / 220.f, PART_WEAPON);     
    Add_Collider_Frame(m_Action_AnimMap[TEXT("COMBO_ATTACK1")], 50.f / 220.f, 70.f / 220.f, PART_WEAPON);
    Add_Collider_Frame(m_Action_AnimMap[TEXT("COMBO_ATTACK2")], 25.f / 246.f, 70.f / 246.f, PART_WEAPON);
#pragma endregion


    return S_OK;
}
#pragma endregion



#pragma region 5. 사용되는 버프 / 디버프에 대한 타이머를 관리해야합니다.
HRESULT CGiant_WhiteDevil::Initialize_BuffDurations()
{
    m_BuffDefault_Durations[BUFF_HIT] = 0.3f;        // 피격: 0.6초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_CORPSE] = 2.0f;       // 시체 : 2.0초
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 0.3f; // 무적 시간.
    m_BuffDefault_Durations[BUFF_DEAD] = 10.f; // 사망 시간.
    m_BuffDefault_Durations[BUFF_DETECT] = 1.f; // 탐지 쿨타임: 0.5초
    m_BuffDefault_Durations[BUFF_ATTACK_TIME] = 0.5f; // 탐지 쿨타임: 0.5초
    m_BuffDefault_Durations[BUFF_NAVIGATION_OFF] = 0.2f; // 왠만하면 기본 쿨타임이아니라 지정 쿨타임.


    m_BuffDefault_Durations[GIANT_BUFF_COMBO_ATTACK] = 3.f; 


    return S_OK;
}
#pragma endregion


#pragma region 6. 특수한 상태를 제어하기 위한 함수입니다.
void CGiant_WhiteDevil::Enable_Collider(_uint iType)
{
    /* PART_WEAPON이면 WEAPON Colider Enable */
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Activate_Collider();
        break;
    case PART_BODY:
        m_pColliderCom->Set_Active(true);
        break;
    default:
        break;
    }
}

void CGiant_WhiteDevil::Disable_Collider(_uint iType)
{
    switch (iType)
    {
    case PART_WEAPON:
        m_pWeapon->Deactivate_Collider();
        break;
    case PART_BODY:
        m_pColliderCom->Set_Active(false);
        break;
    default:
        break;
    }
}
#pragma endregion



#pragma region 7. 보스몹 UI 관리.

void CGiant_WhiteDevil::Take_Damage(_float fDamage, CGameObject* pGameObject)
{
    CMonster::Take_Damage(fDamage, pGameObject);
    Decrease_HpUI(fDamage, 0.1f);
        
}

void CGiant_WhiteDevil::Increase_HpUI(_float fHp, _float fTime)
{
    m_pBossHpBarUI->Increase_Hp(fHp, fTime);
}

void CGiant_WhiteDevil::Decrease_HpUI(_float fHp, _float fTime)
{
    m_pBossHpBarUI->Decrease_Hp(fHp, fTime);
}

HRESULT CGiant_WhiteDevil::Initailize_UI()
{
    CBossHpBarUI::BOSS_HPBAR_DESC Desc{};
    // 정중앙 위치
    Desc.fX = g_iWinSizeX * 0.5f;
    Desc.fY = 100.f;
    Desc.fSizeX = 1200.f;
    Desc.fSizeY = 40.f;
    Desc.fMaxHp = m_MonsterStat.fMaxHP;
    Desc.strName = TEXT("백랑의 광전사.");


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
#pragma endregion



#pragma region 99. 기본 초기화 함수들 정의.
HRESULT CGiant_WhiteDevil::Ready_Components(GIANTWHITEDEVIL_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Ready Shader Component");
        return E_FAIL;
    }
        

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CMonster::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_Giant_WhiteDevil")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
    {
        CRASH("Failed Add Component Model");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CGiant_WhiteDevil::Ready_Colliders(GIANTWHITEDEVIL_DESC* pDesc)
{

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

HRESULT CGiant_WhiteDevil::Ready_Navigations()
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

HRESULT CGiant_WhiteDevil::Ready_BehaviorTree()
{
    CGiant_WhiteDevilTree::GIANT_WHITEDEVIL_BT_DESC BT{};
    BT.pOwner = this;

    m_pTree = CGiant_WhiteDevilTree::Create(m_pDevice, m_pContext, &BT);

    return S_OK;
}

HRESULT CGiant_WhiteDevil::Ready_PartObjects()
{
    CWhiteLargeHalberd::WHITE_LARGEHALBERD_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("socket_Attach_Arms_R");
    Weapon.pOwner = this;
    Weapon.eCurLevel = m_eCurLevel;
    Weapon.fAttackPower = m_MonsterStat.fAttackPower;
    Weapon.eShaderPath = MESH_SHADERPATH::DEFAULT;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_WhiteLargeHalberd")
        , reinterpret_cast<CPartObject**>(&m_pWeapon), &Weapon)))
    {
        CRASH("Failed Create Giant_WhiteDevil Weapon");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CGiant_WhiteDevil::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;


    return S_OK;
}

#pragma endregion



CGiant_WhiteDevil* CGiant_WhiteDevil::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGiant_WhiteDevil* pInstance = new CGiant_WhiteDevil(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiant_WhiteDevil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGiant_WhiteDevil::Clone(void* pArg)
{
    CGiant_WhiteDevil* pInstance = new CGiant_WhiteDevil(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiant_WhiteDevil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGiant_WhiteDevil::Destroy()
{
    CMonster::Destroy();
}

void CGiant_WhiteDevil::Free()
{
    CMonster::Free();
    Safe_Release(m_pBossHpBarUI);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pTree);
}
#ifdef _DEBUG
void CGiant_WhiteDevil::ImGui_Render()
{

    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window
    ImVec2 windowSize = ImVec2(300.f, 300.f);
    ImVec2 windowPos = ImVec2(io.DisplaySize.x - windowSize.x, windowSize.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = "GiantWhite Debug";
    ImGui::Begin(strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("HP : (%.2f)", m_MonsterStat.fHP);
    ImGui::Text("MAX HP : (%.2f)", m_MonsterStat.fMaxHP);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("POS : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vDistance = vTargetPos - vMyPos;
    _float fDistance = XMVectorGetX(XMVector3Length(vDistance));
    ImGui::Text("Target Distance : (%.2f)", fDistance);

    static _float PosArr[3] = { vPos.x, vPos.y, vPos.z };
    ImGui::InputFloat3("Position", PosArr);

    if (ImGui::Button("Apply"))
    {
        _float3 vResultPos = { PosArr[0], PosArr[1], PosArr[2] };
        _vector vPosition = XMLoadFloat3(&vResultPos);

        m_pTransformCom->Set_State(STATE::POSITION
            , m_pNavigationCom->Compute_OnCell(vPosition));
    }


    ImGui::End();
}
#endif // _DEBUG