#include "InventorySkill_Icon.h"
CInventorySkill_Icon::CInventorySkill_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CInventorySkill_Icon::CInventorySkill_Icon(const CInventorySkill_Icon& Prototype)
    : CUIObject(Prototype)
{
}

void CInventorySkill_Icon::Set_Visibility()
{
	m_IsVisibility = !m_IsVisibility;
}

void CInventorySkill_Icon::Change_Skill(CSkillUI_Icon* pSkillIcon, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;    


    Safe_Release(m_pTextureCom);
    m_pTextureCom = static_cast<CTexture*>(pSkillIcon->Get_Component(L"Com_Texture"));
    if (nullptr == m_pTextureCom)
        return;

    Safe_AddRef(m_pTextureCom);

    //// 무슨 이름으로 넣을래?
    /*m_pGameInstance->Change_Texture_ToGameObject(this, TEXT("Com_Texture")
        , reinterpret_cast<CComponent**>(&m_pTextureCom), ENUM_CLASS(LEVEL::STATIC)
        , strTextureTag);*/
}

HRESULT CInventorySkill_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventorySkill_Icon::Initialize_Clone(void* pArg)
{
    m_iTextureIndex = 0;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLICON_DESC* pDesc = static_cast<SKILLICON_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_pTransformCom->Scale(_float3(0.8f, 0.8f, 1.f));

    /* Change Skill로 기본 Skill들 채워넣기*/


    return S_OK;
}

void CInventorySkill_Icon::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Priority_Update(fTimeDelta);
}


void CInventorySkill_Icon::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Update(fTimeDelta);

   
}

void CInventorySkill_Icon::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CInventorySkill_Icon::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;

    __super::Begin();
    
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    __super::End();

    return S_OK;
}

HRESULT CInventorySkill_Icon::Ready_Components(SKILLICON_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    // Texture Component는 Change Skill 이벤트로 넣는다.

    return S_OK;
}

HRESULT CInventorySkill_Icon::Ready_Render_Resources()
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

CInventorySkill_Icon* CInventorySkill_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventorySkill_Icon* pInstance = new CInventorySkill_Icon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventorySkill_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventorySkill_Icon::Clone(void* pArg)
{
    CInventorySkill_Icon* pInstance = new CInventorySkill_Icon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventorySkill_Icon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventorySkill_Icon::Destroy()
{
    __super::Destroy();
}

void CInventorySkill_Icon::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
