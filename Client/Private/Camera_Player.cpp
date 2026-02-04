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

	m_eCurLevel = pDesc->eCurLevel;
	m_fMouseSensor = pDesc->fMouseSensor * 0.8f; // 감도를 낮춤
	m_pTarget = pDesc->pTarget;

	m_vTargetOffset = pDesc->vTargetOffset;
	m_vLockOnOffset = pDesc->vLockOnOffset;
	// 화면 크기 가져오기
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	// 초기 각도 (플레이어 후방에서 플레이어를 바라봄)
	m_fYaw = 0.f;   
	m_fPitch = XMConvertToRadians(m_fDefaultPitch);  // 기본 각도로 설정
	m_fTargetYaw = m_fYaw;
	m_fTargetPitch = m_fPitch;

	if (FAILED(CCamera::Initialize_Clone(pArg)))
		return E_FAIL;

	// 마우스 커서 클립 활성화
	Enable_Mouse_Clip();

	if (FAILED(Create_LockOn_UI())) 
		return E_FAIL;

	return S_OK;


}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
	CCamera::Priority_Update(fTimeDelta);
	
}

void CCamera_Player::Update(_float fTimeDelta)
{

	CCamera::Update(fTimeDelta);

	// 락온 모드에 따라 카메라 업데이트 방식 선택
	if (m_IsLockOnMode)
		Update_LockOn_Camera(fTimeDelta);
	else
		Update_Chase_Target(fTimeDelta);

	Apply_Shake(fTimeDelta);
	
	// LockOn UI 업데이트
	if (m_pLockOnUI)
		Update_LockOn_UI(fTimeDelta);

	// 1. 마우스 클립 업데이트 => 창 크기를 벗어나지 못하게 함.
	Update_Mouse_Clip();

	// 파이프라인 업데이트
	CCamera::Update_PipeLines();
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
	CCamera::Late_Update(fTimeDelta);

	if (m_pLockOnUI)
		m_pLockOnUI->Late_Update(fTimeDelta);


#ifdef _DEBUG
	// 임시 디버깅용도 Render 추가

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this);
#endif // _DEBUG

	
}

HRESULT CCamera_Player::Render()
{
#ifdef _DEBUG
	//ImGui_Render();
#endif // _DEBUG



	return S_OK;
}

#pragma region 0. 일반 상태 <- 플레이어 추적.

void CCamera_Player::Update_Chase_Target(_float fTimeDelta)
{
	if (!m_pTarget) return;

	// 1. 입력 및 회전값 갱신 (부드러운 회전)
	Handle_Mouse_Input(fTimeDelta);

	_float fLerp = 1.0f - powf(0.5f, m_fRotationSmoothSpeed * fTimeDelta);
	m_fYaw = Lerp(m_fYaw, m_fTargetYaw, fLerp);
	m_fPitch = Lerp(m_fPitch, m_fTargetPitch, fLerp);

	// 2. 목표 위치 계산
	// 플레이어 위치 + (회전된 오프셋)
	_matrix matRot = XMMatrixRotationX(m_fPitch) * XMMatrixRotationY(m_fYaw);
	_vector vOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vTargetOffset), matRot);
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	_vector vDstCamPos = vTargetPos + vOffset;

	// 3. 부드러운 이동 (감쇠 진동 없는 SmoothDamp 느낌의 Lerp)
	_vector vCurPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_float fPosLerp = 1.0f - powf(0.5f, m_fSmoothSpeed * fTimeDelta);
	_vector vFinalPos = XMVectorLerp(vCurPos, vDstCamPos, fPosLerp);

	XMStoreFloat4(&m_vCurrentCameraPos, vFinalPos);

	// 4. Transform 적용
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos + m_vShakeOffset);

	// 회전: 계산된 회전 행렬을 그대로 Look/Up/Right에 적용 (수동 계산 제거)
	// 회전 행렬의 각 행(Row)이 곧 로컬 축 벡터입니다.
	m_pTransformCom->Set_State(STATE::RIGHT, matRot.r[0]);
	m_pTransformCom->Set_State(STATE::UP, matRot.r[1]);
	m_pTransformCom->Set_State(STATE::LOOK, matRot.r[2]);
}

void CCamera_Player::Handle_Mouse_Input(_float fTimeDelta, _float fSensitivityMultiplier)
{
	POINT ptMouse = m_pGameInstance->Get_Mouse_Cursor(g_hWnd);
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);

	// Q키 상태 확인 (Pitch 조작 모드)
	m_IsPitchControlMode = m_pGameInstance->Get_KeyPress(DIK_Q);

	// Q키를 누르지 않았을 때는 기본 Pitch로 복귀
	if (!m_IsPitchControlMode)
	{
		m_fTargetPitch = XMConvertToRadians(m_fDefaultPitch);
	}
	
	if (m_IsInventoryMode || m_IsSkillMode)
		return;

	if (PtInRect(&rcClient, ptMouse))
	{
		// X축 마우스 움직임 (Yaw - 좌우 회전)
		if (_long MouseMoveX = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		{
			_float fAngleX = (_float)MouseMoveX * m_fMouseSensor * fSensitivityMultiplier * fTimeDelta;

			if (m_IsLockOnMode)
				m_fLockOnYaw += fAngleX;
			else
				m_fTargetYaw += fAngleX; // 목표 각도만 수정
		}

		// Y축 마우스 움직임 (Pitch - 위아래 회전) - Q키를 누르고 있을 때만
		if (_long MouseMoveY = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
		{
			_float fAngleY = (_float)MouseMoveY * m_fMouseSensor * fSensitivityMultiplier * fTimeDelta;

			// 락온 모드가 아니고 Q키를 누르고 있을 때만 Pitch 적용
			if (!m_IsLockOnMode && m_IsPitchControlMode)
			{
				m_fTargetPitch -= fAngleY; // 목표 각도만 수정

				// 목표 Pitch 제한 적용
				if (m_fTargetPitch > XMConvertToRadians(m_fMaxPitch))
					m_fTargetPitch = XMConvertToRadians(m_fMaxPitch);
				else if (m_fTargetPitch < XMConvertToRadians(m_fMinPitch))
					m_fTargetPitch = XMConvertToRadians(m_fMinPitch);
			}
		}
	}
}

/* 해당 카메라 생성 시 Mouse Clipped를 킵니다 */
void CCamera_Player::Enable_Mouse_Clip()
{
	if (!m_IsMouseClipped)
	{
		// 현재 클라이언트 영역을 화면 좌표로 변환
		GetClientRect(g_hWnd, &m_rcClipRect);
		ClientToScreen(g_hWnd, (POINT*)&m_rcClipRect.left);
		ClientToScreen(g_hWnd, (POINT*)&m_rcClipRect.right);

		// 마우스 커서를 해당 영역으로 제한
		ClipCursor(&m_rcClipRect);
		m_IsMouseClipped = true;
	}
}

/* 해당 카메라 삭제 시 Mouse Clipped를 끕니다. */
void CCamera_Player::Disable_Mouse_Clip()
{
	if (m_IsMouseClipped)
	{
		// 마우스 커서 제한 해제
		ClipCursor(nullptr);
		m_IsMouseClipped = false;
	}
}

void CCamera_Player::Update_Mouse_Clip()
{
	// 창 크기가 변경되었을 수 있으므로 클립 영역 업데이트
	if (m_IsMouseClipped)
	{
		RECT rcNewClip;
		GetClientRect(g_hWnd, &rcNewClip);
		ClientToScreen(g_hWnd, (POINT*)&rcNewClip.left);
		ClientToScreen(g_hWnd, (POINT*)&rcNewClip.right);

		// 영역이 변경되었으면 업데이트
		if (memcmp(&m_rcClipRect, &rcNewClip, sizeof(RECT)) != 0)
		{
			m_rcClipRect = rcNewClip;
			ClipCursor(&m_rcClipRect);
		}
	}
}

#pragma endregion

#pragma region 1. Lock ON 상태.
void CCamera_Player::Toggle_LockOn_Mode()
{
	if (m_IsLockOnMode)
	{
		// 락온 해제
		Disable_LockOn_Mode();
		Clear_LockOn_Target();
	}
	else
	{
		// 락온 시도
		Try_LockOn_Closest_Monster();
	}
}

void CCamera_Player::Update_LockOn_Camera(_float fTimeDelta)
{
	// 1. 유효성 검사 (타겟 소실/거리 체크)
	if (!Validate_LockOn_Target())
	{
		Clear_LockOn_Target();
		return;
	}

	// 2. 목표 위치 계산
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vEnemyPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);


	// 3. 방향벡터를 계산합니다.
	_vector vLookDir = vPlayerPos - vEnemyPos;
	vLookDir = XMVector3Normalize(XMVectorSetY(vLookDir, 0.f)); // 수평 방향으로만 계산 => 높이 무시

	// 4. 거리 설정.
	_float fBackDistance = fabsf(m_vLockOnOffset.z);

	// 5. 목표 위치 => 플레이어 위치에서 '뒤로' 거리만큼 이동 + 위로 높이만큼 이동
	_vector vDstCamPos = vPlayerPos + (vLookDir * fBackDistance) + XMVectorSet(0.f, m_vLockOnOffset.y, 0.f, 0.f);

	// 6. 부드러운 이동 (감쇠 진동 방지)
	_vector vCurPos = XMLoadFloat4(&m_vCurrentCameraPos);
	_float fPosLerp = 1.0f - powf(0.1f, m_fLockOnSmoothSpeed * fTimeDelta);
	_vector vFinalPos = XMVectorLerp(vCurPos, vDstCamPos, fPosLerp);
	XMStoreFloat4(&m_vCurrentCameraPos, vFinalPos);

	// 7. Transform 적용
	m_pTransformCom->Set_State(STATE::POSITION, vFinalPos + m_vShakeOffset);

	// 8. 시선 고정 (LookAt)
	_float3 vLookAt = {};
	XMStoreFloat3(&vLookAt, vEnemyPos);
	m_pTransformCom->LookAt(vLookAt);
}


void CCamera_Player::Clear_LockOn_Target()
{
	m_pLockOnTarget = nullptr;
	Disable_LockOn_Mode();
}

void CCamera_Player::Enable_LockOn_Mode()
{
	m_IsLockOnMode = true;

	// LockOn 모드 시작 시 마우스 회전값 초기화 (자연스러운 전환을 위해)
	m_fLockOnYaw = 0.0f;

	// LockOn 카메라 위치 초기화
	if (m_pTarget && m_pLockOnTarget)
		Calculate_LockOn_Camera_Position(0.0f);

	// LockOn UI 활성화
	if (m_pLockOnUI)
	{
		// Target 설정.
		m_pLockOnUI->Set_Target(m_pLockOnTarget);
		m_pLockOnUI->Set_Active(true);
	}
}

void CCamera_Player::Disable_LockOn_Mode()
{
	m_IsLockOnMode = false;

	// 1. 현재 카메라의 Look 방향을 가져와서 Yaw/Pitch로 변환
	_vector vCurrentLook = m_pTransformCom->Get_State(STATE::LOOK);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	// 2. Look 벡터에서 Yaw 계산 (Y축 제거한 수평 방향)
	_vector vHorizontalLook = XMVectorSetY(vCurrentLook, 0.0f);
	_float fHorizontalLength = XMVectorGetX(XMVector3Length(vHorizontalLook));

	if (fHorizontalLength > 0.001f)
	{
		vHorizontalLook = XMVector3Normalize(vHorizontalLook);
		m_fYaw = atan2f(XMVectorGetX(vHorizontalLook), XMVectorGetZ(vHorizontalLook));
		m_fTargetYaw = m_fYaw;
	}
	else
	{
		// 수직으로 바라보는 경우 현재 LockOn Yaw 유지
		m_fYaw = m_fLockOnYaw;
		m_fTargetYaw = m_fLockOnYaw;
	}

	// Look 벡터에서 Pitch 계산
	m_fPitch = asinf(-XMVectorGetY(vCurrentLook));
	m_fTargetPitch = m_fPitch;

	// LockOn UI 비활성화
	if (m_pLockOnUI)
	{
		m_pLockOnUI->Clear_Target();
		m_pLockOnUI->Set_Active(false);
	}
}

_bool CCamera_Player::Try_LockOn_Closest_Monster()
{
	CMonster* pClosestMonster = Find_Closest_Monster(m_fMaxLockOnDistance);

	if (pClosestMonster)
	{
		Set_LockOn_Target(pClosestMonster);
		Enable_LockOn_Mode();
		return true;
	}

	return false;
}

// 가장 가까운 몬스터 탐지
CMonster* CCamera_Player::Find_Closest_Monster(_float fMaxDistance)
{
	if (!m_pTarget)
		return nullptr;

	// 플레이어 위치
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	CMonster* pClosestMonster = nullptr;
	_float fClosestDistance = FLT_MAX;

	// Object Manager에서 모든 몬스터 검색
	CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Monster"));
	if (!pLayer)
		return nullptr;

	list<CGameObject*>& MonsterList = pLayer->Get_GameObjects();

	for (auto& pGameObject : MonsterList)
	{
		if (!pGameObject || pGameObject->Is_Destroy())
			continue;

		// 몬스터인지 확인 (Monster 클래스로 캐스팅 가능한지 체크)
		CMonster* pMonster = dynamic_cast<CMonster*>(pGameObject);
		if (!pMonster)
			continue;

		// 몬스터가 살아있는지 확인
		if (pMonster->HasBuff(CMonster::BUFF_DEAD) || pMonster->HasBuff(CMonster::BUFF_CORPSE))
			continue;

		// 거리 체크
		_vector vMonsterPos = pMonster->Get_Transform()->Get_State(STATE::POSITION);
		_float fDistance = XMVectorGetX(XMVector3Length(vMonsterPos - vPlayerPos));

		if (fDistance > fMaxDistance)
			continue;

		// 가장 가까운 몬스터 선택 (거리 기준)
		if (fDistance < fClosestDistance)
		{
			fClosestDistance = fDistance;
			pClosestMonster = pMonster;
		}
	}

	return pClosestMonster;
}

void CCamera_Player::Set_LockOn_Target(CMonster* pTarget)
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


void CCamera_Player::Calculate_LockOn_Camera_Position(_float fTimeDelta)
{
	if (!m_pTarget || !m_pLockOnTarget)
		return;

	// 1. 플레이어와 타겟의 위치
	_vector vPlayerPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vTargetPos = m_pLockOnTarget->Get_Transform()->Get_State(STATE::POSITION);

	// 2. 플레이어에서 몬스터로의 방향 벡터 계산
	_vector vPlayerToTarget = vTargetPos - vPlayerPos;
	vPlayerToTarget = XMVectorSetY(vPlayerToTarget, 0.0f); // Y축 제거 (수평면에서만)

	_float fDistance = XMVectorGetX(XMVector3Length(vPlayerToTarget));
	if (fDistance < 0.1f)
	{
		// 3. 거리가 너무 가까우면 플레이어의 Look 방향 사용
		vPlayerToTarget = m_pTarget->Get_Transform()->Get_State(STATE::LOOK);
		vPlayerToTarget = XMVectorSetY(vPlayerToTarget, 0.0f);
	}

	vPlayerToTarget = XMVector3Normalize(vPlayerToTarget);

	// 4. 플레이어가 몬스터를 바라보는 방향을 기준으로 Yaw 각도 계산
	_float fTargetYaw = atan2f(XMVectorGetX(vPlayerToTarget), XMVectorGetZ(vPlayerToTarget));

	// 5. 락온 전용 오프셋을 사용해서 카메라 위치 계산 (더 멀리)
	_matrix matRotY = XMMatrixRotationY(fTargetYaw);
	_vector vCameraOffset = XMVector3TransformNormal(XMLoadFloat4(&m_vLockOnOffset), matRotY);

	// 6. 카메라 위치 = 플레이어 위치 + 회전된 오프셋
	_vector vCameraPos = vPlayerPos + vCameraOffset;

	XMStoreFloat4(&m_vLockOnCameraPos, vCameraPos);
}

#pragma endregion




void CCamera_Player::StartShake(_float duration, _float magnitude)
{
	m_fShakeTime = duration;
	m_fShakeDuration = duration;
	m_fShakeMagnitude = magnitude;
}

#ifdef _DEBUG
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

void CCamera_Player::ImGui_Render()
{

	ImGuiIO& io = ImGui::GetIO();

	// 기존 Player Debug Window

	ImVec2 windowSize = ImVec2(300.f, 300.f);
	ImVec2 windowPos = ImVec2(0.f, 0.f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

	ImGui::Begin("CC", nullptr, ImGuiWindowFlags_NoCollapse);

	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

	if (m_IsLockOnMode)
	{
		ImGui::Text("Lock On Camera Pos : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
	}
	else
	{
		ImGui::Text("Normal Camera Pos : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
	}

	ImGui::End();
}
#endif // _DEBUG

HRESULT CCamera_Player::Create_LockOn_UI()
{
	CLockOnUI::LOCKONUI_DESC UIDesc{};
	UIDesc.fSizeX = 64.0f;
	UIDesc.fSizeY = 64.0f;
	UIDesc.eShaderPath = POSTEX_SHADERPATH::LOCKON;
	m_pLockOnUI = static_cast<CLockOnUI*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT
		, ENUM_CLASS(LEVEL::STATIC)
		, TEXT("Prototype_GameObject_LockOnUI"), &UIDesc));

	if (!m_pLockOnUI)
	{
		MSG_BOX(TEXT("Failed to create LockOn UI"));
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Player::Apply_Shake(_float fTimeDelta)
{
	if (m_fShakeTime > 0.0f)
	{
		// 1. 남은 시간 감소
		m_fShakeTime -= fTimeDelta;

		// 2. 랜덤 오프셋 생성
		// -1.0f ~ +1.0f 사이의 랜덤 값을 생성합니다.
		_float fOffsetX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		_float fOffsetY = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		_float fOffsetZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

		m_vShakeOffset = XMVectorSet(fOffsetX, fOffsetY, fOffsetZ, 0.0f);

		// 3. 오프셋에 강도를 곱하고 정규화 (방향만 사용)
		m_vShakeOffset = XMVector3Normalize(m_vShakeOffset) * m_fShakeMagnitude;

		// 4. (선택) 시간이 지남에 따라 쉐이킹 강도를 서서히 줄여 부드럽게 멈추게 함
		m_fShakeMagnitude *= (m_fShakeTime / m_fShakeDuration);
	}
	else
	{
		m_fShakeTime = 0.0f;
		m_fShakeDuration = 0.0f;
		m_fShakeMagnitude = 0.0f;
		m_vShakeOffset = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
}




_bool CCamera_Player::Validate_LockOn_Target()
{

	if (!m_pTarget || !m_pLockOnTarget)
		return false;

	if (m_pLockOnTarget && (m_pLockOnTarget->HasBuff(CMonster::BUFF_DEAD)
		|| m_pLockOnTarget->HasBuff(CMonster::BUFF_CORPSE)))
		return false;

	return true;
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

void CCamera_Player::Update_LockOn_UI(_float fTimeDelta)
{

	// UI가 활성화되어 있으면 업데이트 실행
	if (m_pLockOnUI->Is_Active())
	{
		// Camera에서 Priority_Update와 Update만 호출 (Late_Update는 렌더링 시스템이 처리)
		m_pLockOnUI->Priority_Update(fTimeDelta);
		m_pLockOnUI->Update(fTimeDelta);
	}

	
}

void CCamera_Player::Free()
{
	// LockOn UI 해제
	Safe_Release(m_pLockOnUI);
	
	// 마우스 커서 클립 해제
	Disable_Mouse_Clip();
	
	CCamera::Free();
}