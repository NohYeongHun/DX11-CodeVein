#include "SkillUI.h"

CSkillUI::CSkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
    , m_IsVisibility(false)
{
}

CSkillUI::CSkillUI(const CSkillUI& Prototype)
    : CUIObject( Prototype )
    , m_IsVisibility(Prototype.m_IsVisibility)
{

}

const _bool CSkillUI::Get_Visibility()
{
    return m_IsVisibility;
}

void CSkillUI::Set_Visibility()
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

void CSkillUI::Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    //m_InventorySkill_Panels[iSkillPanel]->Change_Skill(iSkillSlot, strTextureTag, iTextureIndex);


    // 동시에 HUD에도 동기화를 해주어야합니다.
    HUD_SKILLCHANGE_DESC Desc{};
    Desc.iSkillPanelIdx = iSkillPanel;
    Desc.pText = TEXT("Action_SkillIcon");
    Desc.iSlotIdx = iSkillSlot;
    Desc.iTextureIdx = iTextureIndex;

    m_pGameInstance->Publish<HUD_SKILLCHANGE_DESC>(EventType::HUD_SKILL_CHANGE, &Desc);
}


HRESULT CSkillUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_IsVisibility = false;

    return S_OK;
}

HRESULT CSkillUI::Initialize_Clone(void* pArg)
{

    UIOBJECT_DESC  Desc{};
    // 정 중앙 위치
    Desc.fX = (g_iWinSizeX >> 1);
    Desc.fY = (g_iWinSizeY >> 1);
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    if (FAILED(Ready_Skills()))
        return E_FAIL;



    return S_OK;
}

void CSkillUI::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    __super::Priority_Update(fTimeDelta);
}

void CSkillUI::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);
}

void CSkillUI::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CSkillUI::Render()
{

    return S_OK;
}

HRESULT CSkillUI::Ready_Childs()
{
    // Status Panel 준비.
    m_SkillUI_Panels.resize(PANEL::PANEL_END);

#pragma region LEFT_PANEL( SKILL UI)
    CSkillUI_Panel::SKILLUI_PANEL_DESC Desc{};
    Desc.fX = -390.f;
    Desc.fY = 0.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.ePanelType = CSkillUI_Panel::PANELTYPE::SKILL_UI;
    Desc.fSlot_SizeX = 80;
    Desc.fSlot_SizeY = 80;
    Desc.iSkillSlot = 8;


    CUIObject* pUIObject = nullptr;
    // Skill Panel Left 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillUI_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_SkillUI_Panels[PANEL::SKILLUI_PANEL] = static_cast<CSkillUI_Panel*>(pUIObject);
#pragma endregion


#pragma region RIGHT_PANEL( SKILL INFO)
    Desc.fX = 390.f;
    Desc.fY = 0.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.ePanelType = CSkillUI_Panel::PANELTYPE::SKILL_INFO;
    Desc.fSlot_SizeX = 80;
    Desc.fSlot_SizeY = 80;
    Desc.iSkillSlot = 0;


    pUIObject = nullptr;
    // Skill Panel Left 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillUI_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_SkillUI_Panels[PANEL::SKILLINFO_PANEL] = static_cast<CSkillUI_Panel*>(pUIObject);
#pragma endregion


    return S_OK;
}



HRESULT CSkillUI::Ready_Events()
{
#pragma region INVENTORY DISPLAY
    //// Event 등록
    //m_pGameInstance->Subscribe(EventType::INVENTORY_DISPLAY, Get_ID(), [this](void* pData)
    //    {
    //        this->Set_Visibility();   // 멤버 함수 호출
    //    });

    //// Event 목록 관리.
    //m_Events.push_back(EventType::INVENTORY_DISPLAY);

#pragma endregion

#pragma region SKILL_CHANGE

    //m_pGameInstance->Subscribe(EventType::INVENTORY_SKILL_CHANGE, Get_ID(), [this](void* pData)
    //    {
    //        INVENTORY_SKILLCHANGE_DESC* desc = static_cast<INVENTORY_SKILLCHANGE_DESC*>(pData);
    //        this->Change_Skill(
    //            desc->iSkillPanelIdx,
    //            desc->iSlotIdx
    //            , desc->pText
    //            , desc->iTextureIdx);  // 멤버 함수 호출
    //    });

    //// Event 목록 관리.
    //m_Events.push_back(EventType::INVENTORY_SKILL_CHANGE);

#pragma endregion

    return S_OK;
}

HRESULT CSkillUI::Ready_Skills()
{
    /*INVENTORY_SKILLCHANGE_DESC Desc{};
    Desc.pText = TEXT("Action_SkillIcon");
    Desc.iSkillPanelIdx = SKILL_PANEL_LEFT;
    Desc.iSlotIdx = 0;
    Desc.iTextureIdx = 0;
    m_pGameInstance->Publish(EventType::INVENTORY_SKILL_CHANGE, &Desc);

    Desc.iSlotIdx = 2;
    Desc.iTextureIdx = 1;
    m_pGameInstance->Publish(EventType::INVENTORY_SKILL_CHANGE, &Desc);*/

    return S_OK;
}


CSkillUI* CSkillUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillUI* pInstance = new CSkillUI(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSKillUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillUI::Clone(void* pArg)
{
    CSkillUI* pInstance = new CSkillUI(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSkillUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillUI::Destroy()
{
    __super::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();
}

void CSkillUI::Free()
{
    __super::Free();
}
