#include "WolfDevil.h"

CWolfDevil::CWolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{pDevice, pContext}
{
}

CWolfDevil::CWolfDevil(const CWolfDevil& Prototype)
    : CMonster(Prototype)
{
}

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
        

    if (FAILED(Ready_BehaviourTree()))
    {
        CRASH("Failed Ready BehaviourTree WolfDevil");
        return E_FAIL;
    }

    if (FAILED(InitializeAction_ToAnimationMap()))
    {
        CRASH("Failed Ready InitializeAction WolfDevil");
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
    /*vPos.x += iTest * -10.f;
    vPos.z += iTest * 3.f;*/
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

    m_pModelCom->Set_Animation(Get_CurrentAnimation(), false);

    return S_OK;
}

void CWolfDevil::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWolfDevil::Update(_float fTimeDelta)
{

    /*if (m_pTree)
        m_pTree->Update(fTimeDelta);*/


    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }

    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    __super::Update(fTimeDelta);
}

void CWolfDevil::Late_Update(_float fTimeDelta)
{
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

#pragma region 충돌
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

#pragma region 버프 플래그 관리
HRESULT CWolfDevil::Initialize_BuffDurations()
{
    if (FAILED(__super::Initialize_BuffDurations()))
    {
        CRASH("Failed Init BuffDuration");
        return E_FAIL;
    }

    // 추가 버프 있으면 추가.
    // m_BuffDefaultDurations[BUFF_HIT] = 0.6f;        // 피격: 0.6초

    return S_OK;
}

/* STRING Index 형태로 관리.*/
HRESULT CWolfDevil::InitializeAction_ToAnimationMap()
{
    m_Action_AnimMap.emplace(L"IDLE", WOLFDEVIL_IDLE_LOOP);
    m_Action_AnimMap.emplace(L"ATTACK_NORMAL", WOLFDEVIL_ATTACK_NORMAL);
    m_Action_AnimMap.emplace(L"ATTACK_JUMP", WOLFDEVIL_ATTACK_JUMP);
    m_Action_AnimMap.emplace(L"DEATH_BACK", WOLFDEVIL_DEATH_BACK);
    m_Action_AnimMap.emplace(L"DEATH_NORMAL", WOLFDEVIL_DEATH_NORMAL);
    m_Action_AnimMap.emplace(L"DODGE_BACK", WOLFDEVIL_DODGE_BACK);
    m_Action_AnimMap.emplace(L"DOWN_START", WOLFDEVIL_DOWN_P_START);
    m_Action_AnimMap.emplace(L"DOWN_LOOP", WOLFDEVIL_DOWN_P_LOOP);
    m_Action_AnimMap.emplace(L"DOWN_END", WOLFDEVIL_DOWN_P_END);
    m_Action_AnimMap.emplace(L"THREAT", WOLFDEVIL_THREAT_01);
    m_Action_AnimMap.emplace(L"WALK_F", WOLFDEVIL_WALK_F);
    m_Action_AnimMap.emplace(L"RUN_F", WOLFDEVIL_RUN);
    m_Action_AnimMap.emplace(L"STUN", WOLFDEVIL_STUN);

    return S_OK;
}
#pragma endregion



#pragma region COMPONENT 추가
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


    return S_OK;
}

HRESULT CWolfDevil::Ready_BehaviourTree()
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


#pragma endregion

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
