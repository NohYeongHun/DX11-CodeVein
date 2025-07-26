#include "Player_IdleState.h"


static _uint iDebugCnt = 0;
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
	if (iDebugCnt == 1)
	{
		static _float currentYaw = m_pPlayer->Get_Transform()->GetYawFromQuaternion();
		OutputDebugString((L"[CPlayer_IdleState2] Player Yaw: " + std::to_wstring(XMConvertToDegrees(currentYaw)) + L"°\n").c_str());
	}

	IDLE_ENTER_DESC* pDesc = static_cast<IDLE_ENTER_DESC*>(pArg);
	
	//m_pModelCom->Set_RootMotionEnabled(false);
	m_iNextIdx = -1;
	// 애니메이션 인덱스를 변경해줍니다.
	m_iAnimation_IdleIdx = pDesc->iAnimation_Index;
	m_pPlayer->Change_Animation(m_iAnimation_IdleIdx, m_isLoop);

	//m_pModelCom->Set_RootMotionEnabled(false);

}

/* State 실행 */
void CPlayer_IdleState::Update(_float fTimeDelta)
{
	Handle_Input();
}

// 종료될 때 실행할 동작..
void CPlayer_IdleState::Exit()
{
	// 한번만 
	
	iDebugCnt++;
	//static _float currentYaw = m_pPlayer->Get_Transform()->GetYawFromQuaternion();
	//OutputDebugString((L"[CPlayer_IdleState] Player Yaw: " + std::to_wstring(XMConvertToDegrees(currentYaw)) + L"°\n").c_str());

	//if (m_iNextIdx > -1)
	//	m_pModelCom->Change_Animation_WithBlend(m_iNextIdx, 0.2f);
	//m_pModelCom->Set_RootMotionEnabled(true);
}

// 상태 초기화
void CPlayer_IdleState::Reset()
{
	m_iNextIdx = -1;
	m_eDir = { DIR::END };
}

void CPlayer_IdleState::Change_State(const INPUT_INFO& eInputInfo)
{
	CPlayer_WalkState::WALK_ENTER_DESC Walk{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};

	INPUT_INFO realTimeInput = Get_MoveMentInfo();

	/*if (eInputInfo.bMouseL)
	{
		m_iNextIdx = 38;
		Run.iAnimation_Idx = 38;
		Run.eDirection = eInputInfo.eDirection;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::SWORD_STRONG_ATTACK, &StrongAttack);
	}*/
	if (eInputInfo.bA || eInputInfo.bD || eInputInfo.bW || eInputInfo.bS)
	{
		m_iNextIdx = 6;
		Run.iAnimation_Idx = 6;
		Run.eDirection = eInputInfo.eDirection;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, &Run);
	}
	
	
	
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_IdleState::Handle_Input()
{
	m_isKeyInput = false;

	auto input = Get_MoveMentInfo();
	if (input.bAnyMovementKey)
		m_isKeyInput = true;

	m_eDir = input.eDirection;

	Change_State(input);
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
