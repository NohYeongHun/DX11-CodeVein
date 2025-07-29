#include "Player_IdleState.h"


static _uint iDebugCnt = 0;
CPlayer_IdleState::CPlayer_IdleState()
{
}

HRESULT CPlayer_IdleState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	
	
	return S_OK;
}

/* State 시작 시*/
void CPlayer_IdleState::Enter(void* pArg)
{
	// ✅ 구문 수정
	//_vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);
	//OutputDebugString((L"[IDLE ENTER] Position: " +
	//	std::to_wstring(XMVectorGetX(vPos)) + L", " +
	//	std::to_wstring(XMVectorGetZ(vPos)) + L"\n").c_str());

	IDLE_ENTER_DESC* pDesc = static_cast<IDLE_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

	// Idle 시에는 RootMotion 비활성화

	m_iNextState = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);


}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();
	Change_State();
		
}

// 종료될 때 실행할 동작..
void CPlayer_IdleState::Exit()
{
	if (m_iNextState != -1)
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::STRONG_ATTACK)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
		else if (m_iNextState == CPlayer::PLAYER_STATE::DODGE)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		else if (m_iNextState == CPlayer::GUARD)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		else if (m_iNextState == CPlayer::PLAYER_STATE::ATTACK)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
	}
}

// 상태 초기화
void CPlayer_IdleState::Reset()
{
	m_iNextState = -1;
	m_eDir = { ACTORDIR::END };
}

void CPlayer_IdleState::Change_State()
{
	CPlayer_WalkState::WALK_ENTER_DESC Walk{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};


	

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE)) // 구르기.
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::DODGE))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::DODGE;
		Dodge.iAnimation_Idx = PLAYER_ANIM_DODGE;
		m_iNextAnimIdx = PLAYER_ANIM_DODGE;
		m_pFsm->Change_State(m_iNextState, &Dodge);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::STRONG_ATTACK;
		m_iNextAnimIdx = PLAYER_ANIM_STRONG_ATTACK;
		StrongAttack.iAnimation_Idx = PLAYER_ANIM_STRONG_ATTACK;
		StrongAttack.eDirection = m_eDir;
		m_pFsm->Change_State(m_iNextState, &StrongAttack);
		return;
	}

	if (m_pPlayer->Is_KeyUp(PLAYER_KEY::GUARD))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::GUARD;
		m_iNextAnimIdx = PLAYER_ANIM_GUARD_START;
		Guard.iAnimation_Idx = PLAYER_ANIM_GUARD_START;
		m_pFsm->Change_State(m_iNextState, &Guard);
		return;

	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
		m_iNextAnimIdx = PLAYER_ANIM_ATTACK1;
		Attack.iAnimation_Idx = PLAYER_ANIM_ATTACK1;
		m_pFsm->Change_State(m_iNextState, &Attack);
		return;
	}

	if (m_pPlayer->Is_MovementKeyPressed())
	{
		m_iNextState = CPlayer::PLAYER_STATE::RUN;
		Run.iAnimation_Idx = PLAYER_ANIM_RUN;
		Run.eDirection = m_eDir;
		m_pFsm->Change_State(m_iNextState, &Run);
		return;
	}
	
}


CPlayer_IdleState* CPlayer_IdleState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_IdleState* pInstance = new CPlayer_IdleState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player Idle State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_IdleState::Free()
{
	__super::Free();
}
