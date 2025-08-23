CBT_Monster_PrevEncounterAction::CBT_Monster_PrevEncounterAction(CMonster* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_Monster_PrevEncounterAction";
}

BT_RESULT CBT_Monster_PrevEncounterAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_ATTACK_TIME))
        return BT_RESULT::FAILURE;

    if (m_pOwner->Is_EncounterDistance())
        return BT_RESULT::SUCCESS;

    /* 지나가기. */
    if (m_IsAnimationFinished)
    {
        return BT_RESULT::SUCCESS;
    }
        

    switch (m_eEncounterPhase)
    {
    case ENCOUNTER_PHASE::ENTER:
        return Enter_Encounter(fTimeDelta);
    case ENCOUNTER_PHASE::LOOP:
        return Update_Encounter(fTimeDelta);
    case ENCOUNTER_PHASE::COMPLETED:
        return Complete(fTimeDelta);
    }

    return BT_RESULT::FAILURE;
}

void CBT_Monster_PrevEncounterAction::Reset()
{
    m_eEncounterPhase = ENCOUNTER_PHASE::ENTER;
}

BT_RESULT CBT_Monster_PrevEncounterAction::Enter_Encounter(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eEncounterPhase != ENCOUNTER_PHASE::ENTER)
    {
        CRASH("Failed Tree Encounter Enter Logic");
    }

    m_eEncounterPhase = ENCOUNTER_PHASE::LOOP;

    // 0. 회전.
    m_pOwner->RotateTurn_ToTargetYaw();

    // 1. 조우 애니메이션 선택
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"PREV_ENCOUNTER");

    // 2. 조우 상태로 변경
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false);

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_PrevEncounterAction::Update_Encounter(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eEncounterPhase = ENCOUNTER_PHASE::COMPLETED;
    }

	return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_PrevEncounterAction::Complete(_float fTimeDleta)
{
    m_IsAnimationFinished = true;
    return BT_RESULT::SUCCESS;
}

CBT_Monster_PrevEncounterAction* CBT_Monster_PrevEncounterAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_PrevEncounterAction");
        return nullptr;
    }

    return new CBT_Monster_PrevEncounterAction(pOwner);
}

void CBT_Monster_PrevEncounterAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
