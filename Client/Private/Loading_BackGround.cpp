#include "Loading_BackGround.h"

CLoading_BackGround::CLoading_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CLoading_BackGround::CLoading_BackGround(const CLoading_BackGround& Prototype)
    : CUIObject(Prototype)
{
}

const _bool CLoading_BackGround::Get_Visibility()
{
    return m_IsVisibility;
}

void CLoading_BackGround::Set_Visibility(_bool IsVIsibility)
{
    m_IsVisibility = IsVIsibility;
}

HRESULT CLoading_BackGround::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CLoading_BackGround::Initialize_Clone(void* pArg)
{

    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = g_iWinSizeX;
    Desc.fSizeY = g_iWinSizeY;


    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    m_iTextureIndex = 0;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    
    

    return S_OK;
}

void CLoading_BackGround::Priority_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    __super::Priority_Update(fTimeDelta);
}

void CLoading_BackGround::Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;
    __super::Update(fTimeDelta);
}

void CLoading_BackGround::Late_Update(_float fTimeDelta)
{
    if (!m_IsVisibility)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

    __super::Late_Update(fTimeDelta);

    
}

HRESULT CLoading_BackGround::Render()
{
    if (!m_IsVisibility)
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

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();


    __super::End();

    return S_OK;
}

HRESULT CLoading_BackGround::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Loading_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoading_BackGround::Ready_Events()
{
#pragma region Loading Display

    // Event 등록
    m_pGameInstance->Subscribe(EventType::LOAIDNG_DISPLAY, this, nullptr, [this](void* pData)
        {
            LOADINGEVENT_DESC* desc = static_cast<LOADINGEVENT_DESC*>(pData);
            this->Set_Visibility(desc->isVisibility);   // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.push_back(EventType::LOAIDNG_DISPLAY);
#pragma endregion

    return S_OK;
}

HRESULT CLoading_BackGround::Ready_Childs()
{
    m_LoadingPanels.resize(1);

    // Panel 객체는 위치만 존재하고 Skill Slot을 소유하는 객체임.
    // 우측 아래에 패널 배치 예정.
    CLoading_Panel::LOADING_PANEL_DESC Desc{};
    Desc.fX = (g_iWinSizeX >> 1) - 100.f;
    Desc.fY = (g_iWinSizeY >> 1) * -1.f + 100.f;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
    Desc.fSlotSizeX = 30.f;
    Desc.fSlotSizeY = 30.f;

    CUIObject* pUIObject = nullptr;

    // Skill Panel1 추가 
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_Loading_Panel"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_LoadingPanels[0] = static_cast<CLoading_Panel*>(pUIObject);

    return S_OK;
}

CLoading_BackGround* CLoading_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_BackGround* pInstance = new CLoading_BackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLoading_BackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_BackGround::Clone(void* pArg)
{
    CLoading_BackGround* pInstance = new CLoading_BackGround(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLoading_BackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_BackGround::Destroy()
{
    __super::Destroy();

    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, this);
}


void CLoading_BackGround::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
