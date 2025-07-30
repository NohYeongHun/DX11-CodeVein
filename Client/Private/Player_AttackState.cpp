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
	m_pModelCom->Set_RootMotionTranslate(true);
	m_pModelCom->Set_RootMotionRotation(true);


	// 방향 제어 관련 초기화
	m_bCanChangeDirection = true;
	m_fCurrentLockTime = 0.0f;
	m_bIsDirectionLocked = false;
	
}

/* State 실행 */
void CPlayer_AttackState::Update(_float fTimeDelta)
{
	Handle_Input();
	Handle_Unified_Direction_Input(fTimeDelta);
	//Handle_DirectionInput(fTimeDelta);
	
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
		else if (m_iNextState == CPlayer::PLAYER_STATE::STRONG_ATTACK)
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
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
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = PLAYER_ANIM_IDLE_SWORD;
		Idle.iAnimation_Idx = PLAYER_ANIM_IDLE_SWORD;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
		return;
	}

	// 현재 모션이 최소한 Exit CoolTime보다 진행되었을 경우
	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			if (m_iCurAnimIdx == PLAYER_ANIM_ATTACK4) // 마지막 연계공격이면 무시
				return;

			// 다음 연계공격으로 변경
			m_iNextAnimIdx = m_iCurAnimIdx + 1;
			m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
			Attack.iAnimation_Idx = m_iNextAnimIdx;
			// ⭐ 방향은 AttackState에서 자동으로 계산되므로 전달하지 않음
			Attack.eDirection = ACTORDIR::END;

			m_pFsm->Change_State(m_iNextState, &Attack);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
		{
			if (m_iCurAnimIdx != PLAYER_ANIM_ATTACK2)
				return;

			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
				return;

			m_iNextAnimIdx = PLAYER_ANIM_SPECIAL_DOWN3;
			m_iNextState = CPlayer::STRONG_ATTACK;
			StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &StrongAttack);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
		{
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
				return;

			m_iNextAnimIdx = PLAYER_ANIM_GUARD_START;
			m_iNextState = CPlayer::GUARD;
			Guard.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Guard);
			return;
		}

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = PLAYER_ANIM_RUN_F_LOOP;
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Run);
			return;
		}
	}
}

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
		// ⭐ 키 입력이 있으면 그 방향으로 이동하면서 공격
		if (m_pPlayer->Is_MovementKeyPressed())
		{
			ACTORDIR eCurrentDirection = m_pPlayer->Calculate_Direction();
			if (eCurrentDirection != ACTORDIR::END)
			{
				m_eDir = eCurrentDirection;
				m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 0.5f); // 이동 속도 조절
			}
		}
		else
		{
			// 키 입력이 없으면 플레이어 Look Vector 방향으로 이동
			Move_By_Player_LookVector(fTimeDelta, 0.4f);
		}
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
			// 회전된 후의 새로운 Look Vector로 이동
			vPlayerLook = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
			vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
			vPlayerLook = XMVector3Normalize(vPlayerLook);
		}
		return;
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
