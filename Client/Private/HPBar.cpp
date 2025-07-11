#include "HPBar.h"

CHPBar::CHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CHPBar::CHPBar(const CHPBar& Prototype)
    : CUIObject(Prototype)
{
}

/*
* 주의점 : 플레이어의 무적 시간 만큼 제공해야합니다. (피격 시간) 
* 피격 시간 이내에 한번 더 맞았을 경우 효과가 제대로 안나올 수도 있음.
*/
void CHPBar::Increase_Hp(_uint iHp, _float fTime)
{
    m_fLeftRatio = static_cast<_float>(m_iHp) / static_cast<_float>(m_iMaxHp);
    m_iHp = min(m_iHp + iHp, m_iMaxHp);
    m_fRightRatio = static_cast<_float>(m_iHp) / static_cast<_float>(m_iMaxHp);
    m_bIncrease = true;
}

void CHPBar::Decrease_Hp(_uint iHp, _float fTime)
{
    m_fRightRatio = static_cast<_float>(m_iHp) / static_cast<_float>(m_iMaxHp);
    m_iHp = max(m_iHp - iHp, 0);
    m_fLeftRatio = static_cast<_float>(m_iHp) / static_cast<_float>(m_iMaxHp);
    m_bDecrease = true;
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

    if (FAILED(Ready_Events()))
        return E_FAIL;

    return S_OK;
}

void CHPBar::Priority_Update(_float fTimeDelta)
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

  
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(5);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    // 폰트 출력.
    Render_HP();

    __super::End();

    return S_OK;
}

void CHPBar::Render_HP()
{
    _float fScreenX = m_RenderMatrix._41 + (g_iWinSizeX >> 1) + 80.f;
    _float fScreenY = (g_iWinSizeY >> 1) - m_RenderMatrix._42 - 15.f;

    _float2 vPosition = { fScreenX , fScreenY };
    // Window 좌표계 기준 출력. (0, 0이 좌측 상단)

    wchar_t szBuffer[64] = {};
    swprintf_s(szBuffer, L"%d / %d", m_iHp, m_iMaxHp);

    m_pGameInstance->Render_Font(TEXT("HUD_TEXT"), szBuffer
        , vPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {}, 1.5f);
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

HRESULT CHPBar::Ready_Render_Resources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;


    _float fFillRatio = 1.f - (static_cast<_float>(m_iHp) / static_cast<_float>(m_iMaxHp));
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

HRESULT CHPBar::Ready_Events()
{
#pragma region HP_CHANGE
    m_pGameInstance->Subscribe(EventType::HP_CHANGE, Get_ID(), [this](void* pData)
        {
            HPCHANGE_DESC* desc = static_cast<HPCHANGE_DESC*>(pData);
            if (desc->bIncrease)
                this->Increase_Hp(desc->iHp, desc->fTime);
            else
                this->Decrease_Hp(desc->iHp, desc->fTime);
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::HP_CHANGE);

#pragma endregion
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

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());
}

void CHPBar::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
