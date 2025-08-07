#include "Camera_Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& Prototype)
	: CCamera(Prototype)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize_Clone(void* pArg)
{
	CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor * 1.5f;
	m_pTarget = pDesc->pTarget;

	// 화면 크기 가져오기
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	// 타겟 기준 뒤에서 바라보는 오프셋 (플레이어 뒤쪽 5미터, 위쪽 3미터)
	XMStoreFloat4(&m_vTargetOffset, XMVectorSet(0.f, 2.f, -6.f, 0.f));
	m_vOriginalOffset = m_vTargetOffset;

	// 초기 Yaw 각도 (0도 = 플레이어 정면을 바라봄)
	m_fYaw = 0.f;

	// 줌인 시 오프셋 (더 가까이, 살짝 위에서)
	XMStoreFloat4(&m_vZoomTargetOffset, XMVectorSet(0.f, 2.2f, -5.f, 0.f));

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	
}

void CCamera_Player::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	// 줌 업데이트 (마우스 처리 전에)
	Update_Zoom(fTimeDelta);

	//// ✨ LockOn 모드에 따른 카메라 업데이트 분기
	//if (m_bLockOnMode && m_pLockOnTarget)
	//{
	//	Update_LockOn_Camera(fTimeDelta);
	//}
	//else
	//{
	//	Update_Normal_Camera(fTimeDelta);
	//}


	//Update_Normal_Camera(fTimeDelta);
	Update_Chase_Target(fTimeDelta);



	// 6. 파이프라인 업데이트
	__super::Update_PipeLines();
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}


// Getter 함수들 (필요시 추가)
_float CCamera_Player::Get_Yaw() const
{
	return m_fYaw;
}

void CCamera_Player::Set_Yaw(_float fYaw)
{
	m_fYaw = fYaw;
}

void CCamera_Player::Add_Yaw(_float fYawDelta)
{
	m_fYaw += fYawDelta;
}

void CCamera_Player::Set_TargetOffset(_float4 vOffset)
{
	m_vTargetOffset = vOffset;
}

void CCamera_Player::Start_Zoom_In(_float fZoomDuration)
{
	if (m_bIsZooming && m_bZoomIn) return; // 이미 줌인 중이면 무시

	m_bIsZooming = true;
	m_bZoomIn = true;
	m_fZoomLerpTime = 0.f;
	m_fZoomMaxTime = fZoomDuration;
}

void CCamera_Player::Start_Zoom_Out(_float fZoomDuration)
{
	if (m_bIsZooming && !m_bZoomIn) return; // 이미 줌아웃 중이면 무시

	m_bIsZooming = true;
	m_bZoomIn = false;
	m_fZoomLerpTime = 0.f;
	m_fZoomMaxTime = fZoomDuration;
}

void CCamera_Player::Update_Zoom(_float fTimeDelta)
{
	if (!m_bIsZooming) return;

	m_fZoomLerpTime += fTimeDelta;
	_float fLerpRatio = m_fZoomLerpTime / m_fZoomMaxTime;

	if (fLerpRatio >= 1.f)
	{
		fLerpRatio = 1.f;
		m_bIsZooming = false;
	}

	// 부드러운 보간을 위한 easing 함수 적용 (선택사항)
	fLerpRatio = fLerpRatio * fLerpRatio * (3.f - 2.f * fLerpRatio); 

	_vector vCurrentOffset, vTargetOffset;

	if (m_bZoomIn)
	{
		// 줌인: 원래 오프셋 -> 줌 오프셋
		vCurrentOffset = XMLoadFloat4(&m_vOriginalOffset);
		vTargetOffset = XMLoadFloat4(&m_vZoomTargetOffset);
	}
	else
	{
		// 줌아웃: 줌 오프셋 -> 원래 오프셋
		vCurrentOffset = XMLoadFloat4(&m_vZoomTargetOffset);
		vTargetOffset = XMLoadFloat4(&m_vOriginalOffset);
	}

	// 현재 오프셋으로부터 TargetOffset으로 자연스럽게 보간.
	_vector vLerpedOffset = XMVectorLerp(vCurrentOffset, vTargetOffset, fLerpRatio);
	XMStoreFloat4(&m_vTargetOffset, vLerpedOffset);
}

void CCamera_Player::Reset_Zoom()
{
	m_bIsZooming = false;
	m_fZoomLerpTime = 0.f;
	m_vTargetOffset = m_vOriginalOffset;
}


void CCamera_Player::Set_LockOn_Target(CGameObject* pTarget)
{
	m_pLockOnTarget = pTarget;

	if (pTarget)
	{
		Enable_LockOn_Mode();
	}
	else
	{
		Disable_LockOn_Mode();
	}
}

void CCamera_Player::Clear_LockOn_Target()
{
	m_pLockOnTarget = nullptr;
	Disable_LockOn_Mode();
}

void CCamera_Player::Enable_LockOn_Mode()
{
	m_bLockOnMode = true;

	// LockOn 모드 시작 시 현재 Yaw 각도를 LockOn Yaw로 설정
	m_fLockOnYaw = m_fYaw;

	// LockOn 카메라 위치 초기화
	if (m_pTarget && m_pLockOnTarget)
	{
		Calculate_LockOn_Camera_Position(0.0f);
		m_vCurrentCameraPos = m_vLockOnCameraPos;
	}
}

void CCamera_Player::Disable_LockOn_Mode()
{
	m_bLockOnMode = false;

	// 일반 모드로 돌아갈 때 현재 LockOn Yaw를 일반 Yaw로 설정
	m_fYaw = m_fLockOnYaw;
}

void CCamera_Player::Update_LockOn_Camera(_float fTimeDelta)
{
	if (!m_pTarget || !m_pLockOnTarget)
		return;

	// 마우스 입력으로 카메라 회전 (LockOn 중에도 약간의 조작 가능)
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	if (PtInRect(&rcClient, ptMouse))
	{
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMove * m_fMouseSensor * 0.3f * fTimeDelta; // 일반보다 느리게
			//_float fAngle = (_float)MouseMove * m_fScreenBasedSensitivity  * m_fMouseSensor; // 일반보다 느리게
			m_fLockOnYaw += fAngle;
		}
	}

	// LockOn 카메라 위치 계산
	Calculate_LockOn_Camera_Position(fTimeDelta);

	// 부드러운 카메라 이동
	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_vector vTargetPos = XMLoadFloat4(&m_vLockOnCameraPos);

	_float fLerpFactor = 1.0f - powf(0.8f, m_fLockOnSmoothSpeed * fTimeDelta);
	_vector vSmoothedPos = XMVectorLerp(vCurrentPos, vTargetPos, fLerpFactor);

	// 카메라 위치 설정
	XMStoreFloat4(&m_vCurrentCameraPos, vSmoothedPos);
	m_pTransformCom->Set_State(STATE::POSITION, vSmoothedPos);

	// 플레이어와 타겟 사이의 중점을 바라보도록 설정
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vTargetPos_Enemy = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vLookAtPos = (vPlayerPos + vTargetPos_Enemy) * 0.5f;

	// 살짝 위쪽을 바라보도록 Y 좌표 조정
	vLookAtPos = XMVectorSetY(vLookAtPos, XMVectorGetY(vLookAtPos));

	_float3 vLookAtFloat3;
	XMStoreFloat3(&vLookAtFloat3, vLookAtPos);
	m_pTransformCom->LookAt(vLookAtFloat3);
}

void CCamera_Player::Update_Chase_Target(_float fTimeDelta)
{

	if (nullptr == m_pTarget)
		return;

	// 1. 타겟(플레이어) 위치 가져오기
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 2. 마우스 입력 처리 (클라이언트 영역 내에서만)
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient); // 현재 창에서의 마우스 위치만 가져오기.

	//_float fTargetYaw = m_fYaw;
	if (PtInRect(&rcClient, ptMouse))
	{
		// 마우스 X축 이동으로 Y축 중심 회전 (Transform의 Turn 함수 사용)
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMove * m_fMouseSensor * fTimeDelta;
			
			//fTargetYaw += fAngle; // 목표값만 변경
			m_fYaw += fAngle;

		}
	}

	// 2. Yaw도 부드럽게 보간
	//_float fYawDiff = fTargetYaw - m_fYaw;

	// 최단 경로 계산 (180도 넘어가는 경우 처리)
	//while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
	//while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

	// 부드러운 회전 보간
	//_float fRotationSpeed = 12.0f; // 8.0f → 12.0f로 증가
	//_float fRotationLerpFactor = 1.0f - powf(0.3f, fRotationSpeed * fTimeDelta); // 0.1f → 0.3f
	//m_fYaw += fYawDiff * fRotationLerpFactor;

	// 3. Y축 회전 행렬로 오프셋 회전
	_matrix matRotY = XMMatrixRotationY(m_fYaw);
	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);

	// 4. 목표 카메라 위치 계산
	_vector vTargetCameraPos = vTargetPos + vRotatedOffset;
	XMStoreFloat4(&m_vTargetCameraPos, vTargetCameraPos);

	// 5. 첫 번째 업데이트인 경우 즉시 목표 위치로 이동
	if (m_bFirstUpdate)
	{
		m_vCurrentCameraPos = m_vTargetCameraPos;
		m_bFirstUpdate = false;
	}

	// 6. 부드러운 보간을 사용하여 카메라 위치 업데이트
	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_vector vTargetPos_Camera = XMLoadFloat4(&m_vTargetCameraPos);

	// Lerp를 사용한 부드러운 이동 (속도는 m_fSmoothSpeed로 조절)
	_float fLerpFactor = 1.0f - powf(0.5f, m_fSmoothSpeed * fTimeDelta);
	_vector vSmoothedPos = XMVectorLerp(vCurrentPos, vTargetPos_Camera, fLerpFactor);

	// 현재 위치 업데이트
	XMStoreFloat4(&m_vCurrentCameraPos, vSmoothedPos);
	m_pTransformCom->Set_State(STATE::POSITION, vSmoothedPos);

	// 7. Transform의 LookAt 함수를 사용해서 플레이어를 바라보도록 설정
	_float3 vTargetPosFloat3;
	XMStoreFloat3(&vTargetPosFloat3, vTargetPos);
	m_pTransformCom->LookAt(vTargetPosFloat3);
}


//void CCamera_Player::Update_Chase_Target(_float fTimeDelta)
//{
//
//	if (nullptr == m_pTarget)
//		return;
//
//	// 1. 타겟(플레이어) 위치 가져오기
//	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
//
//	// 2. 마우스 입력 처리 (클라이언트 영역 내에서만)
//	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
//	RECT rcClient;
//	GetClientRect(g_hWnd, &rcClient); // 현재 창에서의 마우스 위치만 가져오기.
//
//	if (PtInRect(&rcClient, ptMouse))
//	{
//		// 마우스 X축 이동으로 Y축 중심 회전 (Transform의 Turn 함수 사용)
//		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
//		{
//			_float fAngle = (_float)MouseMove * m_fMouseSensor * fTimeDelta;
//			m_fYaw += fAngle;
//		}
//	}
//
//	// 3. Y축 회전 행렬로 오프셋 회전
//	_matrix matRotY = XMMatrixRotationY(m_fYaw);
//	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);
//
//	// 4. 목표 카메라 위치 계산
//	_vector vTargetCameraPos = vTargetPos + vRotatedOffset;
//	XMStoreFloat4(&m_vTargetCameraPos, vTargetCameraPos);
//
//	// 5. 첫 번째 업데이트인 경우 즉시 목표 위치로 이동
//	if (m_bFirstUpdate)
//	{
//		m_vCurrentCameraPos = m_vTargetCameraPos;
//		m_bFirstUpdate = false;
//	}
//
//	// 6. 부드러운 보간을 사용하여 카메라 위치 업데이트
//	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
//	_vector vTargetPos_Camera = XMLoadFloat4(&m_vTargetCameraPos);
//
//
//	// Lerp를 사용한 부드러운 이동 (속도는 m_fSmoothSpeed로 조절)
//	_float fLerpFactor = 1.0f - powf(0.5f, m_fSmoothSpeed * fTimeDelta);
//	_vector vSmoothedPos = XMVectorLerp(vCurrentPos, vTargetPos_Camera, fLerpFactor);
//
//	// 현재 위치 업데이트
//	XMStoreFloat4(&m_vCurrentCameraPos, vSmoothedPos);
//	m_pTransformCom->Set_State(STATE::POSITION, vSmoothedPos);
//
//	// 7. Transform의 LookAt 함수를 사용해서 플레이어를 바라보도록 설정
//	_float3 vTargetPosFloat3;
//	XMStoreFloat3(&vTargetPosFloat3, vTargetPos);
//	m_pTransformCom->LookAt(vTargetPosFloat3);
//}



void CCamera_Player::Calculate_LockOn_Camera_Position(_float fTimeDelta)
{
	if (!m_pTarget || !m_pLockOnTarget)
		return;

	// 플레이어와 타겟의 위치
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 플레이어에서 타겟으로의 방향 벡터 (Y축 제거)
	_vector vPlayerToTarget = vTargetPos - vPlayerPos;
	vPlayerToTarget = XMVectorSetY(vPlayerToTarget, 0.0f);
	_float fPlayerToTargetDistance = XMVectorGetX(XMVector3Length(vPlayerToTarget));

	if (fPlayerToTargetDistance > 0.1f)
	{
		vPlayerToTarget = XMVector3Normalize(vPlayerToTarget);
	}
	else
	{
		// 플레이어와 타겟이 너무 가까우면 플레이어의 Look 벡터 사용
		vPlayerToTarget = m_pTarget->Get_Transform()->Get_State(STATE::LOOK);
		vPlayerToTarget = XMVectorSetY(vPlayerToTarget, 0.0f);
		vPlayerToTarget = XMVector3Normalize(vPlayerToTarget);
	}

	// 플레이어와 타겟 사이의 중점
	_vector vMidPoint = (vPlayerPos + vTargetPos) * 0.5f;

	// 카메라 위치 계산을 위한 기준 벡터들
	_vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_vector vRight = XMVector3Cross(vUp, vPlayerToTarget);
	vRight = XMVector3Normalize(vRight);

	// Yaw 회전 적용 (플레이어-타겟 축을 기준으로 회전)
	_matrix matYawRotation = XMMatrixRotationAxis(vUp, m_fLockOnYaw);
	_vector vCameraDirection = XMVector3TransformNormal(-vPlayerToTarget, matYawRotation);

	// Pitch 적용 (살짝 위에서 바라보도록)
	_float fPitchRad = XMConvertToRadians(m_fLockOnPitch);
	_vector vCameraRight = XMVector3Cross(vUp, vCameraDirection);
	vCameraRight = XMVector3Normalize(vCameraRight);
	_matrix matPitchRotation = XMMatrixRotationAxis(vCameraRight, fPitchRad);
	vCameraDirection = XMVector3TransformNormal(vCameraDirection, matPitchRotation);

	// 최종 카메라 위치 = 중점 + (방향 * 거리)
	_vector vCameraPos = vMidPoint + vCameraDirection * m_fLockOnDistance + vUp * m_fLockOnHeight;

	XMStoreFloat4(&m_vLockOnCameraPos, vCameraPos);
}

void CCamera_Player::Update_Normal_Camera(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	// 마우스 입력 처리
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	if (PtInRect(&rcClient, ptMouse))
	{
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMove * m_fMouseSensor * fTimeDelta;
			m_fYaw += fAngle;
		}
	}

	// 타겟 위치 가져오기
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	// Y축 회전 행렬로 오프셋 회전
	_matrix matRotY = XMMatrixRotationY(m_fYaw);
	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);

	// 목표 카메라 위치 계산
	_vector vTargetCameraPos = vTargetPos + vRotatedOffset;
	XMStoreFloat4(&m_vTargetCameraPos, vTargetCameraPos);

	// 첫 번째 업데이트인 경우 즉시 목표 위치로 이동
	if (m_bFirstUpdate)
	{
		m_vCurrentCameraPos = m_vTargetCameraPos;
		m_bFirstUpdate = false;
	}


	// === 4. 거리 기반 보간 (데드존 + 적응형 보간) ===
	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_vector vTargetPos_Camera = XMLoadFloat4(&m_vTargetCameraPos);

	_vector vDistance = vTargetPos_Camera - vCurrentPos;
	_float fDistanceLength = XMVectorGetX(XMVector3Length(vDistance));

	// 🔧 위치 데드존: 5cm 이내의 미세한 움직임은 무시
	const _float fPositionDeadZone = 0.5f;

	if (fDistanceLength > fPositionDeadZone)
	{
		// 🔧 거리에 따른 적응형 보간 속도
		_float fAdaptiveSpeed = m_fSmoothSpeed;

		// 거리가 클수록 더 빠르게 따라감 (자연스러운 움직임)
		if (fDistanceLength > 2.0f)
			fAdaptiveSpeed *= 1.5f;
		else if (fDistanceLength < 0.3f)
			fAdaptiveSpeed *= 0.5f;

		// 🔧 더 부드러운 보간 (기존 0.5f → 0.85f)
		_float fLerpFactor = 1.0f - powf(0.85f, fAdaptiveSpeed * fTimeDelta);

		// 🔧 최대 보간 속도 제한 (너무 빠른 움직임 방지)
		fLerpFactor = min(fLerpFactor, 0.9f);

		_vector vSmoothedPos = XMVectorLerp(vCurrentPos, vTargetPos_Camera, fLerpFactor);
		XMStoreFloat4(&m_vCurrentCameraPos, vSmoothedPos);
	}

	// === 5. 최종 카메라 위치 설정 ===
	_vector vFinalPos = XMLoadFloat4(&m_vCurrentCameraPos);
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);

	// === 6. 카메라 방향 설정 (타겟을 바라보도록) ===
	_float3 vLookAtTarget;
	XMStoreFloat3(&vLookAtTarget, vTargetPos + XMVectorSet(0.f, 1.5f, 0.f, 0.f)); // 플레이어 머리 높이
	m_pTransformCom->LookAt(vLookAtTarget);
}

void CCamera_Player::Debug_CameraVectors()
{
	CCamera* pCamera = m_pGameInstance->Get_MainCamera();
	if (!pCamera)
		return;

	_vector vLook = pCamera->Get_LookVector();
	_vector vRight = pCamera->Get_RightVector();

	// Y축 제거 전/후 비교
	OutputDebugString((L"[CAM] Original Look: (" +
		std::to_wstring(XMVectorGetX(vLook)) + L", " +
		std::to_wstring(XMVectorGetY(vLook)) + L", " +
		std::to_wstring(XMVectorGetZ(vLook)) + L")\n").c_str());

	vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector3Normalize(vLook);

	OutputDebugString((L"[CAM] Final Look: (" +
		std::to_wstring(XMVectorGetX(vLook)) + L", " +
		std::to_wstring(XMVectorGetY(vLook)) + L", " +
		std::to_wstring(XMVectorGetZ(vLook)) + L")\n").c_str());
}

void CCamera_Player::Force_Update_Target_Position()
{
	if (nullptr == m_pTarget)
		return;

	// 🔧 현재 플레이어 위치를 즉시 반영
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_matrix matRotY = XMMatrixRotationY(m_fYaw);
	_vector vRotatedOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRotY);
	_vector vTargetCameraPos = vTargetPos + vRotatedOffset;
	XMStoreFloat4(&m_vTargetCameraPos, vTargetCameraPos);

	// 🔧 더 반응성 좋은 보간 (움직임 중에는 더 빠르게)
	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_vector vTargetPos_Camera = XMLoadFloat4(&m_vTargetCameraPos);

	// 움직임 중에는 더 빠른 보간 사용
	_float fQuickLerpFactor = 0.3f; // 일반 보간보다 빠름
	_vector vQuickSmoothedPos = XMVectorLerp(vCurrentPos, vTargetPos_Camera, fQuickLerpFactor);

	XMStoreFloat4(&m_vCurrentCameraPos, vQuickSmoothedPos);

	// 카메라 Transform 즉시 업데이트
	m_pTransformCom->Set_State(STATE::POSITION, vQuickSmoothedPos);

	// 카메라 방향도 즉시 업데이트
	_float3 vLookAtTarget;
	XMStoreFloat3(&vLookAtTarget, vTargetPos + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
	m_pTransformCom->LookAt(vLookAtTarget);
}

void CCamera_Player::Handle_Mouse_Input(_float fTimeDelta, _float fSensitivityMultiplier)
{
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	if (PtInRect(&rcClient, ptMouse))
	{
		if (_long MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMove * m_fMouseSensor * fSensitivityMultiplier * fTimeDelta;

			if (m_bLockOnMode)
				m_fLockOnYaw += fAngle;
			else
				m_fYaw += fAngle;
		}
	}
}


CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();
}