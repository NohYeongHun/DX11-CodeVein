#include "Title_BackGround.h"



CTitle_BackGround::CTitle_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CTitle_BackGround::CTitle_BackGround(const CTitle_BackGround& Prototype)
    : CUIObject(Prototype )
{
}

HRESULT CTitle_BackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTitle_BackGround::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    TITLE_BAKCGROUND_DESC* pDesc = static_cast<TITLE_BAKCGROUND_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    m_iTextureCount = pDesc->iTextureCount;
    m_iPassIdx = pDesc->iPassIdx;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 매번 업데이트 엔진할 때마다 가져옴.
    m_iTextureIdx = pDesc->iTexture;
    m_fAlpha = pDesc->fAlpha;
    m_strObjTag = pDesc->strObjTag;
    m_fChangeTime = pDesc->fChangeTime;

    
    return S_OK;
}

void CTitle_BackGround::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CTitle_BackGround::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTitle_BackGround::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;

    Time_Calc(fTimeDelta);
}

HRESULT CTitle_BackGround::Render()
{
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string str = converter.to_bytes(m_strObjTag);

    ImGui::Begin("Title BackGround");
    ImGui::SliderFloat(str.c_str(), &m_fAlpha, 0.f, 1.f);
    ImGui::End();

    __super::Begin();

    if (FAILED(Ready_Render_Resource()))
        return E_FAIL;


    m_pShaderCom->Begin(m_iPassIdx);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    __super::End();

    return S_OK;
}

void CTitle_BackGround::Start_FadeOut()
{
    m_IsFadeOut = true;
    m_fFadeTime = 0.f;
    m_iPassIdx = 3; // FadeOut
}

// 필요한 시간을 계산하는 함수.
void CTitle_BackGround::Time_Calc(_float fTimeDelta)
{
    if (m_fTime < m_fChangeTime)
    {
        m_fTime += fTimeDelta;
        m_fAlpha += fTimeDelta * 0.1f;
        if (m_fAlpha >= 1.f)
            m_fAlpha = 0.2f;
    }
    else
        m_fTime = 0.f;

    if (m_IsFadeOut)
    {
        if (m_fFadeTime < 1.f)
            m_fFadeTime += fTimeDelta;
    }
    

}


HRESULT CTitle_BackGround::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Title_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTitle_BackGround::Ready_Render_Resource()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Float("g_fAlpha", m_fAlpha)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
        return E_FAIL;

    _float fFade = Clamp(m_fFadeTime / 1.f, 0.f, 1.f);
    if (FAILED(m_pShaderCom->Bind_Float("g_fFade", fFade)))
        return E_FAIL;

    return S_OK;
}

CTitle_BackGround* CTitle_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTitle_BackGround* pInstance = new CTitle_BackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTitle_BackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTitle_BackGround::Clone(void* pArg)
{
    CTitle_BackGround* pInstance = new CTitle_BackGround(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTitle_BackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTitle_BackGround::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
