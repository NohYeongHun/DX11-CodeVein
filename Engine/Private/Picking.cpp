#include "Picking.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext}
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    D3D11_VIEWPORT vp{};
    UINT nVP = 1;
    m_pContext->RSGetViewports(&nVP, &m_ViewPort); // 반드시 필요한 부분
    m_Identity_Matrix = XMMatrixIdentity();
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());


    return S_OK;
}

void CPicking::Update()
{
    GetCursorPos(&m_PtMouse);
    ScreenToClient(m_hWnd, &m_PtMouse);

    // near, far Point in World Space
   
    _matrix worldMatrix = XMMatrixIdentity();

    m_ViewMatrix = *m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW);
	m_ProjMatrix = *m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ);
    
    m_Near_Point = { _float(m_PtMouse.x), _float(m_PtMouse.y), 0.f , 1.f};
    m_Far_Point = { _float(m_PtMouse.x), _float(m_PtMouse.y), 1.f, 1.f};
    
    _fvector vNearWorld = XMVector3Unproject(XMLoadFloat4(&m_Near_Point),
        m_ViewPort.TopLeftX, m_ViewPort.TopLeftY, m_ViewPort.Width, m_ViewPort.Height,
        m_ViewPort.MinDepth, m_ViewPort.MaxDepth,
        XMLoadFloat4x4(&m_ProjMatrix), XMLoadFloat4x4(&m_ViewMatrix)
        , worldMatrix);

    _fvector vFarWorld = XMVector3Unproject(XMLoadFloat4(&m_Far_Point),
        m_ViewPort.TopLeftX, m_ViewPort.TopLeftY, m_ViewPort.Width, m_ViewPort.Height,
        m_ViewPort.MinDepth, m_ViewPort.MaxDepth,
        XMLoadFloat4x4(&m_ProjMatrix), XMLoadFloat4x4(&m_ViewMatrix)
        , worldMatrix);


    // Ray 계산.
    XMStoreFloat3(&m_Ray_Origin, vNearWorld);
    XMStoreFloat3(&m_Ray_Dir, XMVector3Normalize(vFarWorld - vNearWorld));
 
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);
    if (FAILED(pInstance->Initialize(hWnd)))
    {
        MSG_BOX(TEXT("Failed to Created : CPicking"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    
}
