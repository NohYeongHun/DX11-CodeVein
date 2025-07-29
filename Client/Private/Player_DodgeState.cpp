
CPlayer_DodgeState::CPlayer_DodgeState()
{
}

HRESULT CPlayer_DodgeState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_isLoop = false;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_DodgeState::Enter(void* pArg)
{

  	DODGE_ENTER_DESC* pDesc = static_cast<DODGE_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;

	// ⭐ Dodge는 non-loop으로 변경
	m_isLoop = false;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(true);
}

/* State 실행 */
void CPlayer_DodgeState::Update(_float fTimeDelta)
{
	Handle_Input();
	Change_State();
}

/*
* 이 문제는 애니메이션 제작 시점의 좌표 기준과 런타임 블렌딩 시점의 좌표 기준이 다르기 때문에 
발생하는 근본적인 좌표계 불일치 문제입니다. 
구르기 같은 이동 애니메이션과 Idle 같은 제자리 애니메이션 간의 
공간적 연속성을 보장하는 추가적인 처리가 필요한 상황입니다.

=> 로컬좌표가 다른 루트 모션간에 블렌딩하는 방법?
*/
// 종료될 때 실행할 동작.=> 왠만하면 Idle
void CPlayer_DodgeState::Exit()
{
	//여기서 동작해야합니다.
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
	}

	m_pModelCom->Set_RootMotionRotation(false);
	m_pModelCom->Set_RootMotionTranslate(false);
		
}

// 상태 초기화
void CPlayer_DodgeState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = -1;
	m_pModelCom->Animation_Reset();
}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_DodgeState::Change_State()
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};


	if (m_pModelCom->Is_Finished())
	{
		// Idle 상태로 전환
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = 16;
		Idle.iAnimation_Idx = 16;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
		return;
	}


	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			// 해당 동작은 쿨타임이 있는경우 무시됨.
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
				return;

			// 다음 연계공격으로 변경.
			m_iNextAnimIdx = 32;;
			m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
			Attack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Attack);
			return;
		}

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = 6;
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

			m_iNextAnimIdx = 48;
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

			m_iNextAnimIdx = 30;
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
	__super::Free();
}