#include "Loading_Slot.h"

CLoading_Slot::CLoading_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CLoading_Slot::CLoading_Slot(const CLoading_Slot& Prototype)
    : CUIObject(Prototype)
{
}

//void CLoading_Slot::Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex)
//{
//    m_pSkill->Change_Skill(strTextureTag, iTextureIndex);
//}
//
//void CLoading_Slot::Execute_Skill()
//{
//    if (nullptr == m_pSkill && m_IsCoolTime)
//        return;
//
//    // 스킬 실행.
//    m_IsCoolTime = true;
//    m_fTime = 0.f;
//
//    // 실제 스킬 실행 로직은?
//}

HRESULT CLoading_Slot::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CLoading_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iTextureIndex = 0;

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    return S_OK;
}

void CLoading_Slot::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}


void CLoading_Slot::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CLoading_Slot::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);

    
}

HRESULT CLoading_Slot::Render()
{
    __super::Begin();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_iTextureIndex", m_iTextureIndex)))
        return E_FAIL;


    //_float fFillRatio = 1.f;
    //if (m_IsCoolTime)                        // 쿨타임이면 0~1로 노말라이즈
    //{
    //    if (m_fCoolTime > 0.f)
    //        fFillRatio = Clamp(m_fTime / m_fCoolTime, 0.f, 1.f);
    //    else
    //        fFillRatio = 1.f;                // 방어 코드: 쿨타임 값이 0이면 그냥 다 찬 걸로
    //}

    //if (FAILED(m_pShaderCom->Bind_Float("g_fFillRatio", fFillRatio)))
    //    return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(m_iCurrentPass);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    __super::End();

    return S_OK;
}

HRESULT CLoading_Slot::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Loading_Slot"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}


CLoading_Slot* CLoading_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_Slot* pInstance = new CLoading_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_Slot::Clone(void* pArg)
{
    CLoading_Slot* pInstance = new CLoading_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Slot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_Slot::Destroy()
{
    __super::Destroy();
}

void CLoading_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pTextureCom);
}
