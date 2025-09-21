CWhiteLargeHalberd::CWhiteLargeHalberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CWhiteLargeHalberd::CWhiteLargeHalberd(const CWhiteLargeHalberd& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CWhiteLargeHalberd::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWhiteLargeHalberd::Initialize(void* pArg)
{
    WHITE_LARGEHALBERD_DESC* pDesc = static_cast<WHITE_LARGEHALBERD_DESC*>(pArg);

    if (FAILED(CWeapon::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Initialize Clone");
        return E_FAIL;
    }

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

    // 기본 false;
    m_pColliderCom->Set_Active(false);

    return S_OK;
}

void CWhiteLargeHalberd::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);
}

void CWhiteLargeHalberd::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(m_pSocketMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    CWeapon::Update_Timer(fTimeDelta);
    CWeapon::Finalize_Update(fTimeDelta);
    
}

void CWhiteLargeHalberd::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);

    if (Is_Visible())
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
            return;
    }

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
        return;
#endif // _DEBUG

}

HRESULT CWhiteLargeHalberd::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
        {
            CRASH("Failed Ready Bind Materials");
            return E_FAIL;
        }
            

        m_pShaderCom->Begin(m_iShaderPath);
        m_pModelCom->Render(i);
    }


    return S_OK;

}

void CWhiteLargeHalberd::On_Collision_Enter(CGameObject* pOther)
{
}

void CWhiteLargeHalberd::On_Collision_Stay(CGameObject* pOther)
{
}

void CWhiteLargeHalberd::On_Collision_Exit(CGameObject* pOther)
{
}

HRESULT CWhiteLargeHalberd::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
        

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Model_WhiteLargeHalberd"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
    {
        CRASH("Failed Load Model");
        return E_FAIL;
    }

    // Dissolve Texture
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve"),
        TEXT("Com_Dissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTexture), nullptr)))
    {
        CRASH("Failed Load DissolveTexture");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CWhiteLargeHalberd::Ready_Colliders()
{
    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(0.5f, 0.5f, 2.7f);
    OBBDesc.vCenter = _float3(0.f, -2.f, 0.f); // 중점.

    OBBDesc.vRotation = _float3(
        XMConvertToRadians(-95.f),  // Pitch: 아래쪽으로 기울어짐
        XMConvertToRadians(45.f),   // Yaw: 오른쪽으로 회전
        XMConvertToRadians(0.f)     // Roll: 회전 없음
    );

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
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));

    return S_OK;
}

HRESULT CWhiteLargeHalberd::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    /*const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;*/

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pDissolveTexture->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;

    _float fDissolveTime = {};
    if (m_bDissolve)
        fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxDissolveTime);
    else if (m_bReverseDissolve)
        fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxReverseDissolveTime);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
    {
        CRASH("Failed Dissolve Texture");
        return E_FAIL;
    }


    return S_OK;
}

CWhiteLargeHalberd* CWhiteLargeHalberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWhiteLargeHalberd* pInstance = new CWhiteLargeHalberd(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWhiteLargeHalberd"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWhiteLargeHalberd::Clone(void* pArg)
{
    CWhiteLargeHalberd* pInstance = new CWhiteLargeHalberd(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CWhiteLargeHalberd"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWhiteLargeHalberd::Free()
{
    CWeapon::Free();
}
