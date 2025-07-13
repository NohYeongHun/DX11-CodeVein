#include "Skill_Panel.h"

CLoading_Panel::CLoading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

CLoading_Panel::CLoading_Panel(const CLoading_Panel& Prototype)
    : CUIObject(Prototype)
    , m_iLoading_Slot { Prototype.m_iLoading_Slot }
{
}

HRESULT CLoading_Panel::Initialize_Prototype()
{
    m_iLoading_Slot = 9;
    return S_OK;
}

HRESULT CLoading_Panel::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    LOADING_PANEL_DESC* pDesc = static_cast<LOADING_PANEL_DESC*>(pArg);

    if (FAILED(Ready_Childs(pDesc)))
        return E_FAIL;

    // 대강 30프레임?
    m_fLoadingTime = m_pGameInstance->Get_TimeDelta(L"Timer_60") * 30.f;
    m_iCurrentIdx = 0;
    return S_OK;
}

void CLoading_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CLoading_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_fLoadingTime >= 0.f)
    {
        m_fLoadingTime -= fTimeDelta;
    }
    else
    {
        m_fLoadingTime = m_pGameInstance->Get_TimeDelta() * 30.f;
        m_LoadingSlots[m_iCurrentIdx]->Change_Shader_Pass(0);
        m_iCurrentIdx = (m_iCurrentIdx + 1) % 8;
        m_LoadingSlots[m_iCurrentIdx]->Change_Shader_Pass(1);
    }
}

void CLoading_Panel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CLoading_Panel::Render()
{

    return S_OK;
}



HRESULT CLoading_Panel::Ready_Components()
{

    return S_OK;
}

HRESULT CLoading_Panel::Ready_Childs(LOADING_PANEL_DESC* pDesc)
{
    // Panel 객체는 위치만 존재하고 Loading Slot을 소유하는 객체임.
    if (nullptr == pDesc)
        return E_FAIL;

    _float fSizeX = pDesc->fSlotSizeX;
    _float fSizeY = pDesc->fSlotSizeY;

    UIOBJECT_DESC Desc{};
    Desc.fX = 0;
    Desc.fY = 0;
    Desc.fSizeX = fSizeX;
    Desc.fSizeY = fSizeY;

    const _float dx = fSizeX * 0.5f + 7.f;   // 옆으로 한 칸
    const _float dy = fSizeY * 0.5f + 7.f;   // 위로 한 칸

    const _float posX[9] = {
     -2 * dx,      // 0 : 맨 왼쪽
     -dx,          // 1 : 왼쪽 위
      0.f,         // 2 : 꼭대기
      dx,          // 3 : 오른쪽 위
      2 * dx,      // 4 : 맨 오른쪽
      dx,          // 5 : 오른쪽 아래
      0.f,         // 6 : 맨 아래
     -dx,          // 7 : 왼쪽 아래
      0.f          // 8 : 한가운데 (마지막)
    };

    const _float posY[9] = {
         0.f,         // 0
         dy,          // 1
         2 * dy,      // 2
         dy,          // 3
         0.f,         // 4
        -dy,          // 5
        -2 * dy,      // 6
        -dy,          // 7
         0.f          // 8
    };

    CUIObject* pUIObject = nullptr;

    for (_uint i = 0; i < m_iLoading_Slot; ++i)
    {
        // 배치할 위치 지정.
        Desc.fX = posX[i];
        Desc.fY = posY[i];

        pUIObject = dynamic_cast<CUIObject*>(
            m_pGameInstance->Clone_Prototype(
                PROTOTYPE::GAMEOBJECT
                , ENUM_CLASS(LEVEL::LOADING)
                , TEXT("Prototype_GameObject_Loading_Slot"), &Desc));

        if (nullptr == pUIObject)
            return E_FAIL;

        AddChild(pUIObject);
        m_LoadingSlots.push_back(static_cast<CLoading_Slot*>(pUIObject));
    }

    

    return S_OK;
}

CLoading_Panel* CLoading_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_Panel* pInstance = new CLoading_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLoading_Panel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_Panel::Clone(void* pArg)
{
    CLoading_Panel* pInstance = new CLoading_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLoading_Panel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_Panel::Destroy()
{
    __super::Destroy();
}

void CLoading_Panel::Free()
{
    __super::Free();
}
