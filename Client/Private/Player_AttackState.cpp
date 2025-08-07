#include "Player_StrongAttackState.h"
#include "Player_AttackState.h"

CPlayer_AttackState::CPlayer_AttackState()
{
}

HRESULT CPlayer_AttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_ColliderActiveMap.emplace(m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"))
		, COLLIDER_ACTIVE_INFO{0.f, 0.3f, false});

	m_ColliderActiveMap.emplace(m_pPlayer->Find_AnimationIndex(TEXT("ATTACK2"))
		, COLLIDER_ACTIVE_INFO{ 0.f, 0.28f, false });

	m_ColliderActiveMap.emplace(m_pPlayer->Find_AnimationIndex(TEXT("ATTACK3"))
		, COLLIDER_ACTIVE_INFO{ 0.f, 0.25f, false });

	m_ColliderActiveMap.emplace(m_pPlayer->Find_AnimationIndex(TEXT("ATTACK4"))
		, COLLIDER_ACTIVE_INFO{ 0.f, 0.27f, false });
	

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
	
	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(true);
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
	
	// 콜라이더 활성화 정보 초기화
	CPlayerState::Reset_ColliderActiveInfo();


	// 방향 제어 관련 초기화
	m_bCanChangeDirection = true;
	m_fCurrentLockTime = 0.0f;
	m_bIsDirectionLocked = false;


	
	// 애니메이션이 존재할때 언제부터 활성화되어야 하는가에 대한 결정을 애니메이션이?
}

/* State 실행 */
void CPlayer_AttackState::Update(_float fTimeDelta)
{
	Handle_Input();
	Handle_Unified_Direction_Input(fTimeDelta);
	Change_State(fTimeDelta);

	CPlayerState::Handle_Collider_State();
	
}

// 종료될 때 실행할 동작..
void CPlayer_AttackState::Exit()
{
	if (m_iNextState != -1)
	{
		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
	}

	//m_pFsm->Set_StateCoolTime(CPlayer::DODGE, 0.1f);
	
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
	CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};

	// 애니메이션이 끝났을 때
	if (m_pModelCom->Is_Finished())
	{
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		Idle.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
		return;
	}

	// 현재 모션이 최소한 Exit CoolTime보다 진행되었을 경우
	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			if (m_iCurAnimIdx == m_pPlayer->Find_AnimationIndex(TEXT("ATTACK4"))) // 마지막 연계공격이면 무시
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
			if (m_iCurAnimIdx != m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1")))
				return;

			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
				return;

			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1"));
			m_iNextState = CPlayer::STRONG_ATTACK;
			StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &StrongAttack);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
		{
			if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
				return;

			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START"));
			m_iNextState = CPlayer::GUARD;
			Guard.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Guard);
			return;
		}

		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE))
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE"));
			m_iNextState = CPlayer::PLAYER_STATE::DODGE;
			Dodge.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Dodge);
			return;
		}

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Run);
			return;
		}
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
