#include "Inventory_Panel.h"

CInventory_Panel::CInventory_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventory_Panel::CInventory_Panel(const CInventory_Panel& Prototype)
    : CUIObject(Prototype)
{
}

void CInventory_Panel::Change_Skill(_uint iSkillSlot, CSkillUI_Icon* pSkillIcon, _uint iTextureIndex)
{
    if (m_ePanelType == SKILL_PANEL)
        m_SkillSlots[iSkillSlot]->Change_Skill(pSkillIcon, iTextureIndex);
}

void CInventory_Panel::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;

	for (auto& val : m_SkillSlots)
		val->Set_Visibility();
    
    for (auto& val : m_ItemSlots)
		val->Set_Visibility();
    
    for (auto& val : m_StatusIcons)
		val->Set_Visibility();
    
    for (auto& val : m_StatusInfos)
		val->Set_Visibility();
}

HRESULT CInventory_Panel::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CInventory_Panel::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

     m_iInventory_Slot = 4; 

     INVENTORY_PANEL_DESC* pDesc = static_cast<INVENTORY_PANEL_DESC*>(pArg);

     m_ePanelType = pDesc->ePanelType;
     m_iPanelIdx = pDesc->iPanelIdx;
    if (FAILED(CUIObject::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CInventory_Panel::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Priority_Update(fTimeDelta);
}

void CInventory_Panel::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Update(fTimeDelta);
}

void CInventory_Panel::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CInventory_Panel::Render()
{
  
    return S_OK;
}

HRESULT CInventory_Panel::Ready_Components()
{
    return S_OK;
}


HRESULT CInventory_Panel::Ready_Childs(INVENTORY_PANEL_DESC* pDesc)
{
    HRESULT hr = {};
    switch (pDesc->ePanelType)
    {
    case PANELTYPE::SKILL_PANEL:
        hr = Ready_Skill_Childs(pDesc);
        break;
    case PANELTYPE::ITEM_PANEL:
        hr = Ready_Item_Childs(pDesc);
        break;
    case PANELTYPE::STATUS_PANEL:
        hr = Ready_Status_Childs(pDesc);
        break;
    }

    if (FAILED(hr))
        return E_FAIL;

    return S_OK;

}

HRESULT CInventory_Panel::Ready_Skill_Childs(INVENTORY_PANEL_DESC* pDesc)
{
    m_iInventory_Slot = pDesc->iInventorySlot;

    _float fSizeX = pDesc->fSlot_SizeX;
    _float fSizeY = pDesc->fSlot_SizeY;

    CInventorySkill_Slot::SKILLSLOT_DESC Desc{};
    Desc.fX = 0;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;

    const _float posX[4] = { -fSizeX * 0.5f - 2.f, fSizeX * 0.5f + 2.f, -fSizeX * 0.5f - 2.f, fSizeX * 0.5f + 2.f };
    const _float posY[4] = { fSizeY * 0.5f + 2.f, fSizeY * 0.5f + 2.f , -fSizeY * 0.5f - 2.f, -fSizeY * 0.5f - 2.f };


    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iInventory_Slot; ++i)
    {
        Desc.fX = posX[i];
        Desc.fY = posY[i];
        
        Desc.iPanelIndex = m_iPanelIdx;
        Desc.iSlotIndex = i;
        Desc.iPanelType = m_ePanelType;

        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::STATIC)
                , TEXT("Prototype_GameObject_InventorySkill_Slot"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_SkillSlots.push_back(static_cast<CInventorySkill_Slot*>(pUIObject));
    }

    return S_OK;
}

HRESULT CInventory_Panel::Ready_Item_Childs(INVENTORY_PANEL_DESC* pDesc)
{
    m_iInventory_Slot = pDesc->iInventorySlot;

    _float fSizeX = pDesc->fSlot_SizeX;
    _float fSizeY = pDesc->fSlot_SizeY;

    CInventorySkill_Slot::SKILLSLOT_DESC Desc{};
    Desc.fX = 0;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;

    const _float posX[3] = { -fSizeX - 5.f, 0.f, fSizeX + 5.f};
    const _float posY[3] = { 0.f, 0.f, 0.f};

    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iInventory_Slot; ++i)
    {
        Desc.fX = posX[i];
        Desc.fY = posY[i];

        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::STATIC)
                , TEXT("Prototype_GameObject_InventoryItem_Slot"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_ItemSlots.push_back(static_cast<CInventoryItem_Slot*>(pUIObject));
    }

    return S_OK;
}

HRESULT CInventory_Panel::Ready_Status_Childs(INVENTORY_PANEL_DESC* pDesc)
{
    
    m_iInventory_Slot = pDesc->iInventorySlot;
    _float fSizeX = pDesc->fSlot_SizeX;
    _float fSizeY = pDesc->fSlot_SizeY;

    CInventoryStatus_Icon::STATUS_ICON_DESC Desc{};
    Desc.fX = 0;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;

    _float fPosX = -200.f;
    _float fPosY = 0.f;

    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iInventory_Slot; ++i)
    {
        Desc.fX = fPosX + i * fSizeX ;
        Desc.fY = fPosY;
        Desc.iTextureIndex = i;

        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::STATIC)
                , TEXT("Prototype_GameObject_InventoryStatus_Icon"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_StatusIcons.push_back(static_cast<CInventoryStatus_Icon*>(pUIObject));
    }

    /* Status 설명창 */
    UIOBJECT_DESC InfoDesc{};

    InfoDesc.fX = 160.f;
    InfoDesc.fY = 100.f;
    InfoDesc.fSizeX = 900.f;
    InfoDesc.fSizeY = 80.f;

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_InventoryStatus_Info"), &InfoDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_StatusInfos.push_back(static_cast<CInventoryStatus_Info*>(pUIObject));

    return S_OK;
}


CInventory_Panel* CInventory_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory_Panel* pInstance = new CInventory_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventory_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CInventory_Panel::Clone(void* pArg)
{
    CInventory_Panel* pInstance = new CInventory_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventory_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CInventory_Panel::Destroy()
{
    CUIObject::Destroy();

}

void CInventory_Panel::Free()
{
    CUIObject::Free();
    m_SkillSlots.clear();
    m_ItemSlots.clear();
    m_StatusIcons.clear();
    m_StatusInfos.clear();
}
