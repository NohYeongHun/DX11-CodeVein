#include "TitleText.h"

CTitleText::CTitleText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CTitleText::CTitleText(const CTitleText& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CTitleText::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CTitleText::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    TITLETEXT_DESC* pDesc = static_cast<TITLETEXT_DESC*>(pArg);
    

    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    m_iTextureIndex = pDesc->iTextureIndex;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTitleText::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CTitleText::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTitleText::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;

    Time_Calc(fTimeDelta);
}

HRESULT CTitleText::Render()
{
    __super::Begin();

    if (FAILED(Ready_Render_Resource()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iPassIdx);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    __super::End();

    return S_OK;
}

void CTitleText::Start_FadeOut()
{
    m_IsFadeOut = true;
    m_fFadeTime = 0.f;
    m_iPassIdx = 3; // FadeOut
}

void CTitleText::Time_Calc(_float fTimeDelta)
{
    if (m_IsFadeOut)
    {
        if (m_fFadeTime < 1.f)
            m_fFadeTime += fTimeDelta;
    }
}

HRESULT CTitleText::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Title_Text"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTitleText::Ready_Render_Resource()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", static_cast<void*>(&m_fAlpha), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    _float fFade = Clamp(m_fFadeTime / 1.f, 0.f, 1.f);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFade", static_cast<void*>(&fFade), sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CTitleText* CTitleText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTitleText* pInstance = new CTitleText(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTitleText"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTitleText::Clone(void* pArg)
{
    CTitleText* pInstance = new CTitleText(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTitleText"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTitleText::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
