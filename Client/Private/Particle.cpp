CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CParticle::CParticle(const CParticle& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CParticle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle::Initialize_Clone(void* pArg)
{
    PARTICLE_DESC* pDesc = static_cast<PARTICLE_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone Particle");
        return E_FAIL;
    }

    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready Components");
        return E_FAIL;
    }
    
    return S_OK;
}

void CParticle::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CParticle::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
    m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticle::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
    {
        CRASH("Failed Create Particle")
        return;
    }
        
}

HRESULT CParticle::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Failed Bind Resources");
        return E_FAIL;
    }

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CParticle::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Ready Shader");
        return E_FAIL;
    }
        

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Particle_Explosion"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
    {
        CRASH("Failed Ready VIBuffer");
        return E_FAIL;
    }
        

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    {
        CRASH("Failed Ready Texture");
        return E_FAIL;
    }
        

    return S_OK;
}

HRESULT CParticle::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
    {
        CRASH("Failed Bind WorldMatrix");
        return E_FAIL;
    }
        

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind ViewMatrix");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind ProjMatrix");
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
    {
        CRASH("Failed Bind Diffuse Texture");
        return E_FAIL;
    }

    return S_OK;
}

CParticle* CParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle* pInstance = new CParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle::Clone(void* pArg)
{
    CParticle* pInstance = new CParticle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle::Free()
{
    CGameObject::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
