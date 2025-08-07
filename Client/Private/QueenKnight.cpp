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
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    m_strObjTag = TEXT("QueenKnight");

    return S_OK;
}

HRESULT CQueenKnight::Initialize_Clone(void* pArg)
{
    QUEENKNIGHT_DESC* pDesc = static_cast<QUEENKNIGHT_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pDesc)))
    {
        CRASH("Ready Clone Failed");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Ready Components Failed");
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

    _float3 vScale = { 1.5f, 1.5f, 1.5f };
    m_pTransformCom->Set_Scale(vScale);


    _float3 vPos = { 0.f, 0.f, 0.f };
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

    //m_pModelCom->Set_Animation(KNIGHT_SWORD_IDLE, true);

    return S_OK;
}

void CQueenKnight::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CQueenKnight::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyPress(DIK_3))
    {
        AddBuff(CMonster::BUFF_HIT);
    }
    if (m_pGameInstance->Get_KeyPress(DIK_5))
    {
        AddBuff(CMonster::BUFF_DOWN);
    }

    Update_AI(fTimeDelta);

    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    __super::Update(fTimeDelta);

    Finalize_Update(fTimeDelta);
}

void CQueenKnight::Finalize_Update(_float fTimeDelta)
{
    __super::Finalize_Update(fTimeDelta);
}

void CQueenKnight::Late_Update(_float fTimeDelta)
{
    //m_pTransformCom->Set_State(STATE::POSITION
    //    , m_pNavigationCom->Compute_OnCell(
    //        m_pTransformCom->Get_State(STATE::POSITION), m_fOffsetY));

    m_pTransformCom->Set_State(STATE::POSITION
        , m_pNavigationCom->Compute_OnCell(
            m_pTransformCom->Get_State(STATE::POSITION)));

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CQueenKnight::Render()
{
#ifdef _DEBUG
     m_pColliderCom->Render();
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

        if (FAILED(m_pShaderCom->Begin(0)))
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
}

void CQueenKnight::On_Collision_Stay(CGameObject* pOther)
{
}

void CQueenKnight::On_Collision_Exit(CGameObject* pOther)
{
}
#pragma endregion


#pragma region 2. 몬스터 AI 관리
void CQueenKnight::Update_AI(_float fTimeDelta)
{
    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    Tick_BuffTimers(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }
}
#pragma endregion



#pragma region 3. 몬스터는 자신에게 필요한 수치값들을 초기화해야 합니다.
HRESULT CQueenKnight::Initialize_Stats()
{
    m_fMinDetectionDistance = 6.f;
    return S_OK;
}
#pragma endregion

#pragma region 4. 몬스터는 자신의 애니메이션을 관리해야한다.
HRESULT CQueenKnight::InitializeAction_ToAnimationMap()
{
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

    m_Action_AnimMap.emplace(L"IDLE", KNIGHT_SWORD_IDLE);
    m_Action_AnimMap.emplace(L"GUARDHIT", KNIGHT_SHIELD_SWORD_GUARD_HIT_01);
    m_Action_AnimMap.emplace(L"DAMAGE", KNIGHT_DAMAGE_BACKSTAB);
    m_Action_AnimMap.emplace(L"HIT", KNIGHT_DAMAGE_BACKSTAB);
    m_Action_AnimMap.emplace(L"ATTACK", KNIGHT_SWORD_ATTACK_NORMAL_00);
    m_Action_AnimMap.emplace(L"ATTACK1", KNIGHT_SWORD_ATTACK_NORMAL_00);
    m_Action_AnimMap.emplace(L"ATTACK2", KNIGHT_SWORD_ATTACK_NORMAL_01);
    m_Action_AnimMap.emplace(L"ATTACK3", KNIGHT_SWORD_ATTACK_NORMAL_02);

    // 중간에 사라지게 해서 플레이어 위에서 나타나서 아래로 내다꼽게.
    m_Action_AnimMap.emplace(L"ATTACK_JUMP", KNIGHT_SWORD_ATTACK_JUMP);

    m_Action_AnimMap.emplace(L"RUN", KNIGHT_SHIELD_SWORD_GUARD_RUN_F);
    m_Action_AnimMap.emplace(L"WALK", KNIGHT_SHIELD_SWORD_GUARD_WALK_F);
    // 같은 애니메이션이지만 다른 이름으로 설정해서 Node에서 사용할 수 있게함.
    m_Action_AnimMap.emplace(L"DETECT", KNIGHT_SHIELD_SWORD_GUARD_RUN_F);
    m_Action_AnimMap.emplace(L"DOWN_START", KNIGHT_DOWN_P_LOOP);
    m_Action_AnimMap.emplace(L"DOWN_END", KNIGHT_DOWN_P_END);
    m_Action_AnimMap.emplace(L"DEATH", KNIGHT_DEATH_F);
    

    m_Action_AnimMap.emplace(L"PHASE_ATTACK1", KNIGHT_SHIELD_SWORD_ATTACK_02B);
    m_Action_AnimMap.emplace(L"PHASE_ATTACK2", KNIGHT_SHIELD_SWORD_ATTACK_02C);
    m_Action_AnimMap.emplace(L"PHASE_ATTACK3", KNIGHT_SHIELD_SWORD_ATTACK_02A);

    /* 재생속도 증가. */
    
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK1"], 3.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK2"], 3.f);
    m_pModelCom->Set_AnimSpeed(m_Action_AnimMap[L"PHASE_ATTACK3"], 3.f);

    //m_pModelCom->Set_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_01, 
    //    m_pModelCom->Get_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_01) * 2.f);
    //
    //m_pModelCom->Set_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_02,
    //    m_pModelCom->Get_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_02) * 2.f);
    //
    //m_pModelCom->Set_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_03,
    //    m_pModelCom->Get_CurrentTickPerSecond(KNIGHT_SWORD_ATTACK_STRONG_03) * 2.f);
    //
    ///* 재생속도 증가. */
    //m_pModelCom->Set_CurrentTickPerSecond(WOLFDEVIL_ATTACK_JUMP, m_pModelCom->Get_CurrentTickPerSecond(WOLFDEVIL_ATTACK_JUMP) * 2.f);

    return S_OK;
}
#pragma endregion


#pragma region 5. 사용되는 버프 / 디버프에 대한 타이머를 관리해야합니다.
HRESULT CQueenKnight::Initialize_BuffDurations()
{
    // 어찌보면 이건 그냥 쿨다운의 영역.
    m_BuffDefault_Durations[BUFF_HIT] = 0.5f;        // 피격: 0.6초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_CORPSE] = 2.0f;       // 시체 : 2.0초
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 0.6f; // 무적 시간.

    // 15 초마다 해당 페이즈 시퀀스 공격 반복
    m_BuffDefault_Durations[QUEEN_BUFF_PHASE_ATTACK_COOLDOWN] = 1.f;
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
        break;
    default:
        break;
    }
}

void CQueenKnight::Disable_Collider(_uint iType)
{
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

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_QueenKnight")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    // 오프셋 지정.

    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    m_fOffsetY = box.fHeight * 0.5f;


    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{};
    AABBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    AABBDesc.vCenter = _float3(0.f, box.vExtents.y, 0.f); // 중점.
    AABBDesc.pOwner = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_AABB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
    {
        CRASH("Failed Clone Collider AABB");
        return E_FAIL;
    }


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

    _float3 vPos = { 0.f, 5.f, 0.f };
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

HRESULT CQueenKnight::Ready_Render_Resources()
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

HRESULT CQueenKnight::Ready_PartObjects()
{
    CKnightLance::KNIGHT_LANCE_DESC Weapon{};
    Weapon.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Weapon.pSocketMatrix = m_pModelCom->Get_BoneMatrix("RightHandAttachSocket");
    Weapon.eCurLevel = m_eCurLevel;
    Weapon.pOwner = this;

    if (FAILED(__super::Add_PartObject(TEXT("Com_Weapon"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_GodChildLance")
        , reinterpret_cast<CPartObject**>(&m_pWeapon), &Weapon)))
    {
        CRASH("Failed Create Queen Weapon");
        return E_FAIL;
    }

  /*  CKnightShield::KNIGHT_SHIELD_DESC Shield{};
    Shield.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Shield.pSocketMatrix = m_pModelCom->Get_BoneMatrix("LeftHandAttachSocket");
    Shield.eCurLevel = m_eCurLevel;
    Shield.pOwner = this;

    if (FAILED(__super::Add_PartObject(TEXT("Com_Shield"),
        ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_GodChildShield")
        , reinterpret_cast<CPartObject**>(&m_pShield), &Shield)))
    {
        CRASH("Failed Create Queen Shield");
        return E_FAIL;
    }*/

    //m_PartObjects[L"Com_Weapon"];

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
    __super::Destroy();
}

void CQueenKnight::Free()
{
    __super::Free();
    Safe_Release(m_pWeapon);
    Safe_Release(m_pShield);
    Safe_Release(m_pTree);
}
#pragma endregion


