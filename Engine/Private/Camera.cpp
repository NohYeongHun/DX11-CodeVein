#include "Camera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext}
{
}

CCamera::CCamera(const CCamera& Prototype)
    : CGameObject(Prototype)
    , m_fFovy{ Prototype.m_fFovy }
    , m_fAspect { Prototype.m_fAspect }
    , m_fNear{ Prototype.m_fNear }
    , m_fFar { Prototype.m_fFar }
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize_Clone(void* pArg)
{
    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    // 1. Transform Component에 카메라의 위치 (Eye) 설정
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEye));

    // 2. Transform Component에 카메라의 바라보는 방향 (vAt) 설정.
    m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));


    // 3. ViewPort 정보 가져와서 설정.
    _uint iNumViewPorts{ 1 };
    D3D11_VIEWPORT ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

    m_fFovy = pDesc->fFovy;
    m_fAspect = ViewportDesc.Width / ViewportDesc.Height;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;
    
    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

// PipeLine의 행렬들을 채워줍니다.
void CCamera::Update_PipeLines()
{
    m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
    m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}


void CCamera::Free()
{
    __super::Free();
}
