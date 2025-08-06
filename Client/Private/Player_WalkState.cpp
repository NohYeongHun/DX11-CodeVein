#include "Player_WalkState.h"


CPlayer_WalkState::CPlayer_WalkState()
{
}

HRESULT CPlayer_WalkState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;


	return S_OK;
}

/* State 시작 시*/
void CPlayer_WalkState::Enter(void* pArg)
{
	WALK_ENTER_DESC* pDesc = static_cast<WALK_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.

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

	Handle_Input(fTimeDelta);

	if (m_pPlayer->Is_MovementKeyPressed())
	{
		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 0.5f);
	}
	else
	{
		CPlayer_IdleState::IDLE_ENTER_DESC Desc{};
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
		Desc.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Desc);
		
	}		

	CPlayerState::Handle_Collider_State();
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
	m_pModelCom->Animation_Reset();
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_WalkState::Handle_Input(_float fTimeDelta)
{
	

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
	__super::Free();
}
