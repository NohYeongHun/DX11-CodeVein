
CPlayer_DodgeState::CPlayer_DodgeState()
{
}

HRESULT CPlayer_DodgeState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_isLoop = false;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_DodgeState::Enter(void* pArg)
{

	_vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION); // 구문 수정
	OutputDebugString((L"[DODGE ENTER] Position: " +
		std::to_wstring(XMVectorGetX(vPos)) + L", " +
		std::to_wstring(XMVectorGetZ(vPos)) + L"\n").c_str());

	DODGE_ENTER_DESC* pDesc = static_cast<DODGE_ENTER_DESC*>(pArg);
	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;

	// ⭐ Dodge는 non-loop으로 변경
	m_isLoop = false;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
}

/* State 실행 */
void CPlayer_DodgeState::Update(_float fTimeDelta)
{
	if (m_pModelCom->Is_Finished())
	{
		// ✅ 구문 수정
		_vector vPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);
		OutputDebugString((L"[DODGE FINISHED] Position: " +
			std::to_wstring(XMVectorGetX(vPos)) + L", " +
			std::to_wstring(XMVectorGetZ(vPos)) + L"\n").c_str());
		Change_State();
	}
		
}

/*
* 이 문제는 애니메이션 제작 시점의 좌표 기준과 런타임 블렌딩 시점의 좌표 기준이 다르기 때문에 
발생하는 근본적인 좌표계 불일치 문제입니다. 
구르기 같은 이동 애니메이션과 Idle 같은 제자리 애니메이션 간의 
공간적 연속성을 보장하는 추가적인 처리가 필요한 상황입니다.

=> 로컬좌표가 다른 루트 모션간에 블렌딩하는 방법?
*/
// 종료될 때 실행할 동작.=> 왠만하면 Idle
void CPlayer_DodgeState::Exit()
{
	// 여기서 동작해야합니다.
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		if (m_iNextState == CPlayer::PLAYER_STATE::IDLE)
			m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
	}
		
}

// 상태 초기화
void CPlayer_DodgeState::Reset()
{
	m_eDir = { DIR::END };
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = -1;

}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_DodgeState::Change_State()
{

	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	m_iNextState = CPlayer::PLAYER_STATE::IDLE;
	m_iNextAnimIdx = 17;
	Idle.iAnimation_Index = 17;
	m_pFsm->Change_State(m_iNextState, &Idle);
	//m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.5f, true, true, false);
}


CPlayer_DodgeState* CPlayer_DodgeState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_DodgeState* pInstance = new CPlayer_DodgeState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player DODGE State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_DodgeState::Free()
{
	__super::Free();
}