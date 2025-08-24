#include "Player_IdleState.h"


static _uint iDebugCnt = 0;
CPlayer_IdleState::CPlayer_IdleState()
{
}

HRESULT CPlayer_IdleState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
		return E_FAIL;

	
	
	return S_OK;
}

/* State 시작 시*/
void CPlayer_IdleState::Enter(void* pArg)
{

	IDLE_ENTER_DESC* pDesc = static_cast<IDLE_ENTER_DESC*>(pArg);
	CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

	// Idle 시에는 RootMotion 비활성화
	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(false);

	

	m_iNextState = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();
	Change_State();
	CPlayerState::Handle_Collider_State();
}

// 종료될 때 실행할 동작..
void CPlayer_IdleState::Exit()
{
	if (m_iNextState != -1)
	{

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
	CPlayer_FirstSkillState::FIRSTSKILL_ENTER_DESC Skill{};

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::SKILL_1))
	{
		// 스킬 쿨타임은 Register_CoolTime() 함수에서 설정.
		if (!m_pFsm->Is_ExitCoolTimeEnd(CPlayer::SKILL_1))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::SKILL_1;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"));
		/* DIR 딱히 상관없을듯. */
		// ACTORDIR dir = m_pPlayer->Calculate_Direction();

		Skill.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(m_iNextState, &Skill);
	}

	

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE)) // 구르기.
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::DODGE))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::DODGE;
		
		// 락온 상태에 따라 방향별 Dodge 애니메이션 선택
		if (m_pPlayer->Is_LockOn())
		{
			// 락온시: 방향키에 따라 8방향 Dodge
			ACTORDIR dir = m_pPlayer->Calculate_Direction();
			switch (dir)
			{
			case ACTORDIR::U:   // W키: 전진
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
				break;
			case ACTORDIR::D:   // S키: 후진
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_B"));
				break;
			case ACTORDIR::L:   // A키: 좌측
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_L"));
				break;
			case ACTORDIR::R:   // D키: 우측
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_R"));
				break;
			case ACTORDIR::LU:  // W+A: 좌상향
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_FL"));
				break;
			case ACTORDIR::LD:  // S+A: 좌하향  
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_BL"));
				break;
			case ACTORDIR::RU:  // W+D: 우상향
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_FR"));
				break;
			case ACTORDIR::RD:  // S+D: 우하향
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_BR"));
				break;
			default:
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
				break;
			}
			Dodge.eDirection = dir;
		}
		else
		{
			// 일반 상태: 기본 Dodge (전진)
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
			Dodge.eDirection = ACTORDIR::U;
		}
		
		Dodge.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(m_iNextState, &Dodge);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::STRONG_ATTACK;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1"));
		StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
		
		// 락온 중이면 방향 무시, 일반 상태면 기본 방향 사용
		if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
		{
			StrongAttack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (StrongAttackState에서 타겟 방향으로 회전)
		}
		else
		{
			StrongAttack.eDirection = m_eDir; // 일반 상태에서는 기본 방향 사용
		}
		
		m_pFsm->Change_State(m_iNextState, &StrongAttack);
		return;
	}

	if (m_pPlayer->Is_KeyUp(PLAYER_KEY::GUARD))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::GUARD;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START"));
		Guard.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(m_iNextState, &Guard);
		return;

	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
		Attack.iAnimation_Idx = m_iNextAnimIdx;
		
		// 락온 중이면 방향 무시, 일반 상태면 기본 방향 사용
		if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
			Attack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (AttackState에서 타겟 방향으로 회전)
		else
			Attack.eDirection = ACTORDIR::U; // 일반 상태에서도 전진 공격
		
		m_pFsm->Change_State(m_iNextState, &Attack);
		return;
	}

	if (m_pPlayer->Is_MovementKeyPressed())
	{
		// LockOn 상태에 따라 WALK 또는 RUN으로 분기
		if (m_pPlayer->Is_LockOn())
		{
			// LockOn 상태: WALK 상태로 전환
			m_iNextState = CPlayer::PLAYER_STATE::WALK;
			
			// 방향에 따라 초기 애니메이션 결정
			ACTORDIR dir = m_pPlayer->Calculate_Direction();
			switch (dir)
			{
			case ACTORDIR::U:   // W키: 전진
			case ACTORDIR::LU:  // W+A: 좌상향
			case ACTORDIR::RU:  // W+D: 우상향
				Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
				break;
			case ACTORDIR::D:   // S키: 후진
			case ACTORDIR::LD:  // S+A: 좌하향  
			case ACTORDIR::RD:  // S+D: 우하향
				Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_B"));
				break;
			case ACTORDIR::L:   // A키: 좌측
				Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_L"));
				break;
			case ACTORDIR::R:   // D키: 우측
				Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_R"));
				break;
			default:
				Walk.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
				break;
			}
			
			m_pFsm->Change_State(m_iNextState, &Walk);
		}
		else
		{
			// 일반 상태: RUN 상태로 전환
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
			Run.eDirection = m_eDir;
			m_pFsm->Change_State(m_iNextState, &Run);
		}
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
	CPlayerState::Free();
}
