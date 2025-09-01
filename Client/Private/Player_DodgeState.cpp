
CPlayer_DodgeState::CPlayer_DodgeState()
{
}

HRESULT CPlayer_DodgeState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_isLoop = false;

	// 버프 시스템을 사용하므로 충돌체 맵은 필요 없음

	return S_OK;
}

/* State 시작 시*/
void CPlayer_DodgeState::Enter(void* pArg)
{

  	DODGE_ENTER_DESC* pDesc = static_cast<DODGE_ENTER_DESC*>(pArg);
	CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection; // 회피 방향 저장

	// ⭐ Dodge는 non-loop으로 변경
	m_isLoop = false;
	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(true);

	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

	m_pPlayer->AddBuff(CPlayer::BUFF_INVINCIBLE);

}

/* State 실행 */
void CPlayer_DodgeState::Update(_float fTimeDelta)
{
	Handle_Input();

	if (m_pModelCom->Get_Current_Ratio() < 0.4f)
	{
		// 락온시 타겟을 바라보면서 Dodge
		if (m_pPlayer->Is_LockOn())
		{
			m_pPlayer->Rotate_To_LockOn_Target(fTimeDelta, 8.0f);
			
			// 락온시: 애니메이션 방향에 맞게 이동
			_vector vMoveDir = m_pPlayer->Calculate_Move_Direction(m_eDir);
			m_pPlayer->Move_Direction(vMoveDir * 0.4f, fTimeDelta);
		}
		else
		{
			// 일반 상태: 앞 방향으로 이동 (기존 방식)
			m_pPlayer->Move_Direction(m_pPlayer->Get_Transform()->Get_LookDirection_NoPitch(), fTimeDelta * 0.5f);
		}
	}
	else
	{
		Change_State();
	}





	// 무적 버프 관리
	Handle_Invincible_Buff();
}

// 종료될 때 실행할 동작.=> 왠만하면 Idle
void CPlayer_DodgeState::Exit()
{
	// Dodge 종료시 무적 버프 해제
	m_pPlayer->RemoveBuff(CPlayer::BUFF_INVINCIBLE, true);
	
	//여기서 동작해야합니다.
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::DODGE ||
			m_iNextState == CPlayer::PLAYER_STATE::STRONG_ATTACK || 
			m_iNextState == CPlayer::PLAYER_STATE::ATTACK ||
			m_iNextState == CPlayer::PLAYER_STATE::GUARD
			)
		{
			//m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
			return;
		}

		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
	}

}

void CPlayer_DodgeState::Handle_Invincible_Buff()
{
	// 현재 애니메이션의 재생 비율 가져오기
	_float fCurrentRatio = m_pModelCom->Get_Current_Ratio();

	// 10% ~ 50% 구간에서 무적 버프 적용
	_float fInvincibleStart = 0.1f;  // 10% 지점부터 무적 시작
	_float fInvincibleEnd = 0.5f;    // 50% 지점까지 무적 지속

	_bool bShouldInvincible = (fCurrentRatio >= fInvincibleStart && fCurrentRatio <= fInvincibleEnd);

	if (bShouldInvincible != m_bPrevInvincible)
	{
		if (bShouldInvincible)
		{
			// 무적 버프 시작
			m_pPlayer->AddBuff(CPlayer::BUFF_INVINCIBLE);
		}
		else
		{
			// 무적 버프 종료
			m_pPlayer->RemoveBuff(CPlayer::BUFF_INVINCIBLE, true);
		}

		m_bPrevInvincible = bShouldInvincible;
	}
}

// 상태 초기화
void CPlayer_DodgeState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = -1;
	//m_pModelCom->Animation_Reset();
}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_DodgeState::Change_State()
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};
	CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};



	if (m_pModelCom->Is_Finished())
	{
		// Idle 상태로 전환
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		Idle.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
		return;
	}

	if (m_pModelCom->Get_Current_Ratio() < 0.6f) // 60% 진행 전까지는 상태 변경 불가
		return;

	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			// 해당 동작은 쿨타임이 있는경우 무시됨.
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
				return;

			// 다음 연계공격으로 변경.
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
			m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
			Attack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Attack);
			return;
		}

	/*	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE))
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE"));
			m_iNextState = CPlayer::PLAYER_STATE::DODGE;
			Dodge.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Dodge);
			return;
		}*/

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			// 락온 상태에 따라 RUN 또는 WALK 전환
			if (m_pPlayer->Is_LockOn())
			{
				// 락온시: WALK 상태로 전환
				CPlayer_WalkState::WALK_ENTER_DESC Walk{};
				
				// 방향에 따라 초기 애니메이션 결정
				ACTORDIR dir = m_pPlayer->Calculate_Direction();
				switch (dir)
				{
				case ACTORDIR::U:
				case ACTORDIR::LU:
				case ACTORDIR::RU:
					Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
					break;
				case ACTORDIR::D:
				case ACTORDIR::LD:
				case ACTORDIR::RD:
					Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_B"));
					break;
				case ACTORDIR::L:
					Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_L"));
					break;
				case ACTORDIR::R:
					Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_R"));
					break;
				default:
					Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
					break;
				}
				
				m_iNextState = CPlayer::PLAYER_STATE::WALK;
				m_iNextAnimIdx = Walk.iAnimation_Idx;
				m_pFsm->Change_State(m_iNextState, &Walk);
			}
			else
			{
				// 일반 상태: RUN 상태로 전환
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
				m_iNextState = CPlayer::PLAYER_STATE::RUN;
				Run.iAnimation_Idx = m_iNextAnimIdx;
				m_pFsm->Change_State(m_iNextState, &Run);
			}
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
		{
			// 해당 동작은 쿨타임이 있는경우 무시됨.
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
				return;

			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START"));
			m_iNextState = CPlayer::GUARD;
			StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Guard);
			return;
		}
	}


}


CPlayer_DodgeState* CPlayer_DodgeState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_DodgeState* pInstance = new CPlayer_DodgeState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player DODGE State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_DodgeState::Free()
{
	CPlayerState::Free();
}