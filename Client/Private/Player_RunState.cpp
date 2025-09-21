#include "Player_RunState.h"

CPlayer_RunState::CPlayer_RunState()
{
}

HRESULT CPlayer_RunState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_pModelCom->Get_Current_Ratio();

	m_fFootStepFirst = 18.f / 47.f;
	m_fFootStepSecond = 38.f / 47.f;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_RunState::Enter(void* pArg)
{
	RUN_ENTER_DESC* pDesc = static_cast<RUN_ENTER_DESC*>(pArg);
	CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

	m_iNextState = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;

	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(false);

	_float4 vPos = {};

	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);


	//m_pGameInstance->PlaySoundEffect(L"FootSound1.wav", 0.3f);
	
}

/* State 실행 */
void CPlayer_RunState::Update(_float fTimeDelta)
{
	Handle_Input();

	Update_Sound(fTimeDelta);
	Change_State(fTimeDelta);
	CPlayerState::Handle_Collider_State();
}

void CPlayer_RunState::Update_Sound(_float fTimeDelta)
{

	_float fCurrentRatio = m_pModelCom->Get_Current_Ratio();

	if (!m_bFirstSoundPlayed && fCurrentRatio > m_fFootStepFirst)
	{
		m_strFootSoundFile = L"FootSound1.wav";
		m_pGameInstance->PlaySoundEffect(m_strFootSoundFile, 0.3f);
		m_bFirstSoundPlayed = true;
	}
	else if (!m_bSecondSoundPlayed  && fCurrentRatio > m_fFootStepSecond)
	{
		m_strFootSoundFile = L"FootSound1.wav";
		m_pGameInstance->PlaySoundEffect(m_strFootSoundFile, 0.3f);
		m_bSecondSoundPlayed = true;
	}

	// 애니메이션 리셋 시 플래그 초기화
	if (fCurrentRatio < 0.1f)
	{
		m_bFirstSoundPlayed = false;
		m_bSecondSoundPlayed = false;
	}
}



// 종료될 때 실행할 동작..
void CPlayer_RunState::Exit()
{
	// 보간 정보를 Model에 전달한다.
	if (m_iNextState != -1)
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::DODGE ||
			m_iNextState == CPlayer::PLAYER_STATE::ATTACK ||
			m_iNextState == CPlayer::PLAYER_STATE::STRONG_ATTACK ||
			m_iNextState == CPlayer::PLAYER_STATE::GUARD)
		{
			return;
		}

		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
	}

		
	m_bFirstSoundPlayed = false;
	m_bSecondSoundPlayed = false;
}

// 상태 초기화
void CPlayer_RunState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	//m_pModelCom->Animation_Reset();
}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_RunState::Change_State(_float fTimeDelta)
{

	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};
	CPlayer_FirstSkillState::FIRSTSKILL_ENTER_DESC FirstSkill{};
	CPlayer_SecondSkillState::SECONDSKILL_ENTER_DESC SecondSkill{};

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::SKILL_1))
	{
		// 스킬 쿨타임은 Register_CoolTime() 함수에서 설정.
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::SKILL_1))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::SKILL_1;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("CIRCULATE_PURGE"));

		FirstSkill.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(m_iNextState, &FirstSkill);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::SKILL_2))
	{
		// 스킬 쿨타임은 Register_CoolTime() 함수에서 설정.
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::SKILL_2))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::SKILL_2;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("DRAGON_LUNGE"));

		SecondSkill.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(m_iNextState, &SecondSkill);
		return;
	}

	

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE)) // 구르기.
	{
		// 구르기는 쿨타임이 너무 길면안됨.
		if (!m_pFsm->Is_ExitCoolTimeEnd(CPlayer::DODGE))
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
		
		// 락온 중이면 방향 무시, 일반 상태면 이동 방향 사용
		if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
		{
			StrongAttack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (StrongAttackState에서 타겟 방향으로 회전)
		}
		else
		{
			StrongAttack.eDirection = m_eDir; // 일반 상태에서는 이동 방향 사용
		}
		
		m_pFsm->Change_State(m_iNextState, &StrongAttack);
		return;
	}

	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
	{
		if (!m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
			return;

		m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
		Attack.iAnimation_Idx = m_iNextAnimIdx;
		
		// 락온 중이면 방향 무시, 일반 상태면 이동 방향 사용
		if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
		{
			Attack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (AttackState에서 타겟 방향으로 회전)
		}
		else
		{
			Attack.eDirection = m_eDir; // 일반 상태에서는 이동 방향 사용
		}
		
		m_pFsm->Change_State(m_iNextState, &Attack);
		return;
	}

	//if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
	//{
	//	//달리는 도중에 바꾸면 동작이 어색함.
	//	if (!m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
	//		return;

	//	m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START"));
	//	m_iNextState = CPlayer::PLAYER_STATE::GUARD;
	//	Guard.iAnimation_Idx = m_iNextAnimIdx;
	//	m_pFsm->Change_State(m_iNextState, &Guard);
	//	return;
	//}
	

	if (m_pPlayer->Is_MovementKeyPressed()) // 입력키 이용 중이라면.
	{
		// LockOn 상태로 변경되었으면 WALK로 전환
		if (m_pPlayer->Is_LockOn())
		{
			CPlayer_WalkState::WALK_ENTER_DESC Walk{};
			
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
			
			m_iNextState = CPlayer::PLAYER_STATE::WALK;
			m_iNextAnimIdx = Walk.iAnimation_Idx;
			m_pFsm->Change_State(m_iNextState, &Walk);
			return;
		}
		
		// 일반 상태: 계속 달리기
		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 1.f);
		return;
	}
	else
	{
		
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		Idle.iAnimation_Idx = m_iNextAnimIdx;
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
	CPlayerState::Free();
}