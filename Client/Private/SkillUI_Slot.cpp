#include "SkillUI_Slot.h"
CSkillUI_Slot::CSkillUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkillUI_Slot::CSkillUI_Slot(const CSkillUI_Slot& Prototype)
    : CUIObject(Prototype)
{
}

void CSkillUI_Slot::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;
    if (nullptr != m_pSkill)
    {
        m_pSkill->Set_Visibility();
    }
        
}


void CSkillUI_Slot::Update_SelectedInfo(_uint iPanelType, _uint iPanelIndex, _uint iSlotIndex)
{
    m_iSelect_PanelType = iPanelType;
    m_iSelect_PanelIndex = iPanelIndex;
    m_iSelect_SlotIndex = iSlotIndex;

    if (nullptr != m_pSkill)
        m_pSkill->Update_SelectedInfo(m_iSelect_PanelType, m_iSelect_PanelIndex, m_iSelect_SlotIndex);
}

void CSkillUI_Slot::Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_strTextureTag = strTextureTag;
    m_iIcon_TextureIndex = iTextureIndex;
    m_pSkill->Change_Skill(strTextureTag, iTextureIndex);
    
}


HRESULT CSkillUI_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillUI_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    
    SKILLSLOT_DESC* pDesc = static_cast<SKILLSLOT_DESC*>(pArg);
    m_iIcon_TextureIndex = pDesc->iTextureIndex;
    m_iSlotIndex = pDesc->iSlotIndex;
    m_strTextureTag = pDesc->pText;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    return S_OK;
}

void CSkillUI_Slot::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Priority_Update(fTimeDelta);
}


void CSkillUI_Slot::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Update(fTimeDelta);
}

void CSkillUI_Slot::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);

}

HRESULT CSkillUI_Slot::Render()
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

HRESULT CSkillUI_Slot::Ready_Childs()
{

    CSkillUI_Icon::SKILLICON_DESC Desc{};
    // 정중앙 위치
    Desc.fX = 0;
    Desc.fY = 0; 
    Desc.fSizeX = m_fSizeX;
    Desc.fSizeY = m_fSizeY;
    Desc.iTextureIndex = m_iIcon_TextureIndex;
    Desc.pText = m_strTextureTag.c_str();
    
    CUIObject* pUIObject = nullptr;
    
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SkillUI_Icon"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pSkill = static_cast<CSkillUI_Icon*>(pUIObject);

    return S_OK;
}

HRESULT CSkillUI_Slot::Ready_Components(SKILLSLOT_DESC* pDesc)
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

HRESULT CSkillUI_Slot::Ready_Render_Resources()
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


CSkillUI_Slot* CSkillUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillUI_Slot* pInstance = new CSkillUI_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillUI_Slot::Clone(void* pArg)
{
    CSkillUI_Slot* pInstance = new CSkillUI_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventoryItem_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillUI_Slot::Destroy()
{
    CUIObject::Destroy();
}

void CSkillUI_Slot::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
