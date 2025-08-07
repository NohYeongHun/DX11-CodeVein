CPlayer_GuardState::CPlayer_GuardState()
{
}

HRESULT CPlayer_GuardState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(__super::Initialize(iStateNum, pArg)))
		return E_FAIL;

	m_isLoop = false;

	return S_OK;
}

/* State 시작 시*/
void CPlayer_GuardState::Enter(void* pArg)
{

	GUARD_ENTER_DESC* pDesc = static_cast<GUARD_ENTER_DESC*>(pArg);
	__super::Enter(pDesc); // 기본 쿨타임 설정.
	m_iNextState = -1;
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = pDesc->iAnimation_Idx;

	// ⭐ Dodge는 non-loop으로 변경
	m_isLoop = false;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);
	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(false);

	// 방향 제어 관련 초기화
	m_bCanChangeDirection = true;
	m_fCurrentLockTime = 0.0f;
	m_bIsDirectionLocked = false;
}

/* State 실행 */
void CPlayer_GuardState::Update(_float fTimeDelta)
{
	Handle_Input();
	// 상황에 맞게 Direction을 지정. if LockOn
	Handle_Unified_Direction_Input(fTimeDelta);
	Change_State(fTimeDelta);

	CPlayerState::Handle_Collider_State();

}

/*
* 이 문제는 애니메이션 제작 시점의 좌표 기준과 런타임 블렌딩 시점의 좌표 기준이 다르기 때문에 
발생하는 근본적인 좌표계 불일치 문제입니다. 
구르기 같은 이동 애니메이션과 Idle 같은 제자리 애니메이션 간의 
공간적 연속성을 보장하는 추가적인 처리가 필요한 상황입니다.

=> 로컬좌표가 다른 루트 모션간에 블렌딩하는 방법?
*/
// 종료될 때 실행할 동작.=> 왠만하면 Idle
void CPlayer_GuardState::Exit()
{
	/*_wstring wstr = L" Guard Current Anim Index : " + to_wstring(m_iNextAnimIdx) + L" ";
	OutputDebugString(wstr.c_str());*/
	// 보간 안해도 됨.

	//if (m_iNextState == CPlayer::PLAYER_STATE::IDLE)
	//	m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);
		
	//if (m_iNextState == CPlayer::PLAYER_STATE::GUARD)
	//	m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.05f, true, true, false);

	m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, true);
}

// 상태 초기화
void CPlayer_GuardState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iNextAnimIdx = -1;
	m_iCurAnimIdx = -1;
	m_pModelCom->Animation_Reset();

}

/* 상태에 따른 변경을 정의합니다. */
void CPlayer_GuardState::Change_State(_float fTimeDelta)
{

	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};

	// Guard 상태에서 다시 누르면?

	// Exit CoolTime => 상태 변환이 가능한 시간일 때 상태를 변경해준다면?
	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_KeyPressed(PLAYER_KEY::GUARD))
		{
			// 현재 상태가 시작 상태일때만 End로 변경.
			if (m_iCurAnimIdx == m_pPlayer->Find_AnimationIndex(TEXT("GUARD_START")))
			{
				m_iNextState = CPlayer::PLAYER_STATE::GUARD;
				m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("GUARD_END"));
				Guard.iAnimation_Idx = m_iNextAnimIdx;
				m_pFsm->Change_State(m_iNextState, &Guard);
				return;
			}
		}
	}
	// 모델 재생시간이 끝났는데 End 상태였다면 Idle로 변환
	if (m_pModelCom->Is_Finished())
	{
		if (m_iCurAnimIdx == m_pPlayer->Find_AnimationIndex(TEXT("GUARD_END")))
		{
			m_iNextState = CPlayer::PLAYER_STATE::IDLE;
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));
			Idle.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Idle);
			return;
		}
	}

}


CPlayer_GuardState* CPlayer_GuardState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_GuardState* pInstance = new CPlayer_GuardState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player DODGE State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_GuardState::Free()
{
	__super::Free();
}