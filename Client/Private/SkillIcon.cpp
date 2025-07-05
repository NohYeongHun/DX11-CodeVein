#include "SkillIcon.h"

CSkillIcon::CSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CSkillIcon::CSkillIcon(const CSkillIcon& Prototype)
    : CUIObject(Prototype)
{
}

void CSkillIcon::Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;

    // 무슨 이름으로 넣을래?
    m_pGameInstance->Change_Texture_ToGameObject(this, TEXT("Com_Texture")
        , reinterpret_cast<CComponent**>(&m_pTextureCom), ENUM_CLASS(LEVEL::STATIC)
        , strTextureTag);
}

HRESULT CSkillIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillIcon::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    SKILLICON_DESC* pDesc = static_cast<SKILLICON_DESC*>(pArg);
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    return S_OK;
}

void CSkillIcon::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}


void CSkillIcon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkillIcon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CSkillIcon::Render()
{
    // Texture가 없을 수도 있음.
    if (m_pTextureCom == nullptr)
        return S_OK;

    __super::Begin();

    

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_iTextureIndex", m_iTextureIndex)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    __super::End();

    return S_OK;
}

HRESULT CSkillIcon::Ready_Components(SKILLICON_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CSkillIcon* CSkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillIcon* pInstance = new CSkillIcon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSkillSlot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillIcon::Clone(void* pArg)
{
    CSkillIcon* pInstance = new CSkillIcon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSkillSlot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillIcon::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
