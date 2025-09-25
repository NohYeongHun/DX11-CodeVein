CLoading_Slot::CLoading_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CLoading_Slot::CLoading_Slot(const CLoading_Slot& Prototype)
    : CUIObject(Prototype)
{
}

void CLoading_Slot::Change_Shader_Pass(_uint iShaderPath)
{
    m_iShaderPath = iShaderPath;
}


HRESULT CLoading_Slot::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CLoading_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iTextureIndex = 0;

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // Loader가 얘들을 생성도 하기전에 그려진다.

    return S_OK;
}

void CLoading_Slot::Priority_Update(_float fTimeDelta)
{
    CUIObject::Priority_Update(fTimeDelta);
}


void CLoading_Slot::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);
}

void CLoading_Slot::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);
    
}

// 한 프레임 한번 바인딩된 Texture가 쭉 이어졌다. 
// ViewPort 문제?
/* View Project 문제가 아님.*/
HRESULT CLoading_Slot::Render()
{
    CUIObject::Begin();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(m_iShaderPath);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    CUIObject::End();

    return S_OK;
}

HRESULT CLoading_Slot::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_Texture_Loading_Slot"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}


CLoading_Slot* CLoading_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_Slot* pInstance = new CLoading_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_Slot::Clone(void* pArg)
{
    CLoading_Slot* pInstance = new CLoading_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_Slot::Destroy()
{
    CUIObject::Destroy();
}

void CLoading_Slot::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
