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

	m_iNextIdx = -1;
	// 애니메이션 인덱스를 변경해줍니다.
	m_iAnimation_IdleIdx = pDesc->iAnimation_Index;
	m_pPlayer->Change_Animation(m_iAnimation_IdleIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();

	if (m_isKeyInput)
	{
		Change_State();
	}
}

// 종료될 때 실행할 동작..
void CPlayer_IdleState::Exit()
{
	//if (m_iNextIdx > -1)
	//	m_pModelCom->Change_Animation_WithBlend(m_iNextIdx, 2.f);
}

// 상태 초기화
void CPlayer_IdleState::Reset()
{
	m_iNextIdx = -1;
	m_eDir = { DIR::END };
}

void CPlayer_IdleState::Change_State()
{
	CPlayer_WalkState::WALK_ENTER_DESC Walk{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};


	m_iNextIdx = 6;
	Run.iAnimation_Idx = 6;
	m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &Run);
	
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_IdleState::Handle_Input()
{
	m_isKeyInput = false;

	const _bool bW = m_pGameInstance->Get_KeyPress(DIK_W);
	const _bool bS = m_pGameInstance->Get_KeyPress(DIK_S);
	const _bool bA = m_pGameInstance->Get_KeyPress(DIK_A);
	const _bool bD = m_pGameInstance->Get_KeyPress(DIK_D);
	// Walk로 변경.

	if (bW || bS || bA || bD)
		m_isKeyInput = true;

	// 방향 결정
	if (bW && bA)      m_eDir = DIR::LU;
	else if (bW && bD) m_eDir = DIR::RU;
	else if (bS && bA) m_eDir = DIR::LD;
	else if (bS && bD) m_eDir = DIR::RD;
	else if (bW)       m_eDir = DIR::U;
	else if (bS)       m_eDir = DIR::D;
	else if (bA)       m_eDir = DIR::L;
	else if (bD)       m_eDir = DIR::R;

	


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
