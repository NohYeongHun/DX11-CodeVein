#include "Player_StrongAttackState.h"

CPlayer_AttackState::CPlayer_AttackState()
{
}

HRESULT CPlayer_AttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_fAttackTime = 0.5f;


	return S_OK;
}

/* State 시작 시*/
void CPlayer_AttackState::Enter(void* pArg)
{
	ATTACK_ENTER_DESC* pDesc = static_cast<ATTACK_ENTER_DESC*>(pArg);

	m_isLoop = false;
	m_fCurTime = 0.f;
	
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
	

	// 디버그 출력
	//_wstring debugMsg = L"[COMBO] Step " + to_wstring(m_iCurrentComboStep) +
	//	L", AnimIdx: " + to_wstring(m_iCurAnimIdx) + L"\n";
	//OutputDebugString(debugMsg.c_str());
}

/* State 실행 */
void CPlayer_AttackState::Update(_float fTimeDelta)
{
	m_fCurTime += fTimeDelta;
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
	m_fCurTime = 0.f;
}

void CPlayer_AttackState::Change_State(_float fTimeDelta)
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};

	// 애니메이션이 끝났을 때
	if (m_pModelCom->Is_Finished())
	{
		// Idle 상태로 전환
		m_iNextState = CPlayer::PLAYER_STATE::IDLE;
		m_iNextAnimIdx = 16;
		Idle.iAnimation_Idx = 16;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
	}
	else
	{
		if (m_fCurTime >= m_fAttackTime && m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
		{
			if (m_iCurAnimIdx == 47) // 마지막이면
			{
				// Idle 상태로 전환
				m_iNextState = CPlayer::PLAYER_STATE::IDLE;
				m_iNextAnimIdx = 16;
				Idle.iAnimation_Idx = 16;
				m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
				
			}
			else
			{
				
				CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
				m_iNextAnimIdx = m_iCurAnimIdx + 1;
				m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
				Attack.iAnimation_Idx = m_iNextAnimIdx;
				m_pFsm->Change_State(m_iNextState, &Attack);
			}
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
