#include "Player_WalkState.h"

CPlayer_WalkState::CPlayer_WalkState()
{
}

HRESULT CPlayer_WalkState::Initialize(_uint iStateNum, void* pArg)
{
	PLAYER_WALKSTATE_DESC* pDesc = static_cast<PLAYER_WALKSTATE_DESC*>(pArg);
	if (FAILED(__super::Initialize(iStateNum, pDesc)))
		return E_FAIL;


	return S_OK;
}

/* State 시작 시*/
void CPlayer_WalkState::Enter(void* pArg)
{
	PLAYERWALK_ENTER_DESC* pDesc = static_cast<PLAYERWALK_ENTER_DESC*>(pArg);

	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurIdx = pDesc->iAnimation_WalkIdx;
	m_pPlayer->Change_Animation(m_iCurIdx, m_isLoop);
	
}

/* State 실행 */
void CPlayer_WalkState::Update(_float fTimeDelta)
{
	Handle_Input(fTimeDelta);

	// 키인풋이 없다면? => 애니메이션 종료 예정.
	if (!m_isKeyInput) 
	{
		// Loop를 종료합니다.
		m_pModelCom->Set_Loop(false);

		// 애니메이션이 끝났다면?
		if (m_pModelCom->Is_Finished())
		{
			CPlayer_IdleState::PLAYERIDLE_ENTER_DESC Desc{};
			Desc.iAnimation_IdleIndex = 17;
			m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Desc);
		}
	}
	else
	{
		// 키인풋을 하고 있지만 현재 키 값이 변경되었다면? => 애니메이션을 변경해주어야합니다.
		if (m_iCurIdx != m_iPrevIdx)
		{
			m_pModelCom->Set_Animation(m_iCurIdx, m_isLoop);
		}
	}
		
}

// 종료될 때 실행할 동작..
void CPlayer_WalkState::Exit()
{
	
}

// 상태 초기화
void CPlayer_WalkState::Reset()
{
	m_isKeyInput = true;
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_WalkState::Handle_Input(_float fTimeDelta)
{
	m_isKeyInput = false;

	// 이동 로직도 추가.
	if (m_pGameInstance->Get_KeyPress(DIK_W))
	{
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			
		}
		else
		{
			m_iPrevIdx = m_iCurIdx;
			m_iCurIdx = 13;

			if (m_pGameInstance->Get_KeyPress(DIK_A))
			{
				m_pPlayer->Get_Transform()->Go_Left(fTimeDelta * 0.1f);
			}
			else if (m_pGameInstance->Get_KeyPress(DIK_D))
			{
				m_pPlayer->Get_Transform()->Go_Right(fTimeDelta * 0.1f);
			}

			m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 0.1f);
		}

		m_isKeyInput = true;
	}
	
	if (m_pGameInstance->Get_KeyPress(DIK_S))
	{
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			//m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, nullptr);
		}
		else
		{
			m_iPrevIdx = m_iCurIdx;
			m_iCurIdx = 12;

			if (m_pGameInstance->Get_KeyPress(DIK_A))
			{
				m_pPlayer->Get_Transform()->Go_Left(fTimeDelta * 0.1f);
			}
			else if (m_pGameInstance->Get_KeyPress(DIK_D))
			{
				m_pPlayer->Get_Transform()->Go_Right(fTimeDelta * 0.1f);
			}

			m_pPlayer->Get_Transform()->Go_Backward(fTimeDelta * 0.1f);
		}
		m_isKeyInput = true;
	}
	
	if (m_pGameInstance->Get_KeyPress(DIK_A))
	{
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			//m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, nullptr);
		}
		else
		{
			m_iPrevIdx = m_iCurIdx;
			m_iCurIdx = 14;
			m_pPlayer->Get_Transform()->Go_Left(fTimeDelta * 0.1f);
		}

		m_isKeyInput = true;

	}
	
	if (m_pGameInstance->Get_KeyPress(DIK_D))
	{
		if (m_pGameInstance->Get_KeyPress(DIK_LSHIFT))
		{
			//m_pFsm->Change_State(CPlayer::PLAYER_STATE::RUN, nullptr);
		}
		else
		{
			m_iPrevIdx = m_iCurIdx;
			m_iCurIdx = 15;
			m_pPlayer->Get_Transform()->Go_Right(fTimeDelta * 0.1f);
			
		}
		m_isKeyInput = true;
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
	__super::Free();
}
