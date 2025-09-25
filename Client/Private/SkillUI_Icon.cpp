CSkillUI_Icon::CSkillUI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkillUI_Icon::CSkillUI_Icon(const CSkillUI_Icon& Prototype)
    : CUIObject(Prototype)
{
}

void CSkillUI_Icon::Set_Visibility()
{
    m_IsVisibility = !m_IsVisibility;
}

void CSkillUI_Icon::Update_SelectedInfo(_uint iPanelType, _uint iPanelIndex, _uint iSlotIndex)
{
    m_iSelect_PanelType = iPanelType;
    m_iSelect_PanelIndex = iPanelIndex;
    m_iSelect_SlotIndex = iSlotIndex;
}

void CSkillUI_Icon::Change_Inventory_Skill()
{
    INVENTORY_SKILLCHANGE_DESC Desc{};
    Desc.iSkillPanelIdx = m_iSelect_PanelIndex;
    Desc.iSlotIdx = m_iSelect_SlotIndex;
    Desc.iTextureIdx = m_iTextureIndex;
    Desc.pSkillIcon = this;

    m_pGameInstance->Publish(EventType::INVENTORY_SKILL_CHANGE, &Desc);
}

void CSkillUI_Icon::Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;
    m_strTextureTag = strTextureTag;


    // 무슨 이름으로 넣을래?
    m_pGameInstance->Change_Texture_ToGameObject(this, TEXT("Com_Texture")
        , reinterpret_cast<CComponent**>(&m_pTextureCom), ENUM_CLASS(LEVEL::STATIC)
        , strTextureTag);
}


HRESULT CSkillUI_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillUI_Icon::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLICON_DESC* pDesc = static_cast<SKILLICON_DESC*>(pArg);
    m_strTextureTag = pDesc->pText;
    m_iTextureIndex = pDesc->iTextureIndex;
    
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_pTransformCom->Scale(_float3(0.8f, 0.8f, 1.f));

    /* Change Skill로 기본 Skill들 채워넣기*/


    return S_OK;
}

void CSkillUI_Icon::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Priority_Update(fTimeDelta);
}


void CSkillUI_Icon::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    CUIObject::Update(fTimeDelta);

    if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::LB))
    {
        if (Mouse_InRect2D(g_hWnd))
            Change_Inventory_Skill();
    }
}

void CSkillUI_Icon::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);
}

HRESULT CSkillUI_Icon::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;

    CUIObject::Begin();
    
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    CUIObject::End();

    return S_OK;
}

HRESULT CSkillUI_Icon::Ready_Components(SKILLICON_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Action_SkillIcon"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    // Texture Component는 Change Skill 이벤트로 넣는다.

    return S_OK;
}

HRESULT CSkillUI_Icon::Ready_Render_Resources()
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

CSkillUI_Icon* CSkillUI_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillUI_Icon* pInstance = new CSkillUI_Icon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSkillUI_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillUI_Icon::Clone(void* pArg)
{
    CSkillUI_Icon* pInstance = new CSkillUI_Icon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSkillUI_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillUI_Icon::Destroy()
{
    CUIObject::Destroy();
}

void CSkillUI_Icon::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
