#include "Skill_Icon.h"

CSkill_Icon::CSkill_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkill_Icon::CSkill_Icon(const CSkill_Icon& Prototype)
    : CUIObject(Prototype)
{
}

void CSkill_Icon::Change_Skill(CSkillUI_Icon* pSkillIcon, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;

    Safe_Release(m_pTextureCom);
    m_pTextureCom = static_cast<CTexture*>(pSkillIcon->Get_Component(L"Com_Texture"));
    if (nullptr == m_pTextureCom)
        return;

    Safe_AddRef(m_pTextureCom);
}

HRESULT CSkill_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Icon::Initialize_Clone(void* pArg)
{
    if (FAILED(CUIObject::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLICON_DESC* pDesc = static_cast<SKILLICON_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_pTransformCom->Scale(_float3(0.7f, 0.7f, 1.f));


    return S_OK;
}

void CSkill_Icon::Priority_Update(_float fTimeDelta)
{
    CUIObject::Priority_Update(fTimeDelta);
}


void CSkill_Icon::Update(_float fTimeDelta)
{
    CUIObject::Update(fTimeDelta);
}

void CSkill_Icon::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    CUIObject::Late_Update(fTimeDelta);
}

HRESULT CSkill_Icon::Render()
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

HRESULT CSkill_Icon::Ready_Components(SKILLICON_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkill_Icon::Ready_Render_Resources()
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

CSkill_Icon* CSkill_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Icon* pInstance = new CSkill_Icon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkill_Icon::Clone(void* pArg)
{
    CSkill_Icon* pInstance = new CSkill_Icon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkill_Icon::Destroy()
{
    CUIObject::Destroy();
}

void CSkill_Icon::Free()
{
    CUIObject::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
