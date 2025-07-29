#include "Player_StrongAttackState.h"

CPlayer_AttackState::CPlayer_AttackState()
{
}

HRESULT CPlayer_AttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_AttackState::Enter(void* pArg)
{

	ATTACK_ENTER_DESC* pDesc = static_cast<ATTACK_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

	m_isLoop = false;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
	m_pModelCom->Set_RootMotionTranslate(false);
	m_pModelCom->Set_RootMotionRotation(true);


	// 방향 제어 관련 초기화
	m_bCanChangeDirection = true;
	m_fCurrentLockTime = 0.0f;
	m_bIsDirectionLocked = false;
	
	
	// 디버그 출력
	//_wstring debugMsg = L"[COMBO] Step " + to_wstring(m_iCurrentComboStep) +
	//	L", AnimIdx: " + to_wstring(m_iCurAnimIdx) + L"\n";
	//OutputDebugString(debugMsg.c_str());
}

/* State 실행 */
void CPlayer_AttackState::Update(_float fTimeDelta)
{
	Handle_DirectionInput(fTimeDelta);
	Handle_Input();

	Change_State(fTimeDelta);

}

// 종료될 때 실행할 동작..
void CPlayer_AttackState::Exit()
{
	if (m_iNextState != -1)
	{
		
		if (m_iNextState == CPlayer::PLAYER_STATE::IDLE)
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		}
		else if (m_iNextState == CPlayer::PLAYER_STATE::GUARD)
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		}
		else
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.1f, true, true, true);
		}
		
	}

	m_pFsm->Set_StateCoolTime(CPlayer::DODGE, 0.1f);
	
}

// 상태 초기화
void CPlayer_AttackState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextAnimIdx = -1;

	m_pModelCom->Animation_Reset();
}

void CPlayer_AttackState::Change_State(_float fTimeDelta)
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};

	// 애니메이션이 끝났을 때
	if (m_pModelCom->Is_Finished())
	{
		// Idle 상태로 전환
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = PLAYER_ANIM_IDLE;
		Idle.iAnimation_Idx = PLAYER_ANIM_IDLE;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);

		return;
	}


	// 현재 모션이 최소한 Exit CoolTime보다 진행되었을 경우.
	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			if (m_iCurAnimIdx == PLAYER_ANIM_ATTACK4) // 마지막 연계공격이면  무시
				return;

			// **핵심**: 현재 방향키 입력 상태에 따라 방향 업데이트
			ACTORDIR eCurrentDirection = m_pPlayer->Calculate_Direction();
			if (eCurrentDirection != ACTORDIR::END)
				m_eDir = eCurrentDirection; // 새로운 방향으로 업데이트

			//m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 2.f);

			// 다음 연계공격으로 변경.
			m_iNextAnimIdx = m_iCurAnimIdx + 1;
			m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
			Attack.iAnimation_Idx = m_iNextAnimIdx;
			Attack.eDirection = m_eDir; // **중요**: 현재 방향을 다음 공격에 전달

			m_pFsm->Change_State(m_iNextState, &Attack);
			return;
		}

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = PLAYER_ANIM_RUN;
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Run);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
		{
			// 해당 동작은 쿨타임이 있는경우 무시됨.
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
				return;

			m_iNextAnimIdx = PLAYER_ANIM_STRONG_ATTACK;
			m_iNextState = CPlayer::STRONG_ATTACK;
			StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &StrongAttack);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
		{
			// 해당 동작은 쿨타임이 있는경우 무시됨.
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
				return;

			OutPutDebugInt(m_iCurAnimIdx);

			m_iNextAnimIdx = PLAYER_ANIM_GUARD_START;
			m_iNextState = CPlayer::GUARD;
			Guard.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Guard);
			return;
		}

	}
}

//void CPlayer_AttackState::Handle_DirectionInput(_float fTimeDelta)
//{
//	// 방향 고정 시간 업데이트
//	m_fCurrentLockTime += fTimeDelta;
//	// 특정 시간이 지나면 방향을 고정
//	if (m_fCurrentLockTime >= m_fDirectionLockTime)
//	{
//		m_bIsDirectionLocked = true;
//		m_bCanChangeDirection = false;
//	}
//
//	// 방향 변경이 가능하고 움직임 키가 눌렸을 때
//	if (m_bCanChangeDirection && m_pPlayer->Is_MovementKeyPressed())
//	{
//		// 현재 입력된 방향 계산
//		ACTORDIR eNewDirection = m_pPlayer->Calculate_Direction();
//
//		if (eNewDirection != ACTORDIR::END)
//		{
//			m_eDir = eNewDirection;
//
//			// 해당 방향으로 이동 (공격 중에도 약간의 이동)
//			m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 0.5f); // 느린 속도
//		}
//	}
//	// 방향이 고정되지 않았고 움직임 키가 눌려있다면 계속 이동
//	else if (!m_bIsDirectionLocked && m_eDir != ACTORDIR::END)
//	{
//		// 기존 방향으로 계속 이동
//		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 1.f);
//	}
//
//}

void CPlayer_AttackState::Handle_DirectionInput(_float fTimeDelta)
{
	m_fCurrentLockTime += fTimeDelta;

	if (m_fCurrentLockTime >= m_fDirectionLockTime)
	{
		m_bIsDirectionLocked = true;
		m_bCanChangeDirection = false;
	}

	if (!m_bIsDirectionLocked)
	{
		// 플레이어 Look Vector 방향으로 공격하도록 변경
		Move_By_Player_LookVector(fTimeDelta, 0.4f);
	}
}

void CPlayer_AttackState::Move_By_Player_LookVector(_float fTimeDelta, _float fSpeed)
{
	// 1. 플레이어의 현재 Look Vector 가져오기
	_vector vPlayerLook = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);

	// 2. Y축 제거 (지면에서만 이동)
	vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	// 3. 키 입력이 있다면 플레이어를 해당 방향으로 회전시킨 후 이동
	if (m_pPlayer->Is_MovementKeyPressed())
	{
		// 키 입력에 따른 방향 계산 (카메라 기준)
		_vector vInputDirection = Calculate_Input_Direction_From_Camera();

		if (!XMVector3Equal(vInputDirection, XMVectorZero()))
		{
			// 플레이어를 입력 방향으로 회전
			Rotate_Player_To_Direction(vInputDirection, fTimeDelta);

			// 회전된 후의 새로운 Look Vector로 이동
			vPlayerLook = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
			vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
			vPlayerLook = XMVector3Normalize(vPlayerLook);
		}
	}

	// 4. 플레이어가 바라보는 방향으로 이동
	if (!XMVector3Equal(vPlayerLook, XMVectorZero()))
	{
		m_pPlayer->Get_Transform()->Move_Direction(vPlayerLook, fTimeDelta * fSpeed);
	}
}

_vector CPlayer_AttackState::Calculate_Input_Direction_From_Camera()
{
	// 기존 Player 클래스의 함수를 재사용
	ACTORDIR eInputDir = m_pPlayer->Calculate_Direction();
	return m_pPlayer->Calculate_Move_Direction(eInputDir);
}

void CPlayer_AttackState::Rotate_Player_To_Direction(_vector vTargetDirection, _float fTimeDelta)
{
	if (XMVector3Equal(vTargetDirection, XMVectorZero()))
		return;

	// 목표 방향 계산
	_float x = XMVectorGetX(vTargetDirection);
	_float z = XMVectorGetZ(vTargetDirection);
	_float fTargetYaw = atan2f(x, z);

	// 현재 회전 상태
	_float fCurrentYaw = m_pPlayer->Get_Transform()->GetYawFromQuaternion();
	_float fYawDiff = fTargetYaw - fCurrentYaw;

	// 최단 경로 계산
	while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
	while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

	// 공격 중에는 빠른 회전 (즉시 회전에 가깝게)
	_float fRotationSpeed = 12.0f; // 높은 회전 속도
	_float fMaxRotation = fRotationSpeed * fTimeDelta;

	if (fabsf(fYawDiff) > fMaxRotation)
	{
		fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
	}

	// 새로운 회전 적용
	_float fNewYaw = fCurrentYaw + fYawDiff;
	_vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
	m_pPlayer->Get_Transform()->Set_Quaternion(qNewRot);
}


CPlayer_AttackState* CPlayer_AttackState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_AttackState* pInstance = new CPlayer_AttackState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player Idle State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_AttackState::Free()
{
	__super::Free();
}
