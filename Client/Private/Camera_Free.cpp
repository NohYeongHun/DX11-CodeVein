#include "Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera(Prototype)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize_Clone(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;


	return S_OK;
}

// 카메라에 대한 이동 전환은 Priority_Update에서 완료 후 Pipe Line에 행렬이 전달됨.
void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_pGameInstance->Get_KeyPress(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Get_KeyPress(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Get_KeyPress(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Get_KeyPress(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}


	if (m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::LB))
	{
		_long		MouseMove = {};
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	}


	__super::Update_PipeLines();
}

void CCamera_Free::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
	// 여기서 마우스 이전 프레임 위치 저장.
	
	__super::Late_Update(fTimeDelta);
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}

