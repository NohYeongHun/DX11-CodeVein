#include "PlayerWeapon.h"
CPlayerWeapon::CPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon{ pDevice, pContext }
{

}

CPlayerWeapon::CPlayerWeapon(const CPlayerWeapon& Prototype)
    : CWeapon( Prototype )
{

}

HRESULT CPlayerWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerWeapon::Initialize(void* pArg)
{
    PLAYER_WEAPON_DESC* pDesc = static_cast<PLAYER_WEAPON_DESC*>(pArg);
    //m_pParentState = pDesc->pState;

    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /*m_pTransformCom->Scaling(_float3(0.1f, 0.1f, 0.1f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));*/

    return S_OK;
}

void CPlayerWeapon::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

}

/* Weapon은 CombinedWorldMatrix를 Collider에 전달 해주어야합니다. => 부모 트랜스폼이 있으므로 */
void CPlayerWeapon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    // 1. 가장 먼저 Combined 행렬 초기화
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(m_pSocketMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    // 2. Timer 관련 작업들 진행. 
    Update_Timer(fTimeDelta);
    // 가장 마지막 부근에 Collider Push
    Finalize_Update(fTimeDelta);
}

void CPlayerWeapon::Finalize_Update(_float fTimeDelta)
{
    __super::Finalize_Update(fTimeDelta);
}

void CPlayerWeapon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CPlayerWeapon::Render()
{

#ifdef _DEBUG
if (m_pColliderCom)
    m_pColliderCom->Render();
#endif // _DEBUG

    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }
        

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }



    return S_OK;
}


#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
void CPlayerWeapon::On_Collision_Enter(CGameObject* pOther)
{
}

void CPlayerWeapon::On_Collision_Stay(CGameObject* pOther)
{
}

void CPlayerWeapon::On_Collision_Exit(CGameObject* pOther)
{
}

void CPlayerWeapon::Activate_ColliderFrame(_float fDuration)
{
    CWeapon::Activate_ColliderFrame(fDuration);
}

void CPlayerWeapon::Activate_Collider()
{
    CWeapon::Activate_Collider();
}

void CPlayerWeapon::Deactivate_Collider()
{
    CWeapon::Deactivate_Collider();
}

void CPlayerWeapon::Update_ColliderFrame(_float fTimeDelta)
{
    CWeapon::Update_ColliderFrame(fTimeDelta);
}


#pragma endregion

HRESULT CPlayerWeapon::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sword"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{};
    AABBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    AABBDesc.vCenter = _float3(0.f, 0.f, 0.f); // 중점.
    AABBDesc.pOwner = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_AABB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
    {
        CRASH("Failed Clone Collider AABB");
        return E_FAIL;
    }

    m_pColliderCom->Set_Active(false);

    return S_OK;
}

HRESULT CPlayerWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

CPlayerWeapon* CPlayerWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerWeapon::Clone(void* pArg)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerWeapon::Free()
{
    __super::Free();
}
