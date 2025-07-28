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

	m_isLoop = false;

			
	m_iNextAnimIdx = -1;
	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

	// 이 때 검에 콜라이더 활성화 이런 과정 진행
}

/* State 실행 */
void CPlayer_StrongAttackState::Update(_float fTimeDelta)
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};

	if (m_pModelCom->Is_Finished())
	{
		m_iNextAnimIdx = 16;
		Idle.iAnimation_Idx = 16;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
	}
}

// 종료될 때 실행할 동작..
void CPlayer_StrongAttackState::Exit()
{
	//if (m_iNextIdx > -1) // NextIndex가 있는경우 블렌딩 시작.
	//	m_pModelCom->Change_Animation_WithBlend(m_iNextIdx, 0.5f);
	// 여기서 동작해야합니다.
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::IDLE)
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
