#include "Player_IdleState.h"

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
	IDLE_ENTER_DESC* pDesc = static_cast<IDLE_ENTER_DESC*>(pArg);

	// 애니메이션 인덱스를 변경해줍니다.
	m_iAnimation_IdleIdx = pDesc->iAnimation_Index;
	m_pPlayer->Change_Animation(m_iAnimation_IdleIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();
	if (!m_isLoop)
		CRASH("m_is Loop Failed");
}

// 종료될 때 실행할 동작..
void CPlayer_IdleState::Exit()
{

}

// 상태 초기화
void CPlayer_IdleState::Reset()
{

}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_IdleState::Handle_Input()
{
	CPlayer_RunState::RUN_ENTER_DESC RunDesc{};
	CPlayer_WalkState::WALK_ENTER_DESC WalkDesc{};
	// RUNDESC 추가 예정 

	if (m_pGameInstance->Get_KeyPress(DIK_W))
	{
		// SHIFT 같이 누르면 걷기.
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			WalkDesc.iAnimation_Idx = 13;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &WalkDesc);
		}
		else
		{
			RunDesc.iAnimation_Idx = 6;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &RunDesc);
		}
	}
	else if (m_pGameInstance->Get_KeyPress(DIK_S))
	{
		// SHIFT 같이 누르면 걷기.
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			WalkDesc.iAnimation_Idx = 12;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &WalkDesc);
		}
		else
		{
			RunDesc.iAnimation_Idx = 4;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &RunDesc);
		}
	}
	else if (m_pGameInstance->Get_KeyPress(DIK_A))
	{
		// SHIFT 같이 누르면 걷기.
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			WalkDesc.iAnimation_Idx = 14;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &WalkDesc);
		}
		else
		{
			RunDesc.iAnimation_Idx = 8;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &RunDesc);
		}
		
	}
	else if (m_pGameInstance->Get_KeyPress(DIK_D))
	{
		// SHIFT 같이 누르면 걷기.
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			WalkDesc.iAnimation_Idx = 15;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &WalkDesc);
		}
		else 
		{
			RunDesc.iAnimation_Idx = 10;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &RunDesc);
		}
		
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
