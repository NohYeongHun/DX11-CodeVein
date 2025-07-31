#include "SkyBoss.h"
CSkyBoss::CSkyBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CSkyBoss::CSkyBoss(const CSkyBoss& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CSkyBoss::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkyBoss::Initialize_Clone(void* pArg)
{
    SKYBOSS_DESC* pDesc = static_cast<SKYBOSS_DESC*>(pArg);
    
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_BehaviourTree()))
    {
        CRASH("Failed Ready BehaviourTree SkyBoss")
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


    _vector qInitRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.0f);
    m_pTransformCom->Set_Quaternion(qInitRot);

    static _uint iTest = 0;
    _float3 vPos = { 0.f, 5.f, 0.f };
    vPos.x += iTest * -10.f;
    vPos.z += iTest * 3.f;
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

    iTest++;
    return S_OK;
}

void CSkyBoss::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkyBoss::Update(_float fTimeDelta)
{

    if (m_pTree)
        m_pTree->Update(fTimeDelta);

    
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }
   
    // 하위 객체들 움직임 제어는 Tree 제어 이후에
    __super::Update(fTimeDelta);
    
}

void CSkyBoss::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);
    
}

HRESULT CSkyBoss::Render()
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

#pragma region 움직임 구현


/* 
* Animation
*/
#pragma endregion

void CSkyBoss::On_Collision_Enter(CGameObject* pOther)
{
}

void CSkyBoss::On_Collision_Stay(CGameObject* pOther)
{
}

void CSkyBoss::On_Collision_Exit(CGameObject* pOther)
{
}

#pragma region SkyBoss 상태 함수들

_bool CSkyBoss::Is_StrongHit()
{
    return true;
}

#pragma endregion


#pragma region READY OBJECT, COMPONENT

/* 필수 컴포넌트 */
HRESULT CSkyBoss::Ready_Components(SKYBOSS_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_eCurLevel)
        , TEXT("Prototype_Component_Model_SkyBoss")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;



    return S_OK;
}

HRESULT CSkyBoss::Ready_BehaviourTree()
{
    CSkyBossTree::SKYBOSS_BT_DESC BT{};
    BT.pOwner = this;

    // Tree에 대한 초기화는 트리 내부에서하기.
    m_pTree = CSkyBossTree::Create(m_pDevice, m_pContext, &BT);

    return S_OK;
}


HRESULT CSkyBoss::Ready_Render_Resources()
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

HRESULT CSkyBoss::Ready_PartObjects()
{


    return S_OK;
}

#pragma endregion



CSkyBoss* CSkyBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkyBoss* pInstance = new CSkyBoss(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSkyBoss"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkyBoss::Clone(void* pArg)
{
    CSkyBoss* pInstance = new CSkyBoss(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkyBoss::Destroy()
{
    __super::Destroy();
}

void CSkyBoss::Free()
{
    __super::Free();
    Safe_Release(m_pTree);
}
