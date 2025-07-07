#include "HPBar.h"

CHPBar::CHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CHPBar::CHPBar(const CHPBar& Prototype)
    : CUIObject(Prototype)
{
}


HRESULT CHPBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHPBar::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iTextureIndex = 0;
    m_iMaxHp = 500;
    m_iHp    = m_iMaxHp;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHPBar::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}


void CHPBar::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CHPBar::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CHPBar::Render()
{
    __super::Begin();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_iTextureIndex", m_iTextureIndex)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    Render_HP();

    __super::End();

    return S_OK;
}

void CHPBar::Render_HP()
{
    _float fScreenX = m_RenderMatrix._41 + (g_iWinSizeX >> 1) + 100.f;
    _float fScreenY = (g_iWinSizeY >> 1) - m_RenderMatrix._42 - 15.f;

    _float2 vPosition = { fScreenX , fScreenY };
    // Window 좌표계 기준 출력. (0, 0이 좌측 상단)

    wchar_t szBuffer[64] = {};
    swprintf_s(szBuffer, L"%d / %d", m_iHp, m_iMaxHp);

    m_pGameInstance->Render_Font(TEXT("HUD_TEXT"), szBuffer
        , vPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

HRESULT CHPBar::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HPBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CHPBar* CHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHPBar* pInstance = new CHPBar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CHPBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHPBar::Clone(void* pArg)
{
    CHPBar* pInstance = new CHPBar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CHPBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHPBar::Destroy()
{
    __super::Destroy();

}

void CHPBar::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
