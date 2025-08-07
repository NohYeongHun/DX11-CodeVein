#include "Player_StrongAttackState.h"

CPlayer_StrongAttackState::CPlayer_StrongAttackState()
{
}

HRESULT CPlayer_StrongAttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_StrongAttackState::Enter(void* pArg)
{
	STRONG_ENTER_DESC* pDesc = static_cast<STRONG_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

	m_isLoop = false;

			
	m_iNextAnimIdx = -1;
	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(true);


	// 이 때 검에 콜라이더 활성화 이런 과정 진행
}

/* State 실행 */
void CPlayer_StrongAttackState::Update(_float fTimeDelta)
{
	Handle_Input();
	Handle_Unified_Direction_Input(fTimeDelta);
	Change_State();

	CPlayerState::Handle_Collider_State();
}

// 종료될 때 실행할 동작..
void CPlayer_StrongAttackState::Exit()
{
	

	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		//// 루트모션은 블렌딩안하기.
		//if (m_iNextState == CPlayer::PLAYER_STATE::STRONG_ATTACK ||
		//	m_iNextState == CPlayer::PLAYER_STATE::ATTACK ||
		//	m_iNextState == CPlayer::PLAYER_STATE::DODGE ||
		//	m_iNextState == CPlayer::PLAYER_STATE::GUARD
		//	)
		//{
		//	return;
		//}
		//else 
		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);	
	}

	
}

// 상태 초기화
void CPlayer_StrongAttackState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextAnimIdx = -1;
	m_pModelCom->Animation_Reset();
}

void CPlayer_StrongAttackState::Change_State()
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};

	
	if (m_pModelCom->Is_Finished())
	{
		/*m_iNextAnimIdx = PLAYER_ANIM_IDLE_SWORD;
		Idle.iAnimation_Idx = PLAYER_ANIM_IDLE_SWORD;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);*/

		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
		Attack.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::ATTACK, &Attack);
		return;
	}

	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{


		//if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
		//{
		//	if (m_iCurAnimIdx != m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1")))
		//		return;

		//	/*if (m_iCurAnimIdx != PLAYER_ANIM_SPECIAL_LAUNCH)
		//		return;*/

		//	m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK2"));
		//	m_iNextState = CPlayer::STRONG_ATTACK;
		//	StrongAttack.iAnimation_Idx = m_iNextAnimIdx;
		//	m_pFsm->Change_State(m_iNextState, &StrongAttack);
		//	return;
		//}

		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Run);
			return;
		}
		
		/*if (m_iCurAnimIdx == m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1")) &&
			m_pModelCom->Get_Current_Ratio() > 0.55f)
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
			Attack.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::ATTACK, &Attack);
			return;
		}*/

		
	}
}


CPlayer_StrongAttackState* CPlayer_StrongAttackState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_StrongAttackState* pInstance = new CPlayer_StrongAttackState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player Idle State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_StrongAttackState::Free()
{
	__super::Free();
}
