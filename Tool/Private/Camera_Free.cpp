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

	if (FAILED(CCamera::Initialize_Clone(pArg)))
		return E_FAIL;
	
	m_fYaw = {};
	m_fPitch = {};
	_vector qFinalRotation = XMQuaternionRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);
	// 4. Transform에 설정
	m_pTransformCom->Set_Quaternion(qFinalRotation);

	return S_OK;
}

// 카메라에 대한 이동 전환은 Priority_Update에서 완료 후 Pipe Line에 행렬이 전달됨.
void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	CCamera::Priority_Update(fTimeDelta);


}

void CCamera_Free::Update(_float fTimeDelta)
{
	CCamera::Update(fTimeDelta);

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
	if (m_pGameInstance->Get_KeyPress(DIK_UPARROW))
	{
		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_pTransformCom->Move_Direction(vDir, fTimeDelta);
	}
	if (m_pGameInstance->Get_KeyPress(DIK_DOWNARROW))
	{
		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_pTransformCom->Move_Direction(vDir, fTimeDelta * -1.f);
	}

	//Handle_Mouse_Rotation(fTimeDelta);


	if (m_pGameInstance->Get_KeyPress(DIK_Q))
	{
		_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X);
		_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y);
		if (MouseMoveX != 0 || MouseMoveY != 0)
		{
			// 1. 마우스 움직임을 각도로 변환하여 누적
			m_fYaw += (_float)MouseMoveX * fTimeDelta * m_fMouseSensor;
			m_fPitch += (_float)MouseMoveY * fTimeDelta * m_fMouseSensor;

			// 2. Pitch 제한
			m_fPitch = max(-XM_PIDIV2 * 0.9f, min(XM_PIDIV2 * 0.9f, m_fPitch));

			// 3. 올바른 함수 사용 - XMQuaternionRotationRollPitchYaw
			_vector qFinalRotation = XMQuaternionRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);

			// 4. Transform에 설정
			m_pTransformCom->Set_Quaternion(qFinalRotation);
		}
	}

	/* 값이 일정이상 튀지 않게 관리. */
	while (m_fYaw > XM_PI) m_fYaw -= XM_PI;
	while (m_fPitch > XM_PI * 0.5f) m_fPitch -= XM_PI * 0.5f;
	while (m_fYaw < -XM_PI) m_fYaw += XM_PI;
	while (m_fPitch < XM_PI * -0.5f) m_fPitch += XM_PI * 0.5f;

	CCamera::Update_PipeLines();
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
	// 여기서 마우스 이전 프레임 위치 저장.

	CCamera::Late_Update(fTimeDelta);
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
	CCamera::Free();
}

