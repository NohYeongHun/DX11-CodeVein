#include "KnightShield.h"

CKnightShield::CKnightShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CKnightShield::CKnightShield(const CKnightShield& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CKnightShield::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKnightShield::Initialize(void* pArg)
{
    KNIGHT_SHIELD_DESC* pDesc = static_cast<KNIGHT_SHIELD_DESC*>(pArg);


    if (FAILED(CWeapon::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }
        

    m_pTransformCom->Add_Rotation(XMConvertToRadians(180.f), 0.f, -90.f);

    return S_OK;
}

void CKnightShield::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);
}

void CKnightShield::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(m_pSocketMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    CWeapon::Finalize_Update(fTimeDelta);
}

void CKnightShield::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CKnightShield::Render()
{
#ifdef _DEBUG
    //Edit_Collider(m_pColliderCom, "QueenKnight Shield");
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

void CKnightShield::On_Collision_Enter(CGameObject* pOther)
{
}

void CKnightShield::On_Collision_Stay(CGameObject* pOther)
{
}

void CKnightShield::On_Collision_Exit(CGameObject* pOther)
{
}

HRESULT CKnightShield::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
        

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Model_GodChildShield"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
    {
        CRASH("Failed Load Model")
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CKnightShield::Ready_Colliders()
{
    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    OBBDesc.vCenter = _float3(0.f, box.vExtents.y, 0.f); // 중점.
    OBBDesc.vRotation = _float3(1.5f, 0.f, 0.f); // 중점.
    OBBDesc.pOwner = this;
    OBBDesc.eCollisionType = CCollider::COLLISION_TRIGGER;
    OBBDesc.eMyLayer = CCollider::MONSTER_WEAPON;
    OBBDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON |
        CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_OBB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
    {
        CRASH("Failed Clone Collider OBB");
        return E_FAIL;
    }

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom);

    return S_OK;
}

HRESULT CKnightShield::Bind_ShaderResources()
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

CKnightShield* CKnightShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKnightShield* pInstance = new CKnightShield(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKnightShield"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKnightShield::Clone(void* pArg)
{
    CKnightShield* pInstance = new CKnightShield(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CKnightShield"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKnightShield::Free()
{
    CWeapon::Free();
}
