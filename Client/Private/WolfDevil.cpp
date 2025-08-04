#include "WolfDevil.h"

CWolfDevil::CWolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{pDevice, pContext}
{
}

CWolfDevil::CWolfDevil(const CWolfDevil& Prototype)
    : CMonster(Prototype)
{
}

#pragma region 기본 함수들
HRESULT CWolfDevil::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWolfDevil::Initialize_Clone(void* pArg)
{
    WOLFDEVIL_DESC* pDesc = static_cast<WOLFDEVIL_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Ready_BehaviorTree()))
    {
        CRASH("Failed Ready BehaviourTree WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Initialize_Stats()))
    {
        CRASH("Failed InitializeStat WolfDevil");
        return E_FAIL;
    }
    

    if (FAILED(InitializeAction_ToAnimationMap()))
    {
        CRASH("Failed InitializeAction WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
    {
        CRASH("Failed Ready Components WolfDevil");
        return E_FAIL;
    }

    if (FAILED(Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDuration WolfDevil");
        return E_FAIL;
    }


    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    _float3 vPos = { 5.f, 5.f, 0.f };
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(false);


    return S_OK;
}

void CWolfDevil::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWolfDevil::Update(_float fTimeDelta)
{
    // 이 순서대로 AI 작업을 호출해라.
    Update_AI(fTimeDelta);

    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    __super::Update(fTimeDelta);
}

void CWolfDevil::Late_Update(_float fTimeDelta)
{
    
    if (m_pGameInstance->Get_KeyPress(DIK_1))
    {
        _bool bIsBuffPossible = AddBuff(CMonster::BUFF_DEAD);
    }
    if (m_pGameInstance->Get_KeyPress(DIK_2))
    {
        _bool bIsBuffPossible = AddBuff(CMonster::BUFF_DOWN);
        //if (bIsBuffPossible)
        //    OutputDebugWstring(TEXT("Wolf Devill Down 시작."));
        //else
        //    OutputDebugWstring(TEXT("Wolf Devill Down 실패."));
    }
    if (m_pGameInstance->Get_KeyPress(DIK_3))
    {
        _bool bIsBuffPossible = AddBuff(CMonster::BUFF_HIT);
        //if (bIsBuffPossible)
        //    OutputDebugWstring(TEXT("Wolf Devill HIT 시작."));
        //else
        //    OutputDebugWstring(TEXT("Wolf Devill HIT 실패"));
    }
        

    m_pTransformCom->Set_State(STATE::POSITION
        , m_pNavigationCom->Compute_OnCell(
            m_pTransformCom->Get_State(STATE::POSITION), m_fOffsetY * 0.5f));

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CWolfDevil::Render()
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

        if (FAILED(m_pShaderCom->Begin(0)))
            CRASH("Ready Shader Begin Failed");

        if (FAILED(m_pModelCom->Render(i)))
            CRASH("Ready Render Failed");
    }
    return S_OK;
}



#pragma endregion


#pragma region 0.  몬스터는 충돌에 대한 상태제어를 할 수 있어야한다. => 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
void CWolfDevil::On_Collision_Enter(CGameObject* pOther)
{
}

void CWolfDevil::On_Collision_Stay(CGameObject* pOther)
{
}

void CWolfDevil::On_Collision_Exit(CGameObject* pOther)
{
}

#pragma endregion


#pragma region 1. AI 관리
/* Update 시 AI가 수행해야 하는 순서들을 함수로 표현. */
void CWolfDevil::Update_AI(_float fTimeDelta)
{
    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    // 트리 이후에 상태 값에 대한 초기화를 담당하는 Tick_BuffTimer 실행
    Tick_BuffTimers(fTimeDelta);


    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }

#ifdef _DEBUG
    //OutputDebugWstring(TEXT("현재 WolfDevil의 Buff 쿨타임 : "));
    //OutPutDebugFloat(m_BuffTimers[BUFF_HIT]);
    //OutputDebugWstring(TEXT("현재 WolfDevil의 Buff Flag : "));
    //OutPutDebugInt(m_ActiveBuffs);
    //OutputDebugWstring(TEXT("현재 WolfDevil의 Animation Index : "));
    //OutPutDebugInt(m_pModelCom->Get_CurrentAnimationIndex()); // 현재 모델 인덱스 가져오기.
#endif // _DEBUG

}
#pragma endregion


#pragma region 2. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
// 기본적으로 몬스터 생성시 필요한 STAT 값들을 제외하고 더 필요한 경우 정의
HRESULT CWolfDevil::Initialize_Stats()
{
    return S_OK;
}
#pragma endregion

#pragma region 3. 몬스터는 내 애니메이션이 무엇인지 알아야한다.
/* STRING Index 형태로 관리.*/
HRESULT CWolfDevil::InitializeAction_ToAnimationMap()
{
    m_pModelCom->Set_RootMotionRotation(true);
    m_pModelCom->Set_RootMotionTranslate(true);

    m_Action_AnimMap.emplace(L"IDLE", WOLFDEVIL_IDLE_LOOP);
    m_Action_AnimMap.emplace(L"HIT", WOLFDEVIL_DAMAGE_FRONT);
    m_Action_AnimMap.emplace(L"ATTACK_JUMP", WOLFDEVIL_ATTACK_NORMAL);
    m_Action_AnimMap.emplace(L"ATTACK", WOLFDEVIL_ATTACK_JUMP);
    m_Action_AnimMap.emplace(L"DEATH_BACK", WOLFDEVIL_DEATH_BACK);
    m_Action_AnimMap.emplace(L"DEATH_NORMAL", WOLFDEVIL_DEATH_NORMAL);
    m_Action_AnimMap.emplace(L"DODGE_BACK", WOLFDEVIL_DODGE_BACK);
    m_Action_AnimMap.emplace(L"DOWN_START", WOLFDEVIL_DOWN_P_START);
    m_Action_AnimMap.emplace(L"DOWN_LOOP", WOLFDEVIL_DOWN_P_LOOP);
    m_Action_AnimMap.emplace(L"DOWN_END", WOLFDEVIL_DOWN_P_END);
    m_Action_AnimMap.emplace(L"THREAT", WOLFDEVIL_THREAT_01);
    m_Action_AnimMap.emplace(L"WALK", WOLFDEVIL_WALK_F);
    m_Action_AnimMap.emplace(L"RUN", WOLFDEVIL_RUN);
    // 같은 애니메이션이지만 다른 이름으로 설정해서 Node에서 사용할 수 있게함.
    m_Action_AnimMap.emplace(L"DETECT", WOLFDEVIL_RUN); 
    m_Action_AnimMap.emplace(L"STUN", WOLFDEVIL_STUN);

    /* 재생속도 증가. */ 
    m_pModelCom->Set_CurrentTickPerSecond(WOLFDEVIL_ATTACK_JUMP, m_pModelCom->Get_CurrentTickPerSecond(WOLFDEVIL_ATTACK_JUMP) * 2.f);

    return S_OK;
}

#pragma endregion


#pragma region 3. 몬스터는 자신이 어떤 버프를 소유할 수 있는지를 알아야 한다. => 그리고 그에 맞는 쿨타임도 알아야한다.(몬스터마다 달라질 수 있다.)
HRESULT CWolfDevil::Initialize_BuffDurations()
{
    // 어찌보면 이건 그냥 쿨다운의 영역.
    m_BuffDefault_Durations[BUFF_HIT] = 0.5f;        // 피격: 0.6초
    m_BuffDefault_Durations[BUFF_DOWN] = 5.f;       // 다운: 20초 => 두번 클릭했을 때 다운이 되는가.
    m_BuffDefault_Durations[BUFF_CORPSE] = 2.0f;       // 시체 : 2.0초
    m_BuffDefault_Durations[BUFF_INVINCIBLE] = 0.6f; // 무적 시간.
    // 추가 버프 있으면 추가.
    // m_BuffDefault_Durations[BUFF_HIT] = 0.6f;        // 피격: 0.6초

    return S_OK;
}




#pragma endregion


#pragma region 4. 특수한 상태를 제어하기 위한 함수들
void CWolfDevil::Enable_Collider(_uint iType)
{
}

void CWolfDevil::Disable_Collider(_uint iType)
{
}
#pragma endregion




#pragma region 5. 기본적인 WolfDevil이 생성되기 위한 컴포넌트 객체들.
HRESULT CWolfDevil::Ready_Components(WOLFDEVIL_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_WolfDevil")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    // 오프셋 지정.
    m_fOffsetY = m_pModelCom->Get_BoundingBox().fHeight * 0.5f;

    return S_OK;
}

HRESULT CWolfDevil::Ready_BehaviorTree()
{
   
    CMonsterTree::MONSTER_BT_DESC BT{};
    BT.pOwner = this;

    // Tree에 대한 초기화는 트리 내부에서하기.
    m_pTree = CMonsterTree::Create(m_pDevice, m_pContext, &BT);


    return S_OK;
}

HRESULT CWolfDevil::Ready_Render_Resources()
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

HRESULT CWolfDevil::Ready_PartObjects()
{
    return S_OK;
}


CWolfDevil* CWolfDevil::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWolfDevil* pInstance = new CWolfDevil(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWolfDevil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWolfDevil::Clone(void* pArg)
{
    CWolfDevil* pInstance = new CWolfDevil(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWolfDevil"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWolfDevil::Destroy()
{
    __super::Destroy();
}

void CWolfDevil::Free()
{
    __super::Free();
    Safe_Release(m_pTree);
}


#pragma endregion

