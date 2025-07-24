#include "Player_IdleState.h"

CPlayer_IdleState::CPlayer_IdleState()
{
}

HRESULT CPlayer_IdleState::Initialize(_uint iStateNum, void* pArg)
{
	PLAYER_IDLESTATE_DESC* pDesc = static_cast<PLAYER_IDLESTATE_DESC*>(pArg);
	if (FAILED(__super::Initialize(iStateNum, pDesc)))
		return E_FAIL;
	
	return S_OK;
}

/* State 시작 시*/
void CPlayer_IdleState::Enter(void* pArg)
{
	PLAYERIDLE_ENTER_DESC* pDesc = static_cast<PLAYERIDLE_ENTER_DESC*>(pArg);

	// 애니메이션 인덱스를 변경해줍니다.
	m_iAnimation_IdleIdx = pDesc->iAnimation_IdleIndex;
	m_pPlayer->Change_Animation(m_iAnimation_IdleIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();
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
	CPlayer_WalkState::PLAYERWALK_ENTER_DESC Desc{};
	// RUNDESC 추가 예정 

	if (m_pGameInstance->Get_KeyPress(DIK_W))
	{
		Desc.iAnimation_WalkIdx = 13;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &Desc);
	}
	if (m_pGameInstance->Get_KeyPress(DIK_S))
	{
		Desc.iAnimation_WalkIdx = 12;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &Desc);
	}
	if (m_pGameInstance->Get_KeyPress(DIK_A))
	{
		Desc.iAnimation_WalkIdx = 14;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &Desc);

	}
	if (m_pGameInstance->Get_KeyPress(DIK_D))
	{
		Desc.iAnimation_WalkIdx = 15;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::WALK, &Desc);
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
