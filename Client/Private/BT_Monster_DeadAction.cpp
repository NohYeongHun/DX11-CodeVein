
/* 죽으면 소멸해야 하기 때문에 따로 상태 Reset이 필요해보이진 않음 .*/
CBT_Monster_DeadAction::CBT_Monster_DeadAction(CMonster* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"MonsterIdle_IsDeadActionNode";
}

BT_RESULT CBT_Monster_DeadAction::Perform_Action(_float fTimeDelta)
{
    switch (m_eDeadPhase)
    {
    case DEAD_PHASE::NONE:
        return EnterDead();
    case DEAD_PHASE::DYING:
        return UpdateDying(fTimeDelta);
    case DEAD_PHASE::CORPSE:
        return UpdateCorpse(fTimeDelta);
    case DEAD_PHASE::DISSOLVING:
        return UpdateDissolve(fTimeDelta);
    case DEAD_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_DeadAction::Reset()
{

}

BT_RESULT CBT_Monster_DeadAction::EnterDead()
{
    // 안전 코드 추가
    if (m_eDeadPhase != DEAD_PHASE::NONE)
    {
        CRASH("Failed Tree Dead Start Logic");
    }
        

    // 1. 죽는 애니메이션 선택
    // 탐색
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DEATH_NORMAL");

    // 2. 죽는 상태로 변경
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

    // 3. 콜리전 비활성화 (즉시) => 죽었는데 맞으면 안되겠지요
    // m_pOwner->Set_CollisionEnabled(false);

    // 4. 죽음 이벤트 처리 (점수, 아이템 드롭 등)
    //HandleDeathEvents();

    // 5. 다음 단계로 진행
    m_eDeadPhase = DEAD_PHASE::DYING;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateDying(_float fTimeDelta)
{
    _bool bIsAnimationEnd = m_pOwner->Is_Animation_Finished();
    _bool bIsBuffPossible = m_pOwner->AddBuff(CMonster::BUFF_CORPSE);

    if (bIsAnimationEnd && bIsBuffPossible)
    {
        m_eDeadPhase = DEAD_PHASE::CORPSE; // 죽었을때 시체 상태로 변경.
    }
        

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateCorpse(_float fTimeDelta)
{
    // 시체 상태가 일정시간 유지되어야 합니다. => 이후 삭제. Tick_Timers에서 진행 중
    if (!m_pOwner->HasBuff(CMonster::BUFF_CORPSE))
        m_eDeadPhase = DEAD_PHASE::DISSOLVING; // 현재 디졸브 효과가 없으므로 삭제 진행.
        
       

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateDissolve(_float fTimeDelta)
{
    // 디졸브 효과가 없으므로 삭제로 바꿔봅시다.
    m_eDeadPhase = DEAD_PHASE::COMPLETED;
    m_pOwner->Set_Destroy(true);
    // 디졸브 효과 업데이트
    //_float dissolveProgress = m_fDissolveTimer / 2.0f; // 2초간 디졸브
    ////m_pOwner->Update_Dissolve_Effect(dissolveProgress);

    //// 디졸브 완료
    //if (dissolveProgress >= 1.0f)
    //{
    //    //m_pOwner->Set_Dead(); // 완전히 제거
    //    m_eDeadPhase = DEAD_PHASE::COMPLETED;
    //    return BT_RESULT::SUCCESS;
    //}

    return BT_RESULT::RUNNING;
}



CBT_Monster_DeadAction* CBT_Monster_DeadAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_DeadAction");
        return nullptr;
    }

    return new CBT_Monster_DeadAction(pOwner);
}

void CBT_Monster_DeadAction::Free()
{
    CBTAction::Free();
}
