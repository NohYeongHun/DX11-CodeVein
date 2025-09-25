CEncounter_Title::CEncounter_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CEncounter_Title::CEncounter_Title(const CEncounter_Title& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CEncounter_Title::Initialize_Prototype()
{
    CUIObject::Initialize_Prototype();
    return S_OK;
}

HRESULT CEncounter_Title::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    ENCOUNTER_TITLE_DESC* pDesc = static_cast<ENCOUNTER_TITLE_DESC*>(pArg);


    if (FAILED(CUIObject::Initialize_Clone(pDesc)))
        return E_FAIL;

    m_strTextureTag = pDesc->strTextureTag;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);

    // 타이머 초기화
    m_fTime = 0.f;
    m_fFadeTime = 0.f;
    m_IsFadeOut = false;
    m_iFrameCount = 0;

    m_fFadeEnd = pDesc->fFadeTime;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CEncounter_Title::Priority_Update(_float fTimeDelta)
{
    CUIObject::Priority_Update(fTimeDelta);
}

void CEncounter_Title::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);
}

void CEncounter_Title::Late_Update(_float fTimeDelta)
{
    CUIObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

    Time_Calc(fTimeDelta);
}

HRESULT CEncounter_Title::Render()
{
    OutputDebugStringW(L"[DEBUG] EncounterTitle Render() called!\n");

    CUIObject::Begin();

    if (FAILED(Ready_Render_Resource()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iShaderPath);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    CUIObject::End();

    return S_OK;
}

void CEncounter_Title::Start_FadeOut()
{
    m_IsFadeOut = true;
    m_fFadeTime = 0.f;

    //m_iShaderPath = static_cast<_uint>(POSTEX_SHADERPATH::FADEOUT); // FadeOut
}

void CEncounter_Title::Time_Calc(_float fTimeDelta)
{
    // 이제 QueenKnight에서 시간을 관리하므로 여기서는 페이드아웃만 처리
    if (m_IsFadeOut)
    {
        if (m_fFadeTime < m_fFadeEnd)
            m_fFadeTime += fTimeDelta;
    }
}

HRESULT CEncounter_Title::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), m_strTextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEncounter_Title::Ready_Render_Resource()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", static_cast<void*>(&m_fAlpha), sizeof(_float))))
        return E_FAIL;

    /*if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;*/
    
    if (FAILED(m_pTextureCom->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTextures")))
        return E_FAIL;

    //_float fFade = Clamp(m_fFadeTime / 1.f, 0.f, 1.f);
    _float fFade = Normalize(m_fFadeTime, 0.f, m_fFadeEnd);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFade", static_cast<void*>(&fFade), sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CEncounter_Title* CEncounter_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEncounter_Title* pInstance = new CEncounter_Title(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEncounter_Title"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEncounter_Title::Clone(void* pArg)
{
    CEncounter_Title* pInstance = new CEncounter_Title(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CEncounter_Title"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEncounter_Title::Free()
{
    CUIObject::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
