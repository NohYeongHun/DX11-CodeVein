#include "Player_RunState.h"

CPlayer_RunState::CPlayer_RunState()
{
}

HRESULT CPlayer_RunState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;
	return S_OK;
}

/* State 시작 시*/
void CPlayer_RunState::Enter(void* pArg)
{
	RUN_ENTER_DESC* pDesc = static_cast<RUN_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

	m_iNextState = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;

	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_RunState::Update(_float fTimeDelta)
{
	Handle_Input();
	Change_State(fTimeDelta);
}

// 종료될 때 실행할 동작..
void CPlayer_RunState::Exit()
{
	// 보간 정보를 Model에 전달한다.
	if (m_iNextState != -1)
	{
		

		if (m_iNextState == CPlayer::GUARD)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		else if (m_iNextState == CPlayer::PLAYER_STATE::DODGE)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
		else
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
	}
		
}

// 상태 초기화
void CPlayer_RunState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_pModelCom->Animation_Reset();
}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_RunState::Change_State(_float fTimeDelta)
{

	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};

	

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE)) // 구르기.
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::DODGE))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::DODGE;
		Dodge.iAnimation_Idx = 25;
		m_iNextAnimIdx = 25;
		m_pFsm->Change_State(m_iNextState, &Dodge);

		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
			return;

		StrongAttack.iAnimation_Idx = 38;
		m_iNextState = CPlayer::PLAYER_STATE::STRONG_ATTACK;
		m_iNextAnimIdx = 48;
		m_pFsm->Change_State(m_iNextState, &StrongAttack);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
		m_iNextAnimIdx = 32;
		Attack.iAnimation_Idx = 32;
		m_pFsm->Change_State(m_iNextState, &Attack);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
	{
		//달리는 도중에 바꾸면 동작이 어색함.
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
			return;

		Guard.iAnimation_Idx = 30;
		m_iNextState = CPlayer::PLAYER_STATE::GUARD;
		m_iNextAnimIdx = 30;
		m_pFsm->Change_State(m_iNextState, &Guard);
		return;
	}
	

	if (m_pPlayer->Is_MovementKeyPressed()) // 입력키 이용 중이라면.
	{
		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 1.f);
		return;
	}
	else
	{
		Idle.iAnimation_Idx = 16;
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = 16;
		m_pFsm->Change_State(m_iNextState, &Idle);
		return;
	}
	
}


CPlayer_RunState* CPlayer_RunState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_RunState* pInstance = new CPlayer_RunState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player Idle State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_RunState::Free()
{
	__super::Free();
}