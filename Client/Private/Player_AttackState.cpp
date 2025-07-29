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

	
	
	// 디버그 출력
	//_wstring debugMsg = L"[COMBO] Step " + to_wstring(m_iCurrentComboStep) +
	//	L", AnimIdx: " + to_wstring(m_iCurAnimIdx) + L"\n";
	//OutputDebugString(debugMsg.c_str());
}

/* State 실행 */
void CPlayer_AttackState::Update(_float fTimeDelta)
{
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
		else
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.1f, true, true, true);
		}
		
	}
	
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
		m_iNextAnimIdx = 16;
		Idle.iAnimation_Idx = 16;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);

		return;
	}


	// 현재 모션이 최소한 Exit CoolTime보다 진행되었을 경우.
	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			if (m_iCurAnimIdx == 34) // 마지막 연계공격이면  무시
				return;

			// 다음 연계공격으로 변경.
			m_iNextAnimIdx = m_iCurAnimIdx + 1;
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
