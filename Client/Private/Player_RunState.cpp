
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

	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurIdx = 6;
	//m_iCurIdx = pDesc->iAnimation_Idx;

	//_wstring wstrLoop = TEXT("Loop가 안됌");
	//if (!m_isLoop)
	//	OutputDebugString(wstrLoop.c_str());

	m_pPlayer->Change_Animation(m_iCurIdx, m_isLoop);

	_wstring wstrLoop = TEXT("Loop가 안됌\n");
	if (!m_isLoop)
		OutputDebugString(wstrLoop.c_str());
}

/* State 실행 */
void CPlayer_RunState::Update(_float fTimeDelta)
{
	Handle_Input(fTimeDelta);

	if (!m_isLoop)
		CRASH("m_is Loop Failed");

	if (m_isKeyInput)
	{
		m_pPlayer->Move_By_Camera_Direction_8Way(m_eDir, fTimeDelta, 0.5f);
	}
	else
	{
		CPlayer_IdleState::IDLE_ENTER_DESC Desc{};
		Desc.iAnimation_Index = 17;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Desc);
		// Loop를 종료합니다.
		//m_pModelCom->Set_Loop(false);

		// 애니메이션이 끝났다면?
		//if (m_pModelCom->Is_Finished())
		//{
		//	CPlayer_IdleState::IDLE_ENTER_DESC Desc{};
		//	Desc.iAnimation_Index = 17;
		//	m_pFsm->Change_State(CPlayer::PLAYER_STATE::IDLE, &Desc);
		//}
	}		
}

// 종료될 때 실행할 동작..
void CPlayer_RunState::Exit()
{
	
}

// 상태 초기화
void CPlayer_RunState::Reset()
{
	
}

// 상태 전환 구현 (키 입력 감지)
void CPlayer_RunState::Handle_Input(_float fTimeDelta)
{
	m_isKeyInput = false;

	const _bool bW = m_pGameInstance->Get_KeyPress(DIK_W);
	const _bool bS = m_pGameInstance->Get_KeyPress(DIK_S);
	const _bool bA = m_pGameInstance->Get_KeyPress(DIK_A);
	const _bool bD = m_pGameInstance->Get_KeyPress(DIK_D);

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
