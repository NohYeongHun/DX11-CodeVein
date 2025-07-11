#include "StatusPanel.h"

CStatusPanel::CStatusPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CStatusPanel::CStatusPanel(const CStatusPanel& Prototype)
    : CUIObject(Prototype)
{
}

void CStatusPanel::Increase_Hp(_uint iHp, _float fTime)
{
    m_pHpBar->Increase_Hp(iHp, fTime);
}

void CStatusPanel::Decrease_Hp(_uint iHp, _float fTime)
{
    m_pHpBar->Decrease_Hp(iHp, fTime);
}

void CStatusPanel::Increase_Stemina(_uint iStemina, _float fTime)
{
    m_pSteminaBar->Increase_Stemina(iStemina, fTime);
}

void CStatusPanel::Decrease_Stemina(_uint iStemina, _float fTime)
{
    m_pSteminaBar->Decrease_Stemina(iStemina, fTime);
}

HRESULT CStatusPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStatusPanel::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;


    return S_OK;
}

void CStatusPanel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CStatusPanel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CStatusPanel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CStatusPanel::Render()
{
    return S_OK;
}



HRESULT CStatusPanel::Ready_Components()
{
    return S_OK;
}

HRESULT CStatusPanel::Ready_Childs()
{
    UIOBJECT_DESC Desc{};
    // 정중앙 위치
    Desc.fX = 0;
    Desc.fY = -100.f;
    Desc.fSizeX = 400.f;
    Desc.fSizeY = 40.f;

    CUIObject* pUIObject = nullptr;

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_HPBar"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pHpBar = static_cast<CHPBar*>(pUIObject);

    // 정중앙 위치
    Desc.fX = 0;
    Desc.fY = -50.f;
    Desc.fSizeX = 400.f;
    Desc.fSizeY = 20.f;

    pUIObject = nullptr;

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::STATIC)
            , TEXT("Prototype_GameObject_SteminaBar"), &Desc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_pSteminaBar = static_cast<CSteminaBar*>(pUIObject);

    return S_OK;
}

CStatusPanel* CStatusPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStatusPanel* pInstance = new CStatusPanel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CStatusPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStatusPanel::Clone(void* pArg)
{
    CStatusPanel* pInstance = new CStatusPanel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CStatusPanel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatusPanel::Destroy()
{
    __super::Destroy();
}

void CStatusPanel::Free()
{
    __super::Free();
}
