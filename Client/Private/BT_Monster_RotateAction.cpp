CBT_Monster_RotateAction::CBT_Monster_RotateAction(CMonster* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"CBT_Monster_Rotation";
}

BT_RESULT CBT_Monster_RotateAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_DETECT))
        return BT_RESULT::FAILURE;

    switch (m_eRotPhase)
    {
    case ROTATION_PHASE::ENTER:
        return Enter_Rotation(fTimeDelta);
    case ROTATION_PHASE::ROTATE:
        return Update_Rotation(fTimeDelta);
    case ROTATION_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_RotateAction::Reset()
{
    m_eRotPhase = ROTATION_PHASE::ENTER;
    m_pOwner->Set_RootMotionTranslate(true);
}

/* WALK_B Loop를 진행하면서 */
BT_RESULT CBT_Monster_RotateAction::Enter_Rotation(_float fTimeDelta)
{
	m_pOwner->Set_RootMotionTranslate(true);
    m_pOwner->Change_Animation_Blend(m_pOwner->Find_AnimationIndex(TEXT("WALK_B")), false, 0.2f);
    m_eRotPhase = ROTATION_PHASE::ROTATE;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_RotateAction::Update_Rotation(_float fTimeDelta)
{
    //m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * 2.f);
    
    // 플레이어 후방 방향 구하기.
    _vector vTargetVector = m_pOwner->Get_TargetVector();
    m_pOwner->RootMotion_Translate(vTargetVector * fTimeDelta * 0.3f * -1.f);

    if (m_pOwner->Is_Animation_Finished())
    {
        m_eRotPhase = ROTATION_PHASE::COMPLETED;
    }
    else
    {
        m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * 2.f);
    }

    return BT_RESULT::RUNNING;
}

CBT_Monster_RotateAction* CBT_Monster_RotateAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_Rotation");
        return nullptr;
    }

    return new CBT_Monster_RotateAction(pOwner);
}

void CBT_Monster_RotateAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
