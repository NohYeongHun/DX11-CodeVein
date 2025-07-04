#include "Title.h"

CTitle::CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CTitle::CTitle(const CTitle& Prototype)
    : CUIObject(Prototype )
{
}

HRESULT CTitle::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CTitle::Initialize(void* pArg)
{
    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = g_iWinSizeX >> 2;
    Desc.fSizeY = g_iWinSizeY >> 2;
   
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    
    m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));
   

    return S_OK;
}

void CTitle::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CTitle::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTitle::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CTitle::Render()
{
    __super::Begin();

    /*
    * 해당 위치의 월드 행렬에서 부모기준 위치 변경이 완료되어 있어야한다.
    * 밑의 Bind_Shader_Resources는 현재 Transform의 World 행렬을 곱해준다.
    * Transform의 월드 행렬에는 Parent의 월드행렬이 모두 곱해져 있어야 한다.
    */

    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_RenderMatrix)))
    //    return E_FAIL;
    //
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    //    return E_FAIL;
    //
    //if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
    //    return E_FAIL;
    //
    //m_pShaderCom->Begin(0);    
    //
    //m_pVIBufferCom->Bind_Resources();
    //
    //m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTitle::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Title"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTitle::Ready_Childs()
{
    // AddChild();

    CTitleText::TITLETEXT_DESC TitleTextDesc{};
    TitleTextDesc.fX = 0;
    TitleTextDesc.fY = 0;
    TitleTextDesc.fSizeX = 1280;
    TitleTextDesc.fSizeY = 280;
    

    CUIObject* pUIObject = nullptr;
    TitleTextDesc.iTextureIndex = 0;
    TitleTextDesc.fY = 100;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_TitleText"), &TitleTextDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);

    pUIObject = nullptr;
    TitleTextDesc.iTextureIndex = 3;
    TitleTextDesc.fY = 150;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_TitleText"), &TitleTextDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    

    return S_OK;
}

CTitle* CTitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTitle* pInstance = new CTitle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTitle::Clone(void* pArg)
{
    CTitle* pInstance = new CTitle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTitle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTitle::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
