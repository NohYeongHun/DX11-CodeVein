#include "InventoryStatus_Info.h"

CInventoryStatus_Info::CInventoryStatus_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventoryStatus_Info::CInventoryStatus_Info(const CInventoryStatus_Info& Prototype)
    : CUIObject(Prototype)
{
}

void CInventoryStatus_Info::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;
}

void CInventoryStatus_Info::Change_Info(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;

    // 무슨 이름으로 넣을래?
    m_pGameInstance->Change_Texture_ToGameObject(this, TEXT("Com_Texture")
        , reinterpret_cast<CComponent**>(&m_pTextureCom), ENUM_CLASS(LEVEL::STATIC)
        , strTextureTag);
}

HRESULT CInventoryStatus_Info::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventoryStatus_Info::Initialize_Clone(void* pArg)
{
    m_iTextureIndex = 0;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    STATUS_INFO_DESC* pDesc = static_cast<STATUS_INFO_DESC*>(pArg);
    if (FAILED(Ready_Components()))
        return E_FAIL;


    return S_OK;
}

void CInventoryStatus_Info::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;


    __super::Priority_Update(fTimeDelta);
}


void CInventoryStatus_Info::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);

}

void CInventoryStatus_Info::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CInventoryStatus_Info::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;


    if (ImGui::IsWindowAppearing())              // 또는 static bool once=true;
    {
        ImGui::SetNextWindowPos({ 100, 100 }, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize({ 460, 240 }, ImGuiCond_Appearing); // ← 원하는 픽셀
    }

   

    __super::Begin();
    
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    Render_Info();
    __super::End();

    return S_OK;
}

void CInventoryStatus_Info::Render_Info()
{
    _float fScreenX = m_RenderMatrix._41 + (g_iWinSizeX >> 1) -430.f;
    _float fScreenY = (g_iWinSizeY >> 1) - m_RenderMatrix._42 - 20.f;


    _float2 vPosition = { fScreenX , fScreenY };

    m_statusString = L"STATUS";

    wchar_t szBuffer[64] = {};
    swprintf_s(szBuffer, m_statusString.c_str());

    m_pGameInstance->Render_Font(TEXT("HUD_TEXT"), szBuffer
        , vPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, {}, 1.f);
}

HRESULT CInventoryStatus_Info::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Inventory_StatusInfo"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInventoryStatus_Info::Ready_Render_Resources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;


    return S_OK;
}

CInventoryStatus_Info* CInventoryStatus_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventoryStatus_Info* pInstance = new CInventoryStatus_Info(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventoryStatus_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventoryStatus_Info::Clone(void* pArg)
{
    CInventoryStatus_Info* pInstance = new CInventoryStatus_Info(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventoryStatus_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventoryStatus_Info::Destroy()
{
    __super::Destroy();
}

void CInventoryStatus_Info::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
