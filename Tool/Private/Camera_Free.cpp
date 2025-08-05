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


}

void CCamera_Free::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

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

	//Handle_Mouse_Rotation(fTimeDelta);


	if (m_pGameInstance->Get_KeyPress(DIK_Q))
	{
		_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X);
		_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y);


		if (MouseMoveY != 0)
		{
			// 카메라 기준 Pitch 회전 (자기 기준 오른쪽 축)
			_float fAngle = fTimeDelta * MouseMoveY * m_fMouseSensor;
			_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
			m_pTransformCom->Turn(vRight, fAngle);
		}
		if (MouseMoveX != 0)
		{

			// Y축(Yaw 회전)
			_float fAngle = fTimeDelta * MouseMoveX * m_fMouseSensor;
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
		}
		
	}


	__super::Update_PipeLines();
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

void CCamera_Free::Handle_Mouse_Rotation(_float fTimeDelta)
{
	// 마우스 우클릭 또는 Q키로 회전 모드 활성화
	if (m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::RB) ||
		m_pGameInstance->Get_KeyPress(DIK_Q))
	{
		// 마우스 이동량 가져오기 (구문 오류 수정)
		_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X);
		_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y);

		// 클라이언트 영역 내에서만 회전 처리 (선택사항)
		POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
		RECT rcClient;
		GetClientRect(g_hWnd, &rcClient);

		if (PtInRect(&rcClient, ptMouse))
		{
			// Y축 마우스 이동 → X축 회전 (Pitch)
			if (MouseMoveY != 0)
			{
				_float fPitchAngle = (_float)MouseMoveY * m_fMouseSensor * fTimeDelta;

				// Pitch 제한 (상하 90도 제한)
				m_fCurrentPitch += fPitchAngle;
				m_fCurrentPitch = max(-XM_PIDIV2 + 0.1f, min(XM_PIDIV2 - 0.1f, m_fCurrentPitch));

				_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
				m_pTransformCom->Turn(vRight, fPitchAngle);
			}

			// X축 마우스 이동 → Y축 회전 (Yaw)
			if (MouseMoveX != 0)
			{
				_float fYawAngle = (_float)MouseMoveX * m_fMouseSensor * fTimeDelta;

				// 부드러운 회전을 위한 보간 (선택사항)
				m_fTargetYaw += fYawAngle;

				// 즉시 회전 (기본)
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYawAngle);
			}

			
		}
	}
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

