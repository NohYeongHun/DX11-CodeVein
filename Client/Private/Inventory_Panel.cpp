#include "Inventory_Panel.h"

CInventory_Panel::CInventory_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventory_Panel::CInventory_Panel(const CInventory_Panel& Prototype)
    : CUIObject(Prototype)
{
}

void CInventory_Panel::Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    if (m_ePanelType == SKILL_PANEL)
        m_SkillSlots[iSkillSlot]->Change_Skill(strTextureTag, iTextureIndex);
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
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CInventory_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CInventory_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CInventory_Panel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;
}

HRESULT CInventory_Panel::Render()
{
    /*wstring_convert<codecvt_utf8<wchar_t>> converter;
    string str = converter.to_bytes(m_strObjTag);*/

    if (m_ePanelType == ITEM_PANEL)
    {
        if (ImGui::IsWindowAppearing())              // 또는 static bool once=true;
        {
            ImGui::SetNextWindowPos({ 100, 100 }, ImGuiCond_Appearing);
            ImGui::SetNextWindowSize({ 460, 240 }, ImGuiCond_Appearing); // ← 원하는 픽셀
        }

        string str = "Inventory Panel[" + to_string(Get_ID()) + ']';
        ImGui::Begin(str.c_str());
        ImGui::InputFloat("FX", &m_fX);
        ImGui::InputFloat("FY", &m_fY);
        ImGui::End();
    }

    

    __super::Begin();

    __super::End();

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

    const _float posX[4] = { -fSizeX * 0.5f - 1.f, fSizeX * 0.5f + 1.f, -fSizeX * 0.5f - 1.f, fSizeX * 0.5f + 1.f };
    const _float posY[4] = { fSizeY * 0.5f - 1.f, fSizeY * 0.5f - 1.f , -fSizeY * 0.5f + 1.f, -fSizeY * 0.5f + 1.f };


    /*const _float posX[4] = { -fSizeX * 0.5f - 1.f, fSizeX * 0.5f + 1.f, -fSizeX * 0.5f - 1.f, fSizeX * 0.5f + 1.f };
    const _float posY[4] = { -fSizeY * 0.5f - 1.f, -fSizeY * 0.5f - 1.f , fSizeY * 0.5f + 1.f, fSizeY * 0.5f + 1.f };*/

    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iInventory_Slot; ++i)
    {
        Desc.fX = posX[i];
        Desc.fY = posY[i];

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

    const _float posX[3] = { -fSizeX * 0.5f - 1.f, 0.f, fSizeX * 0.5f };
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
    __super::Destroy();

}

void CInventory_Panel::Free()
{
    __super::Free();
}
