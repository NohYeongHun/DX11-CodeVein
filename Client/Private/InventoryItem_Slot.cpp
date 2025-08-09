#include "InventoryItem_Slot.h"

CInventoryItem_Slot::CInventoryItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventoryItem_Slot::CInventoryItem_Slot(const CInventoryItem_Slot& Prototype)
    : CUIObject(Prototype)
{
}


void CInventoryItem_Slot::Change_Item(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_pItem->Change_Item(strTextureTag, iTextureIndex);
}

HRESULT CInventoryItem_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventoryItem_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLSLOT_DESC* pDesc = static_cast<SKILLSLOT_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    return S_OK;
}

void CInventoryItem_Slot::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Priority_Update(fTimeDelta);
}


void CInventoryItem_Slot::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Update(fTimeDelta);
}

void CInventoryItem_Slot::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);

}

HRESULT CInventoryItem_Slot::Render()
{
    CUIObject::Begin();

    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    CUIObject::End();

    return S_OK;
}

HRESULT CInventoryItem_Slot::Ready_Childs()
{
    CInventorySkill_Icon::SKILLICON_DESC Desc{};
    // 정중앙 위치
    Desc.fX = 0;
    Desc.fY = 0; 
    Desc.fSizeX = m_fSizeX;
    Desc.fSizeY = m_fSizeY;
    
    CUIObject* pUIObject = nullptr;
    
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_InventoryItem_Icon"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pItem = static_cast<CInventoryItem_Icon*>(pUIObject);

    return S_OK;
}

HRESULT CInventoryItem_Slot::Ready_Components(SKILLSLOT_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_InventorySlot"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInventoryItem_Slot::Ready_Render_Resources()
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


CInventoryItem_Slot* CInventoryItem_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventoryItem_Slot* pInstance = new CInventoryItem_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventoryItem_Slot::Clone(void* pArg)
{
    CInventoryItem_Slot* pInstance = new CInventoryItem_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventoryItem_Slot::Destroy()
{
    CUIObject::Destroy();
}

void CInventoryItem_Slot::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
