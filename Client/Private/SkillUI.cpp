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

	for (auto& val : m_SkillUI_Panels)
		val->Set_Visibility(); // 모든 패널의 가시성을 변경합니다.


    if (m_IsVisibility) // 켜진 상태라면?
    {
        for (auto& val : m_SkillUI_Panels)
            val->Update_SelectedInfo(m_iSelect_PanelType, m_iSelect_PanelIndex, m_iSelect_SlotIndex);
    }
}


void CSkillUI::Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_SkillUI_Panels[iSkillPanel]->Change_Skill(iSkillSlot, strTextureTag, iTextureIndex);
}


HRESULT CSkillUI::Initialize_Prototype()
{
    CUIObject::Initialize_Prototype();
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

    if (FAILED(CUIObject::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    //if (FAILED(Ready_Skills()))
    //    return E_FAIL;



    return S_OK;
}

void CSkillUI::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    CUIObject::Priority_Update(fTimeDelta);
}

void CSkillUI::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (m_pGameInstance->Get_KeyUp(DIK_ESCAPE))
        Set_Visibility();

    CUIObject::Update(fTimeDelta);
}

void CSkillUI::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __sCUIObjectuper::Late_Update(fTimeDelta);
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
    Desc.fSizeX = 600;
    Desc.fSizeY = 800;
    Desc.ePanelType = CSkillUI_Panel::PANELTYPE::SKILL_UI;
    Desc.fSlot_SizeX = 80;
    Desc.fSlot_SizeY = 80;
    Desc.iSkillSlot = 8;
	Desc.pText = L"Prototype_Component_Texture_Action_SkillIcon";


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
    Desc.fY = -100.f;
    Desc.fSizeX = 600;
    Desc.fSizeY = 600;
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
#pragma region SKILL UI DISPLAY
    // Event 등록
    m_pGameInstance->Subscribe(EventType::SKILLINFO_DISPLAY, Get_ID(), [this](void* pData)
        {
            SKILLINFO_DISPLAY_DESC* pDesc = static_cast<SKILLINFO_DISPLAY_DESC*>(pData);
            m_iSelect_PanelIndex = pDesc->iPanelIndex;
            m_iSelect_PanelType = pDesc->iPanelType;
            m_iSelect_SlotIndex = pDesc->iSlotIndex;
            this->Set_Visibility();   // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::SKILLINFO_DISPLAY);

#pragma endregion

#pragma region SKILL_CHANGE

    m_pGameInstance->Subscribe(EventType::SKILLINFO_SKILL_CHANGE, Get_ID(), [this](void* pData)
        {
            SKILLINFO_SKILL_CHANGE_DESC* desc = static_cast<SKILLINFO_SKILL_CHANGE_DESC*>(pData);
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

HRESULT CSkillUI::Ready_Skills()
{
    SKILLINFO_SKILL_CHANGE_DESC Desc{};
    Desc.pText = TEXT("Action_SkillIcon");
    Desc.iSkillPanelIdx = PANEL::SKILLUI_PANEL;
    for (_uint i = 0; i < 8; i++)
    {
        Desc.iSlotIdx = i;
        Desc.iTextureIdx = i;
        m_pGameInstance->Publish(EventType::SKILLINFO_SKILL_CHANGE, &Desc);
    }

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
    CUIObject::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();
}

void CSkillUI::Free()
{
    CUIObject::Free();
}
