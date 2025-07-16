#include "SkillUI_Panel.h"

CSkillUI_Panel::CSkillUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkillUI_Panel::CSkillUI_Panel(const CSkillUI_Panel& Prototype)
    : CUIObject(Prototype)
{
}

void CSkillUI_Panel::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;

    for (auto& val : m_SkillSlots)
        val->Set_Visibility();
}

void CSkillUI_Panel::Update_SelectedInfo(_uint iPanelType, _uint iPanelIndex, _uint iSlotIndex)
{
    m_iSelect_PanelType = iPanelType;
    m_iSelect_PanelIndex = iPanelIndex;
    m_iSelect_SlotIndex = iSlotIndex;

    for (auto& val : m_SkillSlots)
        val->Update_SelectedInfo(m_iSelect_PanelType, m_iSelect_PanelIndex, m_iSelect_SlotIndex);
    
}


void CSkillUI_Panel::Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex)
{
    if (m_ePanelType == PANELTYPE::SKILL_UI)
        m_SkillSlots[iSkillSlot]->Change_Skill(strTextureTag, iTextureIndex);
}

HRESULT CSkillUI_Panel::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CSkillUI_Panel::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    SKILLUI_PANEL_DESC* pDesc = static_cast<SKILLUI_PANEL_DESC*>(pArg);

     m_ePanelType = pDesc->ePanelType;
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CSkillUI_Panel::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Priority_Update(fTimeDelta);
}

void CSkillUI_Panel::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);
}

void CSkillUI_Panel::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;


    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CSkillUI_Panel::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;

    __super::Begin();

    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;


    m_pShaderCom->Begin(6);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    __super::End();

    return S_OK;
}

HRESULT CSkillUI_Panel::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkillUI_Panel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;


    return S_OK;
}


HRESULT CSkillUI_Panel::Ready_Childs(SKILLUI_PANEL_DESC* pDesc)
{
    HRESULT hr = {};
    switch (pDesc->ePanelType)
    {
    case PANELTYPE::SKILL_UI:
        hr = Ready_Skill_Childs(pDesc);
        break;
    case PANELTYPE::SKILL_INFO:
        hr = Ready_Item_Childs(pDesc);
        break;
    
    }

    if (FAILED(hr))
        return E_FAIL;

    return S_OK;

}

HRESULT CSkillUI_Panel::Ready_Skill_Childs(SKILLUI_PANEL_DESC* pDesc)
{
    _float fSizeX = pDesc->fSlot_SizeX;
    _float fSizeY = pDesc->fSlot_SizeY;
    
    m_iInventory_Slot = pDesc->iSkillSlot;

    CSkillUI_Slot::SKILLSLOT_DESC Desc{};
    Desc.fX = -200.f;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;



    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < 8; ++i)
    {
        Desc.fX = -200.f + fSizeX * (i % 4) + 10.f * (i % 4);
        Desc.fY = 300.f - 200.f * (i / 4);
        Desc.iTextureIndex = i;
        Desc.iSlotIndex = i;
        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::STATIC)
                , TEXT("Prototype_GameObject_SkillUI_Slot"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_SkillSlots.push_back(static_cast<CSkillUI_Slot*>(pUIObject));
    }

    return S_OK;
}

HRESULT CSkillUI_Panel::Ready_Item_Childs(SKILLUI_PANEL_DESC* pDesc)
{
    return S_OK;
}

HRESULT CSkillUI_Panel::Ready_Render_Resources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}




CSkillUI_Panel* CSkillUI_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillUI_Panel* pInstance = new CSkillUI_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSkillUI_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkillUI_Panel::Clone(void* pArg)
{
    CSkillUI_Panel* pInstance = new CSkillUI_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSkillUI_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkillUI_Panel::Destroy()
{
    __super::Destroy();

}

void CSkillUI_Panel::Free()
{
    __super::Free();
    m_SkillSlots.clear();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);

}
