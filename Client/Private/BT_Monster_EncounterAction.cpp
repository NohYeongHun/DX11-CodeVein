CBT_Monster_EncounterAction::CBT_Monster_EncounterAction(CMonster* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_Monster_EncounterAction";
}

BT_RESULT CBT_Monster_EncounterAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    /* 조우 거리 이내에 들어오지 않았다면? */
    if (!m_pOwner->Is_EncounterDistance())
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

void CBT_Monster_EncounterAction::Reset()
{
	m_eEncounterPhase = ENCOUNTER_PHASE::ENTER;
}

BT_RESULT CBT_Monster_EncounterAction::Enter_Encounter(_float fTimeDelta)
{

    // 안전 코드 추가
    if (m_eEncounterPhase != ENCOUNTER_PHASE::ENTER)
    {
        CRASH("Failed Tree Encounter Enter Logic");
    }

    m_eEncounterPhase = ENCOUNTER_PHASE::LOOP;

    m_pOwner->Set_BGM(true);

    // 0. 회전.
    m_pOwner->RotateTurn_ToTargetYaw();

    // 1. 조우 애니메이션 선택
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"ENCOUNTER");

    // 2. 조우 상태로 변경
    m_pOwner->Change_Animation_Blend(iNextAnimationIdx);
    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_EncounterAction::Update_Encounter(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eEncounterPhase = ENCOUNTER_PHASE::COMPLETED;
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx, false);
        m_pOwner->Set_Encountered(true);

        
        CQueenKnight* pQueenKnight = dynamic_cast<CQueenKnight*>(m_pOwner);
        if (nullptr != pQueenKnight)
            pQueenKnight->IncreaseDetection();

    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_EncounterAction::Complete(_float fTimeDleta)
{
    /* Encounter 시 실행할 Action */
    m_pOwner->Encounter_Action();
    
    return BT_RESULT::SUCCESS;
}

CBT_Monster_EncounterAction* CBT_Monster_EncounterAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_EncounterAction");
        return nullptr;
    }

    return new CBT_Monster_EncounterAction(pOwner);
}

void CBT_Monster_EncounterAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
