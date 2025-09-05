CFade_Out::CFade_Out(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CFade_Out::CFade_Out(const CFade_Out& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CFade_Out::Initialize_Prototype()
{
    CUIObject::Initialize_Prototype();
    return S_OK;
}

HRESULT CFade_Out::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    FADEOUT_DESC* pDesc = static_cast<FADEOUT_DESC*>(pArg);

    if (FAILED(CUIObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone Fade Out");
        return E_FAIL;
    }
        

    m_iTextureIndex = pDesc->iTextureIndex;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready Components Fade Out");
        return E_FAIL;
    }

    // 초기에는 FadeOut 비활성화
    m_IsFadeOut = false;
    m_fFadeTime = 0.f;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath); // 기본 패스
        

    return S_OK;
}

void CFade_Out::Priority_Update(_float fTimeDelta)
{
    // 자식 객체들에게 모두 FadeOut 효과 부여 => Rendering Pass 변경.
    if (m_IsFadeOut && m_fFadeTime >= 2.f)
    {
        m_pGameInstance->Publish<CLevel_StageOne>(EventType::OPEN_GAMEPAY, nullptr);
        m_IsFadeOut = false;

        return;
    }

    CUIObject::Priority_Update(fTimeDelta);
}

void CFade_Out::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);
}

void CFade_Out::Late_Update(_float fTimeDelta)
{
    CUIObject::Late_Update(fTimeDelta);

    // FadeOut 또는 FadeIn이 활성화된 경우에만 렌더 그룹에 추가
    if (m_IsFadeOut || m_IsFadeIn)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
            return;
    }

    Time_Calc(fTimeDelta);
}

HRESULT CFade_Out::Render()
{
    CUIObject::Begin();

    if (FAILED(Ready_Render_Resource()))
    {
        CRASH("Failed Ready Render Resource");
        return E_FAIL;
    }

    m_pShaderCom->Begin(m_iShaderPath);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    CUIObject::End();

    return S_OK;
}

void CFade_Out::Start_FadeOut()
{
    m_IsFadeOut = true;
    m_IsFadeIn = false;
    m_fFadeTime = 0.f;
}

void CFade_Out::Start_FadeIn()
{
    m_IsFadeIn = true;
    m_IsFadeOut = false;
    m_fFadeTime = 2.f; // 최대값에서 시작해서 감소
}

void CFade_Out::Time_Calc(_float fTimeDelta)
{
    if (m_IsFadeOut)
    {
        if (m_fFadeTime < 2.f)
            m_fFadeTime += fTimeDelta;
    }
    else if (m_IsFadeIn)
    {
        if (m_fFadeTime > 0.f)
            m_fFadeTime -= fTimeDelta;
        else
        {
            // FadeIn 완료
            m_IsFadeIn = false;
            m_fFadeTime = 0.f;
        }
    }
}

HRESULT CFade_Out::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_FadeOut_Texture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    {
        CRASH("Failed FadeOut Texture");
        return E_FAIL;
    }
        
    return S_OK;
}

HRESULT CFade_Out::Ready_Render_Resource()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", static_cast<void*>(&m_fAlpha), sizeof(_float))))
        return E_FAIL;

    // FadeOut 패스(8번)에서는 텍스처 바인딩 생략
    if (m_iShaderPath != 8)
    {
        if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
            return E_FAIL;
    }

    _float fFade = Clamp(m_fFadeTime / 2.f, 0.f, 1.f);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFade", static_cast<void*>(&fFade), sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CFade_Out* CFade_Out::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFade_Out* pInstance = new CFade_Out(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFade_Out"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFade_Out::Clone(void* pArg)
{
    CFade_Out* pInstance = new CFade_Out(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CFade_Out"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFade_Out::Free()
{
    CUIObject::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
