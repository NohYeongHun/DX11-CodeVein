CTestParticle::CTestParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTestParticle::CTestParticle(const CTestParticle& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTestParticle::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Create Prototype");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CTestParticle::Initialize_Clone(void* pArg)
{
    TESTPARTICLE_DESC* pDesc = static_cast<TESTPARTICLE_DESC*>(pArg);
    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Create Prototype");
        return E_FAIL;
    }



    return S_OK;
}

void CTestParticle::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CTestParticle::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);
}

void CTestParticle::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);
}

HRESULT CTestParticle::Render()
{
    return S_OK;
}

HRESULT CTestParticle::Bind_ShaderResources()
{
    return S_OK;
}

CTestParticle* CTestParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestParticle* pInstance = new CTestParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTestParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestParticle::Clone(void* pArg)
{
    CTestParticle* pInstance = new CTestParticle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTestParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestParticle::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);
}
