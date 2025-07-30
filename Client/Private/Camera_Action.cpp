#include "Camera_Player.h"

CCamera_Action::CCamera_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Action::CCamera_Action(const CCamera_Action& Prototype)
	: CCamera(Prototype)
{
}

HRESULT CCamera_Action::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Action::Initialize_Clone(void* pArg)
{
	CAMERA_ACTION_DESC* pDesc = static_cast<CAMERA_ACTION_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor * 2.f;
	m_pTarget = pDesc->pTarget;

	// SpringArm 초기 설정
	m_fArmLength = 5.0f;
	m_fTargetArmLength = 5.0f;
	XMStoreFloat4(&m_vSocketOffset, XMVectorSet(0.f, 2.5f, 0.f, 0.f));
	m_vOriginalSocketOffset = m_vSocketOffset;

	// 초기 회전각
	m_fYaw = 0.f;
	m_fPitch = 0.f;

	// 줌 설정
	XMStoreFloat4(&m_vZoomTargetOffset, XMVectorSet(0.f, 2.0f, 0.f, 0.f));

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Action::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCamera_Action::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (nullptr == m_pTarget)
		return;

	// 줌 업데이트
	Update_Zoom(fTimeDelta);

	// 락온 상태에 따라 다른 카메라 업데이트
	if (IsLockOnActive())
	{
		Update_LockOn_Camera(fTimeDelta);
	}
	else
	{
		Update_SpringArm_Camera(fTimeDelta);
	}

	// 카메라 충돌 검사
	if (m_bDoCollisionTest)
	{
		Update_Camera_Collision();
	}

	// 부드러운 카메라 이동 적용
	Update_Camera_Smoothing(fTimeDelta);

	// 파이프라인 업데이트
	__super::Update_PipeLines();
}

void CCamera_Action::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCamera_Action::Render()
{
	return S_OK;
}

#pragma region SpringArm 카메라 업데이트
void CCamera_Action::Update_SpringArm_Camera(_float fTimeDelta)
{
	// 마우스 입력 처리
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	if (PtInRect(&rcClient, ptMouse))
	{
		// Yaw (좌우 회전)
		if (_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngle = (_float)MouseMoveX * m_fMouseSensor * fTimeDelta;
			m_fYaw += fAngle;
		}

		// Pitch (상하 회전) - 제한 추가
		if (_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
		{
			_float fAngle = (_float)MouseMoveY * m_fMouseSensor * fTimeDelta;
			m_fPitch += fAngle;

			// Pitch 각도 제한 (-45도 ~ +30도)
			m_fPitch = max(-XM_PI * 0.25f, min(XM_PI * 0.167f, m_fPitch));
		}
	}

	// SpringArm 길이 보간 (부드러운 줌)
	if (abs(m_fArmLength - m_fTargetArmLength) > 0.01f)
	{
		m_fArmLength = XMVectorGetX(XMVectorLerp(
			XMVectorSet(m_fArmLength, 0, 0, 0),
			XMVectorSet(m_fTargetArmLength, 0, 0, 0),
			10.0f * fTimeDelta));
	}

	// SpringArm 위치 계산
	_vector vDesiredPos = Calculate_SpringArm_Position();
	XMStoreFloat4(&m_vDesiredCameraPos, vDesiredPos);
}
#pragma endregion

#pragma region SpringArm 위치 계산
_vector CCamera_Action::Calculate_SpringArm_Position()
{
	if (nullptr == m_pTarget)
		return XMVectorZero();

	// 타겟의 위치와 소켓 오프셋
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vSocketPos = vTargetPos + XMLoadFloat4(&m_vSocketOffset);

	// 회전 매트릭스 생성 (Yaw, Pitch 적용)
	_matrix matRotation = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.f);

	// SpringArm 방향 벡터 (뒤쪽 방향)
	_vector vArmDirection = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	vArmDirection = XMVector3TransformNormal(vArmDirection, matRotation);

	// 최종 카메라 위치 = 소켓 위치 + (방향 * 거리)
	_vector vCameraPos = vSocketPos + (vArmDirection * m_fArmLength);

	return vCameraPos;
}
#pragma endregion

#pragma region 락온 시스템
void CCamera_Action::ToggleLockOn()
{
	if (IsLockOnActive())
	{
		ClearLockOnTarget();
	}
	else
	{
		CGameObject* pNearestTarget = FindNearestLockOnTarget();
		if (pNearestTarget)
		{
			SetLockOnTarget(pNearestTarget);
		}
	}
}

void CCamera_Action::SetLockOnTarget(CGameObject* pTarget)
{
	m_pLockOnTarget = pTarget;

	if (m_pLockOnTarget)
	{
		// 락온 시 카메라 거리 조정
		m_fTargetArmLength = m_fLockOnCameraDistance;
	}
}

void CCamera_Action::ClearLockOnTarget()
{
	m_pLockOnTarget = nullptr;
	// 원래 거리로 복원
	m_fTargetArmLength = 5.0f;
}

CGameObject* CCamera_Action::FindNearestLockOnTarget()
{
	if (nullptr == m_pTarget)
		return nullptr;

	// 플레이어 위치와 전방 벡터
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vPlayerForward = m_pTarget->Get_Transform()->Get_State(STATE::LOOK);
	vPlayerForward = XMVector3Normalize(vPlayerForward);

	CGameObject* pNearestTarget = nullptr;
	_float fNearestDistance = m_fLockOnRange;

	// TODO: 여기서 실제 게임 오브젝트 리스트를 검색해야 함
	// 예시: 모든 적 오브젝트를 검색하여 락온 가능한 타겟 찾기
	/*
	// 게임 인스턴스에서 적 오브젝트 리스트 가져오기
	list<CGameObject*>* pEnemyList = m_pGameInstance->Get_GameObjects_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));

	if (pEnemyList)
	{
		for (auto& pEnemy : *pEnemyList)
		{
			if (!pEnemy || pEnemy->Get_Dead())
				continue;

			_vector vEnemyPos = pEnemy->Get_Transform()->Get_State(STATE::POSITION);
			_vector vToEnemy = vEnemyPos - vPlayerPos;
			_float fDistance = XMVectorGetX(XMVector3Length(vToEnemy));

			// 거리 체크
			if (fDistance > m_fLockOnRange)
				continue;

			// 각도 체크
			_vector vToEnemyNorm = XMVector3Normalize(vToEnemy);
			_float fDot = XMVectorGetX(XMVector3Dot(vPlayerForward, vToEnemyNorm));
			_float fAngle = acosf(max(-1.0f, min(1.0f, fDot))); // 안전한 acos 계산

			if (fAngle > XMConvertToRadians(m_fLockOnAngle * 0.5f))
				continue;

			// 가장 가까운 타겟 선택
			if (fDistance < fNearestDistance)
			{
				fNearestDistance = fDistance;
				pNearestTarget = pEnemy;
			}
		}
	}
	*/

	return pNearestTarget;
}

CGameObject* CCamera_Action::FindNextLockOnTarget(_bool bClockwise)
{
	// 현재 락온 타겟이 없으면 가장 가까운 타겟 찾기
	if (!IsLockOnActive())
		return FindNearestLockOnTarget();

	// TODO: 시계방향/반시계방향으로 다음 타겟 찾기 구현
	/*
	// 현재 타겟의 각도 계산
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vCurrentTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vToCurrentTarget = vCurrentTargetPos - vPlayerPos;

	// 다른 타겟들과 각도 비교하여 다음 타겟 선택
	*/

	return nullptr;
}

void CCamera_Action::Update_LockOn_Camera(_float fTimeDelta)
{
	if (nullptr == m_pLockOnTarget || nullptr == m_pTarget)
		return;

	// 락온 타겟이 유효한지 확인 (거리, 생존 여부 등)
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlayerPos));

	// 락온 범위를 벗어나거나 타겟이 죽었으면 락온 해제
	if (fDistance > m_fLockOnRange * 1.2f) // 약간의 여유 범위
	{
		ClearLockOnTarget();
		return;
	}

	// 락온 시 카메라 위치 계산
	_vector vDesiredPos = Calculate_LockOn_Position();
	XMStoreFloat4(&m_vDesiredCameraPos, vDesiredPos);
}

_vector CCamera_Action::Calculate_LockOn_Position()
{
	if (nullptr == m_pLockOnTarget || nullptr == m_pTarget)
		return XMVectorZero();

	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 플레이어와 타겟 사이의 중점 계산
	_vector vMidPoint = (vPlayerPos + vTargetPos) * 0.5f;
	vMidPoint = XMVectorSetY(vMidPoint, XMVectorGetY(vPlayerPos) + m_fLockOnCameraHeight);

	// 플레이어에서 타겟으로의 방향
	_vector vPlayerToTarget = vTargetPos - vPlayerPos;
	vPlayerToTarget = XMVector3Normalize(vPlayerToTarget);

	// 카메라를 약간 뒤쪽과 옆쪽에 배치
	_vector vRight = XMVector3Cross(vPlayerToTarget, XMVectorSet(0, 1, 0, 0));
	vRight = XMVector3Normalize(vRight);
	_vector vBack = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vRight);

	_vector vCameraPos = vMidPoint + (vBack * m_fLockOnCameraDistance * 0.7f) +
		(vRight * m_fLockOnCameraDistance * 0.3f);

	return vCameraPos;
}
#pragma endregion

#pragma region 부드러운 카메라 이동 (최우선 - 자연스러운 플레이어 추적)
void CCamera_Action::Update_Camera_Smoothing(_float fTimeDelta)
{
	if (m_bFirstUpdate)
	{
		XMStoreFloat4(&m_vCurrentCameraPos, XMLoadFloat4(&m_vDesiredCameraPos));
		m_bFirstUpdate = false;
		return;
	}

	_vector vCurrentPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_vector vDesiredPos = XMLoadFloat4(&m_vDesiredCameraPos);

	// 플레이어 움직임 상태 확인
	_bool bPlayerMoving = Is_Player_Moving();

	// 동적 추적 속도 계산
	_float fDynamicLagSpeed = Calculate_Dynamic_LagSpeed(fTimeDelta);

	// 위치 차이 계산
	_vector vDifference = vDesiredPos - vCurrentPos;
	_float fDistance = XMVectorGetX(XMVector3Length(vDifference));

	// 매우 작은 차이는 무시 (떨림 방지)
	if (fDistance < 0.001f)
	{
		XMStoreFloat4(&m_vCurrentCameraPos, vDesiredPos);
	}
	else
	{
		// 보간 계수 계산 - 더 자연스러운 추적을 위한 개선
		_float fLerpFactor;

		if (bPlayerMoving)
		{
			// 움직일 때: 거의 즉시 따라감 (매우 높은 반응성)
			fLerpFactor = 1.0f - powf(0.1f, fDynamicLagSpeed * fTimeDelta);
		}
		else
		{
			// 정지했을 때: 부드럽게 따라감
			fLerpFactor = 1.0f - powf(0.3f, fDynamicLagSpeed * fTimeDelta);
		}

		// 최대 보간 속도 제한
		fLerpFactor = min(fLerpFactor, 0.98f);

		_vector vNewPos = XMVectorLerp(vCurrentPos, vDesiredPos, fLerpFactor);
		XMStoreFloat4(&m_vCurrentCameraPos, vNewPos);
	}

	// 최종 카메라 위치 설정
	_vector vFinalPos = XMLoadFloat4(&m_vCurrentCameraPos);
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos);

	// 카메라 방향 설정
	if (IsLockOnActive() && m_pLockOnTarget)
	{
		// 락온 시: 타겟을 바라봄 (부드러운 전환)
		_vector vTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);
		_vector vLookAtPos = vTargetPos + XMLoadFloat4(&m_vLockOnOffset);

		_float3 vLookAt;
		XMStoreFloat3(&vLookAt, vLookAtPos);

		// 부드러운 회전을 위한 보간 (선택사항)
		m_pTransformCom->LookAt(vLookAt);
	}
	else
	{
		// 일반 모드: 플레이어 방향을 바라봄
		_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
		_float3 vLookAtTarget;
		XMStoreFloat3(&vLookAtTarget, vTargetPos + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
		m_pTransformCom->LookAt(vLookAtTarget);
	}
}

void CCamera_Action::Update_Smooth_Movement(_float fTimeDelta)
{
}

_bool CCamera_Action::Is_Player_Moving()
{
	if (nullptr == m_pTarget)
		return false;

	// 정적 변수로 이전 위치 저장
	static _float4 s_vPrevPlayerPos = { 0, 0, 0, 0 };
	static _bool s_bInitialized = false;

	_vector vCurrentPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_float4 vCurrentPos4;
	XMStoreFloat4(&vCurrentPos4, vCurrentPos);

	// 첫 번째 프레임에서는 움직임 없음으로 처리
	if (!s_bInitialized)
	{
		s_vPrevPlayerPos = vCurrentPos4;
		s_bInitialized = true;
		return false;
	}

	_vector vPrevPos = XMLoadFloat4(&s_vPrevPlayerPos);
	_float fMovement = XMVectorGetX(XMVector3Length(vCurrentPos - vPrevPos));

	s_vPrevPlayerPos = vCurrentPos4;

	return fMovement > m_fMovementThreshold;
}

_float CCamera_Action::Calculate_Dynamic_LagSpeed(_float fTimeDelta)
{
	_bool bPlayerMoving = Is_Player_Moving();

	// 플레이어 움직임 상태 변화 추적
	if (bPlayerMoving != m_bPlayerWasMoving)
	{
		m_fPlayerStopTime = 0.0f;
	}
	m_bPlayerWasMoving = bPlayerMoving;

	if (!bPlayerMoving)
	{
		m_fPlayerStopTime += fTimeDelta;
	}

	_float fDynamicSpeed = m_fCameraLagSpeed;

	if (bPlayerMoving)
	{
		// 움직일 때: 매우 빠르게 따라감 (거의 즉시)
		fDynamicSpeed = m_fCameraLagSpeed * 8.0f; // 더 높은 반응성
	}
	else
	{
		// 정지했을 때: 시간에 따라 점진적으로 느려짐
		if (m_fPlayerStopTime < 0.05f)
		{
			// 방금 정지: 여전히 빠르게
			fDynamicSpeed = m_fCameraLagSpeed * 4.0f;
		}
		else if (m_fPlayerStopTime < 0.2f)
		{
			// 잠시 정지: 중간 속도
			fDynamicSpeed = m_fCameraLagSpeed * 2.0f;
		}
		else
		{
			// 완전 정지: 부드럽게
			fDynamicSpeed = m_fCameraLagSpeed * 1.0f;
		}
	}

	return fDynamicSpeed;
}
#pragma endregion

#pragma region 충돌 검사
void CCamera_Action::Update_Camera_Collision()
{
	if (nullptr == m_pTarget)
		return;

	// 소켓 위치부터 현재 카메라 위치까지 레이캐스트
	_vector vSocketPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION) +
		XMLoadFloat4(&m_vSocketOffset);
	_vector vCurrentCameraPos = XMLoadFloat4(&m_vCurrentCameraPos);

	_vector vDirection = vCurrentCameraPos - vSocketPos;
	_float fOriginalDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fOriginalDistance > 0.001f)  // 0으로 나누기 방지
	{
		vDirection = XMVector3Normalize(vDirection);

		// TODO: 실제 레이캐스트 구현
		// 현재는 간단한 구형 충돌 검사로 대체
		/*
		RaycastHit hit;
		if (Physics.Raycast(vSocketPos, vDirection, out hit, fOriginalDistance, m_CollisionLayerMask))
		{
			// 충돌 지점에서 ProbeSize만큼 앞쪽으로 카메라 위치 조정
			_vector vHitPos = hit.point;
			_vector vNewCameraPos = vHitPos - (vDirection * m_fProbeSize);
			XMStoreFloat4(&m_vCurrentCameraPos, vNewCameraPos);
		}
		*/
	}
}
#pragma endregion

#pragma region 줌 기능
void CCamera_Action::Reset_Zoom()
{
	m_bIsZooming = false;
	m_fZoomLerpTime = 0.f;
	m_fTargetArmLength = m_fArmLength;
}

void CCamera_Action::Start_Zoom_In(_float fZoomDuration)
{
	if (m_bIsZooming && m_bZoomIn) return;

	m_bIsZooming = true;
	m_bZoomIn = true;
	m_fZoomLerpTime = 0.f;
	m_fZoomMaxTime = fZoomDuration;

	// 줌인 시 목표 거리 설정
	m_fTargetArmLength = max(m_fMinArmLength, m_fArmLength * 0.6f);
}

void CCamera_Action::Start_Zoom_Out(_float fZoomDuration)
{
	if (m_bIsZooming && !m_bZoomIn) return;

	m_bIsZooming = true;
	m_bZoomIn = false;
	m_fZoomLerpTime = 0.f;
	m_fZoomMaxTime = fZoomDuration;

	// 줌아웃 시 목표 거리 설정
	m_fTargetArmLength = min(m_fMaxArmLength, m_fArmLength * 1.5f);
}

void CCamera_Action::Update_Zoom(_float fTimeDelta)
{
	if (!m_bIsZooming) return;

	m_fZoomLerpTime += fTimeDelta;
	_float fLerpRatio = m_fZoomLerpTime / m_fZoomMaxTime;

	if (fLerpRatio >= 1.f)
	{
		fLerpRatio = 1.f;
		m_bIsZooming = false;
	}

	// 부드러운 보간을 위한 easing 함수 적용
	fLerpRatio = fLerpRatio * fLerpRatio * (3.f - 2.f * fLerpRatio); // smoothstep

	// SpringArm 길이로 줌 구현 (기존 오프셋 방식 대신)
	_float fStartLength = m_bZoomIn ? 5.0f : m_fTargetArmLength;
	_float fEndLength = m_bZoomIn ? m_fTargetArmLength : 5.0f;

	m_fArmLength = XMVectorGetX(XMVectorLerp(
		XMVectorSet(fStartLength, 0, 0, 0),
		XMVectorSet(fEndLength, 0, 0, 0),
		fLerpRatio));
}
#pragma endregion

#pragma region 기존 함수들 유지
_float CCamera_Action::Get_Yaw() const
{
	return m_fYaw;
}

void CCamera_Action::Set_Yaw(_float fYaw)
{
	m_fYaw = fYaw;
}

void CCamera_Action::Add_Yaw(_float fYawDelta)
{
	m_fYaw += fYawDelta;
}

void CCamera_Action::Set_TargetOffset(_float4 vOffset)
{
	m_vSocketOffset = vOffset;
}

void CCamera_Action::Debug_CameraVectors()
{
	CCamera* pCamera = m_pGameInstance->Get_MainCamera();
	if (!pCamera)
		return;

	_vector vLook = pCamera->Get_LookVector();
	_vector vRight = pCamera->Get_RightVector();

	OutputDebugString((L"[CAM] SpringArm Look: (" +
		std::to_wstring(XMVectorGetX(vLook)) + L", " +
		std::to_wstring(XMVectorGetY(vLook)) + L", " +
		std::to_wstring(XMVectorGetZ(vLook)) + L")\n").c_str());

	OutputDebugString((L"[CAM] ArmLength: " + std::to_wstring(m_fArmLength) +
		L", PlayerMoving: " + (Is_Player_Moving() ? L"True" : L"False") + L"\n").c_str());
}
#pragma endregion

CCamera_Action* CCamera_Action::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Action* pInstance = new CCamera_Action(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CCamera_Action"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Action::Clone(void* pArg)
{
	CCamera_Action* pInstance = new CCamera_Action(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CCamera_Action"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Action::Free()
{
	__super::Free();
}