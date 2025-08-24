CPlayer_StrongAttackState::CPlayer_StrongAttackState()
{
}

HRESULT CPlayer_StrongAttackState::Initialize(_uint iStateNum, void* pArg)
{
	if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
	{
		CRASH("Failed Player StrongAttackState Initialize");
		return E_FAIL;
	}
		
	Add_Collider_Info(m_pPlayer->Find_AnimationIndex(TEXT("STRONG_ATTACK1"))
		, COLLIDER_ACTIVE_INFO{ 70.f / 230.f, 100.f / 230.f, false, 0, CPlayer::PART_WEAPON });


	return S_OK;
}

/* State 시작 시*/
void CPlayer_StrongAttackState::Enter(void* pArg)
{
	STRONG_ENTER_DESC* pDesc = static_cast<STRONG_ENTER_DESC*>(pArg);
	CPlayerState::Enter(pDesc); // 기본 쿨타임 설정.

	m_isLoop = false;

			
	m_iNextAnimIdx = -1;
	// 애니메이션 인덱스를 변경해줍니다.
	m_iCurAnimIdx = pDesc->iAnimation_Idx;
	m_eDir = pDesc->eDirection;
	m_pModelCom->Set_Animation(m_iCurAnimIdx, m_isLoop);

	m_pModelCom->Set_RootMotionRotation(true);
	m_pModelCom->Set_RootMotionTranslate(true);

	// 락온 중이면 타겟을 향해 즉시 회전
	if (m_pPlayer->Is_LockOn() && m_pPlayer->Has_LockOn_Target())
	{
		_vector vLockOnDirection = m_pPlayer->Calculate_LockOn_Direction();
		if (!XMVector3Equal(vLockOnDirection, XMVectorZero()))
		{
			// 락온 방향으로 즉시 회전
			_float x = XMVectorGetX(vLockOnDirection);
			_float z = XMVectorGetZ(vLockOnDirection);
			_float fTargetYaw = atan2f(x, z);
			
			_vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetYaw);
			m_pPlayer->Get_Transform()->Set_Quaternion(qNewRot);
		}
	}

	// 이 때 검에 콜라이더 활성화 이런 과정 진행
}

/* State 실행 */
void CPlayer_StrongAttackState::Update(_float fTimeDelta)
{
	Handle_Input();
	Handle_Unified_Direction_Input(fTimeDelta);
	Change_State();

	CPlayerState::Handle_Collider_State();
}

// 종료될 때 실행할 동작..
void CPlayer_StrongAttackState::Exit()
{
	// 무기 콜라이더 강제 비활성화
	Force_Disable_All_Colliders();
	
	if (m_iNextState != -1) // NextIndex가 있는경우 블렌딩 시작.
	{
		m_pModelCom->Set_BlendInfo(m_iNextAnimIdx, 0.2f, true, true, false);	
	}

	
}

// 상태 초기화
void CPlayer_StrongAttackState::Reset()
{
	m_eDir = { ACTORDIR::END };
	m_iCurAnimIdx = -1;
	m_iNextAnimIdx = -1;
	//m_pModelCom->Animation_Reset();
}

void CPlayer_StrongAttackState::Change_State()
{
	CPlayer_IdleState::IDLE_ENTER_DESC Idle{};
	CPlayer_RunState::RUN_ENTER_DESC Run{};
	CPlayer_AttackState::ATTACK_ENTER_DESC Attack{};
	CPlayer_StrongAttackState::STRONG_ENTER_DESC StrongAttack{};
	CPlayer_GuardState::GUARD_ENTER_DESC Guard{};

	
	if (m_pModelCom->Is_Finished())
	{
		m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("ATTACK1"));
		Attack.iAnimation_Idx = m_iNextAnimIdx;
		m_pFsm->Change_State(CPlayer::PLAYER_STATE::ATTACK, &Attack);
		return;
	}

	if (m_pFsm->Is_ExitCoolTimeEnd(m_iStateNum))
	{
		if (m_pPlayer->Is_MovementKeyPressed())
		{
			m_iNextAnimIdx = m_pPlayer->Find_AnimationIndex(TEXT("RUN"));
			m_iNextState = CPlayer::PLAYER_STATE::RUN;
			Run.iAnimation_Idx = m_iNextAnimIdx;
			m_pFsm->Change_State(m_iNextState, &Run);
			return;
		}
		
	}
}


CPlayer_StrongAttackState* CPlayer_StrongAttackState::Create(_uint iStateNum, void* pArg)
{
	CPlayer_StrongAttackState* pInstance = new CPlayer_StrongAttackState();
	if (FAILED(pInstance->Initialize(iStateNum, pArg)))
	{
		MSG_BOX(TEXT("Create Failed : Player CPlayer_StrongAttackState"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_StrongAttackState::Free()
{
	CPlayerState::Free();
}
