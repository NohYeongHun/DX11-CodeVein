
CPlayer_DamageState::CPlayer_DamageState()
{
}

HRESULT CPlayer_DamageState::Initialize(_uint iStateNum, void* pArg)
{
    if (FAILED(CPlayerState::Initialize(iStateNum, pArg)))
        return E_FAIL;

    return S_OK;
}

void CPlayer_DamageState::Enter(void* pArg)
{
    CPlayerState::Enter(pArg);
    
    DAMAGE_ENTER_DESC* pEnterDesc = static_cast<DAMAGE_ENTER_DESC*>(pArg);
    
    if (nullptr != pEnterDesc)
    {
        m_eDamageDirection = pEnterDesc->eDamageDirection;
        
        // 피격 방향에 따른 애니메이션 선택
        _uint iAnimationIndex = Get_Damage_Animation_By_Direction(m_eDamageDirection);
        
        // 애니메이션 변경 (루프 없음, 한 번만 재생)
        m_pModelCom->Set_Animation(iAnimationIndex, false);
    }
    else
    {
        // 기본값: 앞쪽 피격 애니메이션
        m_eDamageDirection = ACTORDIR::U;
        m_pModelCom->Set_Animation(m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_F")), false);
    }

    // 상태 초기화
    m_IsAnimationFinished = false;
    m_fCurrentInvincibilityTime = 0.f;
    
    // 무적 버프 추가
    m_pPlayer->AddBuff(CPlayer::BUFF_HIT);

    _wstring strSoundTag = TEXT("Player_Hit_0");
    _uint iRandValue = m_pGameInstance->Rand_UnsignedInt(1, 4);
    strSoundTag += to_wstring(iRandValue) + TEXT(".wav");
    m_pGameInstance->PlaySoundEffect(strSoundTag, 0.5f);
}

void CPlayer_DamageState::Update(_float fTimeDelta)
{
    // 무적 시간 업데이트
    m_fCurrentInvincibilityTime += fTimeDelta;
    
    // 애니메이션이 끝났는지 확인
    if (m_pModelCom->Is_Finished())
    {
        m_IsAnimationFinished = true;
        Change_State();
    }
}

void CPlayer_DamageState::Exit()
{
    // 무적 버프 제거 (무적 시간이 충분히 지났다면)
    if (m_fCurrentInvincibilityTime >= m_fInvincibilityTime)
    {
        m_pPlayer->RemoveBuff(CPlayer::BUFF_HIT);
    }
}

void CPlayer_DamageState::Reset()
{
    m_eDamageDirection = ACTORDIR::END;
    m_IsAnimationFinished = false;
    m_fCurrentInvincibilityTime = 0.f;
}

void CPlayer_DamageState::Change_State()
{
    if (m_IsAnimationFinished)
    {
        // 피격 애니메이션이 끝나면 IDLE 상태로 전환
        CPlayer_IdleState::IDLE_ENTER_DESC IdleDesc{};
        IdleDesc.iAnimation_Idx = m_pPlayer->Find_AnimationIndex(TEXT("IDLE"));  // 기본 대기 애니메이션
        m_pFsm->Change_State(CPlayer::IDLE, &IdleDesc);
    }
}

_uint CPlayer_DamageState::Get_Damage_Animation_By_Direction(ACTORDIR eDirection)
{
    switch (eDirection)
    {
    case ACTORDIR::D:   // 뒤쪽 피격
        return m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_B"));
    case ACTORDIR::U:   // 앞쪽 피격
        return m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_F"));
    case ACTORDIR::L:   // 왼쪽 피격
        return m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_L"));
    case ACTORDIR::R:   // 오른쪽 피격
        return m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_R"));
    default:
        return m_pPlayer->Find_AnimationIndex(TEXT("DAMAGE_F"));  // 기본값: 앞쪽 피격
    }
}

CPlayer_DamageState* CPlayer_DamageState::Create(_uint iStateNum, void* pArg)
{
    CPlayer_DamageState* pInstance = new CPlayer_DamageState();

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Create : CPlayer_DamageState"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_DamageState::Free()
{
    CPlayerState::Free();
}