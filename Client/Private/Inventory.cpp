#include "Inventory.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
    , m_IsVisibility(false)
{
}

CInventory::CInventory(const CInventory& Prototype)
    : CUIObject( Prototype )
    , m_IsVisibility(Prototype.m_IsVisibility)
{

}

const _bool CInventory::Get_Visibility()
{
    return m_IsVisibility;
}

void CInventory::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;

    HUDEVENT_DESC Desc{};
    
    if (!m_IsVisibility)
    {
        Desc.isVisibility = true;
        m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY, &Desc);
    }
    else
    {
        Desc.isVisibility = false;
        m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY, &Desc);
    }
        
}

void CInventory::Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_InventorySkill_Panels[iSkillPanel]->Change_Skill(iSkillSlot, strTextureTag, iTextureIndex);
}


HRESULT CInventory::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_IsVisibility = false;

    return S_OK;
}

HRESULT CInventory::Initialize_Clone(void* pArg)
{
    // Inventory 는 크기가 없이 위치만.
    UIOBJECT_DESC  
        Desc{};
    // 정 중앙 위치
    Desc.fX = (g_iWinSizeX >> 1) - 300.f;
    Desc.fY = (g_iWinSizeY >> 1) - 100.f;
    Desc.fSizeX = 600;
    Desc.fSizeY = 300;

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    if (FAILED(Ready_Skills()))
        return E_FAIL;
   


    return S_OK;
}

void CInventory::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    __super::Priority_Update(fTimeDelta);
}

void CInventory::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);
}

void CInventory::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CInventory::Render()
{

    __super::Begin();

    // Middle 이미지는 Inventory 자체에서 출력해도 무방.?
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    __super::End();


    return S_OK;
}

HRESULT CInventory::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Middle_Inventory"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInventory::Ready_Childs()
{
    // Status Panel 준비.
    if (FAILED(Ready_SkillPanel()))
        return E_FAIL;

    if (FAILED(Ready_ItemPanel()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInventory::Ready_SkillPanel()
{
    m_InventorySkill_Panels.resize(SKILL_PANEL::SKILL_PANEL_END);

    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.

#pragma region LEFT_PANEL
    CInventory_Panel::INVENTORY_PANEL_DESC Desc{};
    Desc.fX = -390.f;
    Desc.fY = -50.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.ePanelType = CInventory_Panel::PANELTYPE::SKILL_PANEL;
    Desc.fSlot_SizeX = 80;
    Desc.fSlot_SizeY = 80;
    Desc.iInventorySlot = 4;


    CUIObject* pUIObject = nullptr;
    // Skill Panel Left 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_Inventory_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_InventorySkill_Panels[SKILL_PANEL::SKILL_PANEL_LEFT] = static_cast<CInventory_Panel*>(pUIObject);
#pragma endregion

#pragma region RIGHTTOP_PANEL
    Desc.fX = 300.f;
    Desc.fY = 50.f;
    pUIObject = nullptr;
    // Skill Panel1 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_Inventory_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_InventorySkill_Panels[SKILL_PANEL::SKILL_PANEL_RIGHT_TOP] = static_cast<CInventory_Panel*>(pUIObject);
#pragma endregion

#pragma region RIGHTBOTTOM_PANEL
    Desc.fX = 300.f;
    Desc.fY = -150.f;
    pUIObject = nullptr;
    // Skill Panel1 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_Inventory_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_InventorySkill_Panels[SKILL_PANEL::SKILL_PANEL_RIGHT_BOTTOM] = static_cast<CInventory_Panel*>(pUIObject);
#pragma endregion

    return S_OK;
}

HRESULT CInventory::Ready_ItemPanel()
{

    m_InventoryItem_Panels.resize(ITEM_PANEL_END);

    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.

#pragma region LEFT_PANEL
    CInventory_Panel::INVENTORY_PANEL_DESC Desc{};
    Desc.fX = -250.f;
    Desc.fY = -200.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.ePanelType = CInventory_Panel::PANELTYPE::ITEM_PANEL;
    Desc.fSlot_SizeX = 80;
    Desc.fSlot_SizeY = 80;
    Desc.iInventorySlot = 3;


    CUIObject* pUIObject = nullptr;
    // Skill Panel Left 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_Inventory_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_InventoryItem_Panels[ITEM_PANEL::ITEM_PANEL_TOP] = static_cast<CInventory_Panel*>(pUIObject);
#pragma endregion

    return S_OK;
}

HRESULT CInventory::Ready_Events()
{
#pragma region INVENTORY DISPLAY
    // Event 등록
    m_pGameInstance->Subscribe(EventType::INVENTORY_DISPLAY, Get_ID(), [this](void* pData)
        {
            this->Set_Visibility();   // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::INVENTORY_DISPLAY);

#pragma endregion

#pragma region SKILL_CHANGE

    m_pGameInstance->Subscribe(EventType::INVENTORY_SKILL_CHANGE, Get_ID(), [this](void* pData)
        {
            INVENTORY_SKILLCHANGE_DESC* desc = static_cast<INVENTORY_SKILLCHANGE_DESC*>(pData);
            this->Change_Skill(
                desc->iSkillPanelIdx,
                desc->iSlotIdx
                , desc->pText
                , desc->iTextureIdx);  // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::INVENTORY_SKILL_CHANGE);

#pragma endregion

    return S_OK;
}

HRESULT CInventory::Ready_Skills()
{
    /*INVENTORY_SKILLCHANGE_DESC Desc{};
    Desc.pText = TEXT("Action_SkillIcon");
    Desc.iSkillPanelIdx = SKILL_PANEL_LEFT;
    Desc.iSlotIdx = 0;
    Desc.iTextureIdx = 0;
    m_pGameInstance->Publish(EventType::INVENTORY_SKILL_CHANGE, &Desc);*/

    /*Desc.iSlotIdx = 2;
    Desc.iTextureIdx = 1;
    m_pGameInstance->Publish(EventType::INVENTORY_SKILL_CHANGE, &Desc);*/

    return S_OK;
}

HRESULT CInventory::Ready_Render_Resources()
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

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory* pInstance = new CInventory(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
    CInventory* pInstance = new CInventory(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventory::Destroy()
{
    __super::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();
}

void CInventory::Free()
{
    __super::Free();

    m_InventoryItem_Panels.clear();


    m_InventorySkill_Panels.clear();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pTextureCom);
}
