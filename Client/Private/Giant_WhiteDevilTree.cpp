#include "Giant_WhiteDevilTree.h"
CGiant_WhiteDevilTree::CGiant_WhiteDevilTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehaviorTree(pDevice, pContext)
{
}

HRESULT CGiant_WhiteDevilTree::Initialize(void* pArg)
{
    // 기본 상태들 초기화.
    if (FAILED(CBehaviorTree::Initialize(pArg)))
    {
        CRASH("Failed BehaviorTree Initialize in CGiant_WhiteDevilTree.");
        return E_FAIL;
    }

    GIANT_WHITEDEVIL_BT_DESC* pDesc = static_cast<GIANT_WHITEDEVIL_BT_DESC*>(pArg);
    m_pOwner = pDesc->pOwner; // Owner 설정.
    m_pTarget = m_pOwner->Get_Target(); // 몬스터에 설정되어있는가?

    if (nullptr == m_pTarget)
    {
        CRASH("CMonsterTree Target Failed");
        return E_FAIL;
    }

    // 1. Root Node 생성 무조건 Selector
    CBTSelector* pRootSelector = CBTSelector::Create();

    // 2. 조우 상태 체크.
    pRootSelector->Add_Child(Create_EncounterState_ToSequence());

    // 3. 특수한 상태일때 행동 변경 => 필수.
    pRootSelector->Add_Child(Create_SpecialStates_ToSelector());

    // 4. 행동 Selector
    pRootSelector->Add_Child(Create_ActionStates_ToSelector());

    // 5. 모두 실패했을 경우.
    pRootSelector->Add_Child(Create_IdleAction());
    Set_Root_Node(pRootSelector);

    return S_OK;
}

CBTSequence* CGiant_WhiteDevilTree::Create_EncounterState_ToSequence()
{
    CBTSequence* pEncounterState_Sequence = CBTSequence::Create();
    pEncounterState_Sequence->Add_Child(CBT_Monster_IsEncounterCondition::Create(m_pOwner));
    pEncounterState_Sequence->Add_Child(CBT_Monster_PrevEncounterAction::Create(m_pOwner));
    pEncounterState_Sequence->Add_Child(CBT_Monster_EncounterAction::Create(m_pOwner));

    return pEncounterState_Sequence;
}

CBTSelector* CGiant_WhiteDevilTree::Create_SpecialStates_ToSelector()
{
    CBTSelector* pSpecialState_Selector = CBTSelector::Create();
    pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());
    //pSpecialState_Selector->Add_Child(Create_DownState_ToSequence());
    pSpecialState_Selector->Add_Child(Create_HitReaction_ToSequence());

    return pSpecialState_Selector;
}

CBTSequence* CGiant_WhiteDevilTree::Create_SurvivalCheck_ToSequence()
{
    CBTSequence* pDeadCheck_Sequence = CBTSequence::Create();

    // 1. Condition 체크 => 죽었는가?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

    // 2. Action => 죽었다면?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

    return pDeadCheck_Sequence;
}

CBTSequence* CGiant_WhiteDevilTree::Create_DownState_ToSequence()
{
    CBTSequence* pDownCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Down 상황인가?
    pDownCheck_Sequence->Add_Child(CBT_Monster_IsDown::Create(m_pOwner));

    // 2. Action => 다운되었다면?
    pDownCheck_Sequence->Add_Child(CBT_Monster_DownAction::Create(m_pOwner));

    return pDownCheck_Sequence;
}

CBTSequence* CGiant_WhiteDevilTree::Create_HitReaction_ToSequence()
{
    CBTSequence* pHitCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Hit 상황인가?
    pHitCheck_Sequence->Add_Child(CBT_Monster_IsHit::Create(m_pOwner));

    // 2. Action => Hit 상태라면?
    pHitCheck_Sequence->Add_Child(CBT_Monster_HitAction::Create(m_pOwner));

    return pHitCheck_Sequence;
}

CBTSelector* CGiant_WhiteDevilTree::Create_ActionStates_ToSelector()
{
    CBTSelector* pActionState_Selector = CBTSelector::Create();

    pActionState_Selector->Add_Child(Create_ComboAttack_ToSequence());
    pActionState_Selector->Add_Child(Create_AttackAction_ToSequence());
    
    pActionState_Selector->Add_Child(Create_SearchAction_ToSequence());
    pActionState_Selector->Add_Child(Create_Rotate_ToSequence());

    return pActionState_Selector;
}

CBTSequence* CGiant_WhiteDevilTree::Create_ComboAttack_ToSequence()
{
    CBTSequence* pComboAttack_Sequence = CBTSequence::Create();
    pComboAttack_Sequence->Add_Child(CBT_Monster_IsAttackRange::Create(m_pOwner));
    pComboAttack_Sequence->Add_Child(CBT_GiantWhiteDevil_ComboAttackAction::Create(m_pOwner));

    return pComboAttack_Sequence;
}

CBTSequence* CGiant_WhiteDevilTree::Create_AttackAction_ToSequence()
{
    CBTSequence* pAttack_Sequence = CBTSequence::Create();
    pAttack_Sequence->Add_Child(CBT_Monster_IsAttackRange::Create(m_pOwner));
    pAttack_Sequence->Add_Child(CBT_Monster_AttackAction::Create(m_pOwner));

    return pAttack_Sequence;
}

CBTSequence* CGiant_WhiteDevilTree::Create_Rotate_ToSequence()
{
    CBTSequence* pRotate_Sequence = CBTSequence::Create();
    pRotate_Sequence->Add_Child(CBT_Monster_IsRotate::Create(m_pOwner));
    pRotate_Sequence->Add_Child(CBT_Monster_RotateAction::Create(m_pOwner));
    return pRotate_Sequence;
}

CBTSequence* CGiant_WhiteDevilTree::Create_SearchAction_ToSequence()
{
    // DetectAction을 독립 Action으로 사용 (조건을 내부에서 처리)
    CBTSequence* pSearch_Sequence = CBTSequence::Create();
    pSearch_Sequence->Add_Child(CBT_Monster_IsDetectRange::Create(m_pOwner));
    pSearch_Sequence->Add_Child(CBT_Monster_DetectAction::Create(m_pOwner));
    return pSearch_Sequence;
}

CBTAction* CGiant_WhiteDevilTree::Create_IdleAction()
{
    return CBT_Monster_IdleAction::Create(m_pOwner);
}

void CGiant_WhiteDevilTree::Update(_float fTimeDelta)
{
    if (m_pRootNode)
    {
        m_pRootNode->Execute(fTimeDelta);
    }
}

CGiant_WhiteDevilTree* CGiant_WhiteDevilTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CGiant_WhiteDevilTree* pInstance = new CGiant_WhiteDevilTree(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Create Failed CGiant_WhiteDevilTree"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGiant_WhiteDevilTree::Free()
{
    CBehaviorTree::Free();
}
