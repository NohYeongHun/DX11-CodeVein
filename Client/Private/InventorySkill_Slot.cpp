#include "InventorySkill_Slot.h"

CInventorySkill_Slot::CInventorySkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventorySkill_Slot::CInventorySkill_Slot(const CInventorySkill_Slot& Prototype)
    : CUIObject(Prototype)
{
}

void CInventorySkill_Slot::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;
    if (nullptr != m_pSkill)
        m_pSkill->Set_Visibility();
}

void CInventorySkill_Slot::Change_Skill(CSkillUI_Icon* pSkillIcon, _uint iTextureIndex)
{
    m_pSkill->Change_Skill(pSkillIcon, iTextureIndex);
}

HRESULT CInventorySkill_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventorySkill_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLSLOT_DESC* pDesc = static_cast<SKILLSLOT_DESC*>(pArg);
    m_iSlotIndex = pDesc->iSlotIndex;
    m_iPanelIndex = pDesc->iPanelIndex;
    m_iPanelType = pDesc->iPanelType;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    return S_OK;
}

void CInventorySkill_Slot::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Priority_Update(fTimeDelta);
}


void CInventorySkill_Slot::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);   

    // UI 전용 확인.
    if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::LB))
    {
        if (Mouse_InRect2D(g_hWnd))
        {
            SKILLINFO_DISPLAY_DESC Desc{};
            Desc.Isvisibility = true;
            Desc.iPanelType = CInventory_Panel::PANELTYPE::SKILL_PANEL;
            Desc.iPanelIndex = m_iPanelIndex;
            Desc.iSlotIndex = m_iSlotIndex;
            m_pGameInstance->Publish(EventType::SKILLINFO_DISPLAY, &Desc);
        }
    }
}

void CInventorySkill_Slot::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);

}

HRESULT CInventorySkill_Slot::Render()
{
    __super::Begin();

    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    __super::End();

    return S_OK;
}

HRESULT CInventorySkill_Slot::Ready_Childs()
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
            , TEXT("Prototype_GameObject_InventorySkill_Icon"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pSkill = static_cast<CInventorySkill_Icon*>(pUIObject);

    return S_OK;
}

HRESULT CInventorySkill_Slot::Ready_Components(SKILLSLOT_DESC* pDesc)
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

HRESULT CInventorySkill_Slot::Ready_Render_Resources()
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


CInventorySkill_Slot* CInventorySkill_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventorySkill_Slot* pInstance = new CInventorySkill_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventorySkill_Slot::Clone(void* pArg)
{
    CInventorySkill_Slot* pInstance = new CInventorySkill_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventorySkill_Slot::Destroy()
{
    __super::Destroy();
}

void CInventorySkill_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
