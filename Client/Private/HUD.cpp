#include "HUD.h"

CHUD::CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
    , m_IsVisibility(false)
{
}

CHUD::CHUD(const CHUD& Prototype)
    : CUIObject(Prototype)
    , m_IsVisibility(Prototype.m_IsVisibility)
{
}

const _bool CHUD::Get_Visibility()
{
    return m_IsVisibility;
}

void CHUD::Set_Visibility(_bool IsVIsibility)
{
    m_IsVisibility = IsVIsibility;
}

void CHUD::Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_SkillPanels[iSkillPanel]->Change_Skill(iSkillSlot, strTextureTag, iTextureIndex);
}

void CHUD::Execute_Skill(_uint iSkillPanel, _uint iSkillSlot, _float fSkillCoolTime)
{
    m_SkillPanels[iSkillPanel]->Execute_Skill(iSkillSlot, fSkillCoolTime);
}


HRESULT CHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_IsVisibility = false;
    return S_OK;
}

HRESULT CHUD::Initialize_Clone(void* pArg)
{
    // HUD는 크기가 없이 위치만.
    UIOBJECT_DESC               Desc{};
    // 정 중앙 위치
    Desc.fX = g_iWinSizeX >> 1; 
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;


    return S_OK;
}

void CHUD::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    __super::Priority_Update(fTimeDelta);
}

void CHUD::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    /*HUD_SKILLCHANGE_DESC Desc{};
    Desc.iSkillPanelIdx = SKILL_PANEL_TOP;
    Desc.pText = TEXT("Action_SkillIcon");
    
    if (m_pGameInstance->Get_KeyUp(DIK_1))
    {
        Desc.iSlotIdx = 0;
        Desc.iTextureIdx = 0;
        m_pGameInstance->Publish(EventType::HUD_SKILL_CHANGE, & Desc);
    }
    if (m_pGameInstance->Get_KeyUp(DIK_2))
    {
        Desc.iSlotIdx = 1;
        Desc.iTextureIdx = 1;
        m_pGameInstance->Publish(EventType::HUD_SKILL_CHANGE, &Desc);
    }
    if (m_pGameInstance->Get_KeyUp(DIK_3))
    {
        Desc.iSlotIdx = 2;
        Desc.iTextureIdx = 2;
        m_pGameInstance->Publish(EventType::HUD_SKILL_CHANGE, &Desc);
    }
    if (m_pGameInstance->Get_KeyUp(DIK_4))
    {
        Desc.iSlotIdx = 3;
        Desc.iTextureIdx = 3;
        m_pGameInstance->Publish(EventType::HUD_SKILL_CHANGE, &Desc);
    }*/

    // 마우스 왼쪽 클릭 시 쿨타임 돌게하기.
    if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::LB))
    {
        SKILLEXECUTE_DESC Desc{};
        Desc.iSkillPanelIdx = SKILL_PANEL_TOP;
        Desc.iSlotIdx = 1;
        Desc.fSkillCoolTime = 3.f;
        m_pGameInstance->Publish(EventType::SKILL_EXECUTE, &Desc);
    }

    if (m_pGameInstance->Get_KeyUp(DIK_5))
    {
        HPCHANGE_DESC HpDesc{};
        HpDesc.bIncrease = false;
        HpDesc.iHp = 50;
        HpDesc.fTime = 1.f;
        m_pGameInstance->Publish(EventType::HP_CHANGE, &HpDesc);
    }

    if (m_pGameInstance->Get_KeyUp(DIK_6))
    {
        HPCHANGE_DESC HpDesc{};
        HpDesc.bIncrease = true;
        HpDesc.iHp = 50;
        HpDesc.fTime = 1.f;
        m_pGameInstance->Publish(EventType::HP_CHANGE, &HpDesc);
    }

    if (m_pGameInstance->Get_KeyUp(DIK_7))
    {
        STEMINA_CHANGE_DESC SteminaDesc{};
        SteminaDesc.bIncrease = false;
        SteminaDesc.iStemina = 40;
        SteminaDesc.fTime = 1.f;
        m_pGameInstance->Publish(EventType::STEMINA_CHANGE, &SteminaDesc);
    }

    __super::Update(fTimeDelta);
}

void CHUD::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CHUD::Render()
{
    if (!m_IsVisibility)
        return S_OK;

    return S_OK;
}

HRESULT CHUD::Ready_Childs()
{
    // SKillPanel 준비.
    if (FAILED(Ready_SkillPanel()))
        return E_FAIL;

    // StatusPanel 준비
    if (FAILED(Ready_StatusPanel()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHUD::Ready_SkillPanel()
{
    m_SkillPanels.resize(SKILL_PANEL_END);

    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.
    CSkill_Panel::SKILLPANEL_DESC Desc{};
    Desc.fX = (g_iWinSizeX >> 1) - 200.f;
    Desc.fY = (g_iWinSizeY >> 1) * -1.f + 400.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.fSkillSizeX = 100.f;
    Desc.fSkillSizeY = 100.f;

    CUIObject* pUIObject = nullptr;

    // Skill Panel1 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillPanel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_SkillPanels[SKILL_PANEL_TOP] = static_cast<CSkill_Panel*>(pUIObject);

    // Skill Panel2 추가
    pUIObject = nullptr;

    Desc.fX = (g_iWinSizeX >> 1) - 300.f;
    Desc.fY = (g_iWinSizeY >> 1) * -1.f + 200.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.fSkillSizeX = 80.f;
    Desc.fSkillSizeY = 80.f;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillPanel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_SkillPanels[SKILL_PANEL_BOTTOM] = static_cast<CSkill_Panel*>(pUIObject);

    return S_OK;
}

// Status Panel 추가.
HRESULT CHUD::Ready_StatusPanel()
{
    m_StatusPanels.resize(STATUS_PANEL_END);
    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.
    UIOBJECT_DESC Desc{};
    Desc.fX = (g_iWinSizeX >> 1) * -1.f + 300.f;
    Desc.fY = (g_iWinSizeY >> 1) * -1.f + 200.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;

    CUIObject* pUIObject = nullptr;

    // Skill Panel1 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_StatusPanel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_StatusPanels[STATUS_PANEL_BAR] = static_cast<CStatusPanel*>(pUIObject);
    
    return S_OK;
}

HRESULT CHUD::Ready_Events()
{
#pragma region HUD DISPLAY

    // Event 등록
    m_pGameInstance->Subscribe(EventType::HUD_DISPLAY, Get_ID(), [this](void* pData)
        {
            HUDEVENT_DESC* desc = static_cast<HUDEVENT_DESC*>(pData);
            this->Set_Visibility(desc->isVisibility);   // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::HUD_DISPLAY);
#pragma endregion

#pragma region SKILL_CHANGE
   
    m_pGameInstance->Subscribe(EventType::HUD_SKILL_CHANGE, Get_ID(), [this](void* pData)
        {
            HUD_SKILLCHANGE_DESC* desc = static_cast<HUD_SKILLCHANGE_DESC*>(pData);
            this->Change_Skill(
                desc->iSkillPanelIdx,
                desc->iSlotIdx
            ,desc->pText
            , desc->iTextureIdx);  // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::HUD_SKILL_CHANGE);

#pragma endregion

#pragma region SKILL_EXECUTE
     m_pGameInstance->Subscribe(EventType::SKILL_EXECUTE, Get_ID(), [this](void* pData)
        {
            SKILLEXECUTE_DESC* desc = static_cast<SKILLEXECUTE_DESC*>(pData);
            this->Execute_Skill(
                desc->iSkillPanelIdx,
                desc->iSlotIdx, 
                desc->fSkillCoolTime);  // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::SKILL_EXECUTE);
#pragma endregion



    return S_OK;
}


CHUD* CHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHUD* pInstance = new CHUD(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CHUD"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CHUD::Clone(void* pArg)
{
    CHUD* pInstance = new CHUD(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CHUD"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHUD::Destroy()
{
    __super::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();
}

void CHUD::Free()
{
    __super::Free();
    m_StatusPanels.clear();
    m_SkillPanels.clear();
}
