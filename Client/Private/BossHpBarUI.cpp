#include "BossHpBarUI.h"

CBossHpBarUI::CBossHpBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CBossHpBarUI::CBossHpBarUI(const CBossHpBarUI& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CBossHpBarUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossHpBarUI::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    BOSS_HPBAR_DESC* pDesc = static_cast<BOSS_HPBAR_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components");
        return E_FAIL;
    }

    m_fMaxHp = pDesc->fMaxHp;
    m_fHp = m_fMaxHp;
    m_strName = pDesc->strName;

    m_pTransformCom->Scale(_float3(0.7f, 0.7f, 1.f));

    return S_OK;
}

void CBossHpBarUI::Priority_Update(_float fTimeDelta)
{

    if (m_bDecrease)
    {
        if (m_fRightRatio <= m_fLeftRatio)
        {
            m_fRightRatio = 0;
            m_fLeftRatio = 0;
            m_bDecrease = false;
        }
        else
            m_fRightRatio -= fTimeDelta * 0.2f;
    }
    if (m_bIncrease)
    {
        if (m_fRightRatio <= m_fLeftRatio)
        {
            m_fRightRatio = 0;
            m_fLeftRatio = 0;
            m_bIncrease = false;
        }
        else
            m_fLeftRatio += fTimeDelta * 0.2f;
    }

    CUIObject::Priority_Update(fTimeDelta);
}

void CBossHpBarUI::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);

    m_fFontScreenX = (g_iWinSizeX >> 1) - 60.f;
    m_fFontScreenY = (g_iWinSizeY >> 1) - m_RenderMatrix._42 - 50.f;
}

void CBossHpBarUI::Late_Update(_float fTimeDelta)
{
    CUIObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CBossHpBarUI::Render()
{
    CUIObject::Begin();


    if (FAILED(Ready_Render_Resources()))
    {
        CRASH("Failed Ready Render Resources");
        return E_FAIL;
    }

    /* HP 깎이는 효과*/
    m_pShaderCom->Begin(5);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    Render_Text();

    CUIObject::End();

    return S_OK;
}

void CBossHpBarUI::Render_Text()
{
    _float2 vPosition = { m_fFontScreenX , m_fFontScreenY };

    wchar_t szBuffer[64] = {};
    swprintf_s(szBuffer, m_strName.c_str());

    m_pGameInstance->Render_Font(TEXT("KR_TEXT"), szBuffer
        , vPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {}, 1.f);
}

void CBossHpBarUI::Increase_Hp(_float fHp, _float fTime)
{
    if (m_bDecrease)
    {
        m_fLeftRatio = m_fRightRatio;
        m_fHp = m_fHp < fHp ? 0 : m_fHp + fHp;
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_bDecrease = false;
    }
    else
    {
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_fHp = min(m_fHp + fHp, m_fMaxHp);
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
    }

    m_bIncrease = true;
}

void CBossHpBarUI::Decrease_Hp(_float fHp, _float fTime)
{
    if (m_bIncrease)
    {
        m_fRightRatio = m_fLeftRatio;
        m_fHp = m_fHp < fHp ? 0 : m_fHp - fHp;
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_bIncrease = false;
    }
    else
    {
        m_fRightRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
        m_fHp = max(m_fHp - fHp, 0);
        m_fLeftRatio = static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp);
    }

    m_bDecrease = true;
}





HRESULT CBossHpBarUI::Ready_Components(BOSS_HPBAR_DESC* pDesc)
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Failed Ready Com_Shader");
        return E_FAIL;
    }
     

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
    {
        CRASH("Failed Ready Com_VIBuffer");
        return E_FAIL;
    }

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BossHPBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    {
        CRASH("Failed Ready Com_Texture");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CBossHpBarUI::Ready_Render_Resources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    /* HP 비율 */
    _float fFillRatio = 1.f - (static_cast<_float>(m_fHp) / static_cast<_float>(m_fMaxHp));
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFillRatio", static_cast<void*>(&fFillRatio), sizeof(fFillRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLeftRatio", static_cast<void*>(&m_fLeftRatio), sizeof(m_fLeftRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRightRatio", static_cast<void*>(&m_fRightRatio), sizeof(m_fRightRatio))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIncrease", static_cast<void*>(&m_bIncrease), sizeof(m_bIncrease))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    return S_OK;
}

CBossHpBarUI* CBossHpBarUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossHpBarUI* pInstance = new CBossHpBarUI(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed CBossHPBarUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossHpBarUI::Clone(void* pArg)
{
    CBossHpBarUI* pInstance = new CBossHpBarUI(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed CBossHPBarUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossHpBarUI::Destroy()
{
    CUIObject::Destroy();
}

void CBossHpBarUI::Free()
{
    CUIObject::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
