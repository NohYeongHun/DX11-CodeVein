#include "Player_WalkState.h"


CPlayer_WalkState::CPlayer_WalkState()
{
}

HRESULT CPlayer_WalkState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
		return E_FAIL;


	m_fFootStepFirst = 20.f / 47.f;
	m_fFootStepSecond = 40.f / 47.f;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_WalkState::Enter(void* pArg)
{
	WALK_ENTER_DESC* pDesc = static_cast<WALK_ENTER_DESC*>(pArg);
	CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

	// 애니메이션 인덱스를 변경해줍니다.
	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
}

// LockOn일 때만 Walk 상태
/* State 실행 */
void CPlayer_WalkState::Update(_float fTimeDelta)
{
	if (!m_isLoop)
		CRASH("m_is Loop Failed");

	// 입력 처리 (공격, Dodge 등 모든 입력을 Handle_Input에서 처리)
	Handle_Input(fTimeDelta);

	if (m_pPlayer->Is_MovementKeyPressed())
	{
		if (m_pPlayer->Is_LockOn())
		{
			// 락온시: 타겟을 바라보면서 애니메이션 방향에 맞게 이동
			m_pPlayer->Rotate_To_LockOn_Target(fTimeDelta, 5.0f);
			
			_vector vMoveDir = m_pPlayer->Calculate_Move_Direction(m_eDir);
			if (m_iCurAnimIdx == m_pPlayer->Find_AnimationIndex(TEXT("RUN_F")))
				vMoveDir *= 1.5f;

			m_pPlayer->Move_Direction(vMoveDir * 0.5f, fTimeDelta);
		}
		else
		{
			// 일반 이동: 카메라 기준 회전하며 이동
			m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 0.5f);
		}
	}
	else
	{
		CPlayer_IdleState::IDLE_ENTER_DESC Desc{};
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		Desc.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Desc);
		
	}		

	Update_FootstepSound(fTimeDelta);

	CPlayerState::Handle_Collider_State();
}

void CPlayer_WalkState::Update_FootstepSound(_float fTimeDelta)
{
	_float fCurrentRatio = m_pModelCom->Get_Current_Ratio();

	if (!m_bFirstSoundPlayed && fCurrentRatio > m_fFootStepFirst)
	{
		m_strFootSoundFile = L"FootSound1.wav";
		m_pGameInstance->PlaySoundEffect(m_strFootSoundFile, 0.3f);
		m_bFirstSoundPlayed = true;
	}
	else if (!m_bSecondSoundPlayed && fCurrentRatio > m_fFootStepSecond)
	{
		m_strFootSoundFile = L"FootSound2.wav";
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
void CPlayer_WalkState::Exit()
{
	if (m_iNextState != -1)
		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.5f, true, true, true);


}

// 상태 초기화
void CPlayer_WalkState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextAnimIdx = -1;
	//m_pModelCom->Animation_Reset();
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_WalkState::Handle_Input(_float fTimeDelta)
{
	// Dodge 체크 (최고 우선순위)
	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::DODGE))
	{
		if (m_pFsm->Is_ExitCoolTimeEnd(CPlayer::DODGE))
		{
			CPlayer_DodgeState::DODGE_ENTER_DESC Dodge{};
			
			// 락온 상태에 따라 방향별 Dodge 애니메이션 선택
			if (m_pPlayer->Is_LockOn())
			{
				// 락온시: 방향키에 따라 8방향 Dodge
				ACTORDIR dir = m_pPlayer->Calculate_Direction();
				switch (dir)
				{
				case ACTORDIR::U:   // W키: 전진
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
					break;
				case ACTORDIR::D:   // S키: 후진
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_B"));
					break;
				case ACTORDIR::L:   // A키: 좌측
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_L"));
					break;
				case ACTORDIR::R:   // D키: 우측
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_R"));
					break;
				case ACTORDIR::LU:  // W+A: 좌상향
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_FL"));
					break;
				case ACTORDIR::LD:  // S+A: 좌하향  
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_BL"));
					break;
				case ACTORDIR::RU:  // W+D: 우상향
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_FR"));
					break;
				case ACTORDIR::RD:  // S+D: 우하향
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_BR"));
					break;
				default:
					Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
					break;
				}
				Dodge.eDirection = dir;
			}
			else
			{
				// 일반 상태: 기본 Dodge (전진)
				Dodge.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("DODGE_F"));
				Dodge.eDirection = ACTORDIR::U;
			}
			
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::DODGE, &Dodge);
			return;
		}
	}

	// 공격 입력 체크 (이동보다 우선순위 높음) - 방향 업데이트 전에 실행
	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
	{
		if (m_pFsm->Is_CoolTimeEnd(CPlayer::ATTACK))
		{
			CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
			Attack.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
			
			// 락온 중이면 항상 타겟 방향으로 공격
			if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
			{
				Attack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (AttackState에서 타겟 방향으로 회전)
			}
			else
			{
				// 일반 상태에서는 현재 입력 방향 사용 (실시간 방향 체크)
				ACTORDIR currentInputDir = m_pPlayer->Calculate_Direction();
				Attack.eDirection = (currentInputDir != ACTORDIR::END) ? currentInputDir : m_eDir;
			}
			
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::ATTACK, &Attack);
			return;
		}
	}

	// 강공격 입력 체크 - 방향 업데이트 전에 실행
	if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::STRONG_ATTACK))
	{
		if (m_pFsm->Is_CoolTimeEnd(CPlayer::STRONG_ATTACK))
		{
			CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
			StrongAttack.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1"));
			
			// 락온 중이면 항상 타겟 방향으로 공격
			if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
			{
				StrongAttack.eDirection = ACTORDIR::U; // 락온 중에는 방향 무시 (StrongAttackState에서 타겟 방향으로 회전)
			}
			else
			{
				// 일반 상태에서는 현재 입력 방향 사용 (실시간 방향 체크)
				ACTORDIR currentInputDir = m_pPlayer->Calculate_Direction();
				StrongAttack.eDirection = (currentInputDir != ACTORDIR::END) ? currentInputDir : m_eDir;
			}
			
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::STRONG_ATTACK, &StrongAttack);
			return;
		}
	}

	// 가드 입력 체크
	/*if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
	{
		if (m_pFsm->Is_CoolTimeEnd(CPlayer::GUARD))
		{
			CPlayer_GuardState::GUARD_ENTER_DESC Guard{};
			Guard.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START"));
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::GUARD, &Guard);
			return;
		}
	}*/

	// LockOn 상태에 따라 다른 애니메이션 재생 (공격 체크 이후에 실행)
	if (m_pPlayer->Is_LockOn())
	{
		// LockOn 상태: 방향키에 따라 RUN_F/B/L/R
		ACTORDIR newDir = m_pPlayer->Calculate_Direction();
		
		if (newDir != ACTORDIR::END && newDir != m_eDir)
		{
			m_eDir = newDir;
			_uint newAnimIdx = 0;
			
			switch (m_eDir)
			{
			case ACTORDIR::U:   // W키: 전진
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
				break;
			case ACTORDIR::D:   // S키: 후진
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_B"));
				break;
			case ACTORDIR::L:   // A키: 좌측
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_L"));
				break;
			case ACTORDIR::R:   // D키: 우측
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_R"));
				break;
			case ACTORDIR::LU:  // W+A: 좌상향
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
				break;
			case ACTORDIR::LD:  // S+A: 좌하향  
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_B"));
				break;
			case ACTORDIR::RU:  // W+D: 우상향
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_F"));
				break;
			case ACTORDIR::RD:  // S+D: 우하향
				newAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN_B"));
				break;
			default:
				return;
			}
			
			// 애니메이션 변경
			if (newAnimIdx != m_iCurAnimIdx)
			{
				m_iCurAnimIdx = newAnimIdx;
				m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
			}
		}
	}
	else
	{
		// 일반 상태: RUN 상태로 전환
		CPlayer_RunState::RUN_ENTER_DESC Desc{};
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
		Desc.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &Desc);
	}
}

CPlayer_WalkState* CPlayer_WalkState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_WalkState* pInstance = new CPlayer_WalkState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player Idle State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_WalkState::Free()
{
	CPlayerState::Free();
}
