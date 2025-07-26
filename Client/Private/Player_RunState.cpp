#include "Player_RunState.h"

CPlayer_RunState::CPlayer_RunState()
{
}

HRESULT CPlayer_RunState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;
	return S_OK;
}

/* State 시작 시*/
void CPlayer_RunState::Enter(void* pArg)
{
	RUN_ENTER_DESC* pDesc = static_cast<RUN_ENTER_DESC*>(pArg);

	m_iNextIdx = -1;
	m_iCurIdx = 6;
	m_bFirstFrame = true;

	// ✅ 루트 모션 일시 비활성화 (애니메이션 전환 시 잘못된 회전 방지)
	// m_pModelCom->Set_RootMotionEnabled(false);

	// ✅ 애니메이션 변경
	m_pPlayer->Change_Animation(m_iCurIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_RunState::Update(_float fTimeDelta)
{
	Handle_Input(fTimeDelta);

	if (m_isKeyInput)
	{
		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 1.f);
	}
	else
		Change_State();
}

// 종료될 때 실행할 동작..
void CPlayer_RunState::Exit()
{
	if (m_iNextIdx > -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		m_pModelCom->Change_Animation_WithBlend(m_iNextIdx, 0.5f);
	}

}

// 상태 초기화
void CPlayer_RunState::Reset()
{
	m_eDir = { DIR::END };
	m_iCurIdx = -1;
	m_iNextIdx = -1;
	m_bFirstFrame = false; // ✅ 리셋 시 첫 프레임 플래그도 초기화
}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_RunState::Change_State()
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};

	Idle.iAnimation_Index = 17;
	m_iNextIdx = CPlayer::PLAYER_STATE::IDLE;
	m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_RunState::Handle_Input(_float fTimeDelta)
{
	m_isKeyInput = false;

	auto input = Get_MoveMentInfo();
	if (input.bAnyMovementKey)
		m_isKeyInput = true;

	m_eDir = input.eDirection;
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
	__super::Free();
}