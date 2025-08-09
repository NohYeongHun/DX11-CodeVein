#include "InventoryItem_Icon.h"

CInventoryStatus_Icon::CInventoryStatus_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventoryStatus_Icon::CInventoryStatus_Icon(const CInventoryStatus_Icon& Prototype)
    : CUIObject(Prototype)
{
}

void CInventoryStatus_Icon::Change_Item(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;

    // 무슨 이름으로 넣을래?
    m_pGameInstance->Change_Texture_ToGameObject(this, TEXT("Com_Texture")
        , reinterpret_cast<CComponent**>(&m_pTextureCom), ENUM_CLASS(LEVEL::STATIC)
        , strTextureTag);
}

HRESULT CInventoryStatus_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventoryStatus_Icon::Initialize_Clone(void* pArg)
{
    m_iTextureIndex = 0;

    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    STATUS_ICON_DESC* pDesc = static_cast<STATUS_ICON_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_iTextureIndex = pDesc->iTextureIndex;

    m_pTransformCom->Scale(_float3(0.8f, 0.8f, 1.f));

    /* Change Skill로 기본 Skill들 채워넣기*/


    return S_OK;
}

void CInventoryStatus_Icon::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Priority_Update(fTimeDelta);
}


void CInventoryStatus_Icon::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Update(fTimeDelta);
}

void CInventoryStatus_Icon::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);
}

HRESULT CInventoryStatus_Icon::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;

    CUIObject::Begin();
    
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    CUIObject::End();

    return S_OK;
}

HRESULT CInventoryStatus_Icon::Ready_Components(STATUS_ICON_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_StatusIcon"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    // Texture Component는 Change Skill 이벤트로 넣는다.

    return S_OK;
}

HRESULT CInventoryStatus_Icon::Ready_Render_Resources()
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

CInventoryStatus_Icon* CInventoryStatus_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventoryStatus_Icon* pInstance = new CInventoryStatus_Icon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventoryStatus_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventoryStatus_Icon::Clone(void* pArg)
{
    CInventoryStatus_Icon* pInstance = new CInventoryStatus_Icon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventoryStatus_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventoryStatus_Icon::Destroy()
{
    CUIObject::Destroy();
}

void CInventoryStatus_Icon::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
