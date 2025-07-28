#include "Player_StrongAttackState.h"

CPlayer_AttackState::CPlayer_AttackState()
{
}

HRESULT CPlayer_AttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_AttackState::Enter(void* pArg)
{
	ATTACK_ENTER_DESC* pDesc = static_cast<ATTACK_ENTER_DESC*>(pArg);

	m_isLoop = false;

	m_fAttackTime = 0.5f; // 60프레임
	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

	// 이 때 검에 콜라이더 활성화 이런 과정 진행
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
	//if (m_iNextIdx > -1) // NextIndex가 있는경우 블렌딩 시작.
	//	m_pModelCom->Change_Animation_WithBlend(m_iNextIdx, 0.5f);
	// 여기서 동작해야합니다.
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::ATTACK)
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, false, true, false);
			m_pModelCom->Set_AnimationTrackPosition(m_iNextAnimIdx, m_pModelCom->Get_CurrentTrackPosition());
		}
		else if (m_iNextState == CPlayer::PLAYER_STATE::IDLE)
		{
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
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


	if (m_pModelCom->Is_Finished())
	{
		m_iNextAnimIdx = 16;
		Idle.iAnimation_Idx = 16;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Idle);
	}
	else if (m_iCurAnimIdx != 28)
	{
		if (m_fAttackTime <= m_fCurTime)
		{
			if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::ATTACK))
			{
				_wstring wstr = L"Current Time : " + to_wstring(m_fCurTime) + L'\n';
				OutputDebugString(wstr.c_str());
				m_iNextState = CPlayer::PLAYER_STATE::ATTACK;
				m_iNextAnimIdx = m_iCurAnimIdx + 1;
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
