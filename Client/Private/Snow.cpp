CSnow::CSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CSnow::CSnow(const CSnow& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CSnow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSnow::Initialize_Clone(void* pArg)
{
    SNOW_DESC* pDesc = static_cast<SNOW_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone GameObject");
        return E_FAIL;
    }

    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    return S_OK;
}

void CSnow::Priority_Update(_float fTimeDelta)
{
}

void CSnow::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Drop(fTimeDelta);
}

void CSnow::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
    {
        CRASH("Failed Add RenderGroup");
        return;
    }
        
}

HRESULT CSnow::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSnow::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Ready Shader");
        return E_FAIL;
    }
        

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Particle_Snow"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
    {
        CRASH("Failed Ready Com_VIBuffer");
        return E_FAIL;
    }
        

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    {
        CRASH("Failed Ready Com_Texture");
        return E_FAIL;
    }
        

    return S_OK;
}

HRESULT CSnow::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CSnow* CSnow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSnow* pInstance = new CSnow(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSnow"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSnow::Clone(void* pArg)
{
    CSnow* pInstance = new CSnow(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CSnow"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSnow::Free()
{
    CGameObject::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
