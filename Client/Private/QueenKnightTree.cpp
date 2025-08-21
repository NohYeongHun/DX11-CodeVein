#include "QueenKnightTree.h"
CQueenKnightTree::CQueenKnightTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBehaviorTree(pDevice, pContext)
{
}

CQueenKnightTree::CQueenKnightTree(const CMonsterTree& Prototype)
    : CBehaviorTree(Prototype)
{
}

HRESULT CQueenKnightTree::Initialize(void* pArg)
{
    // 기본 상태들 초기화.
    if (FAILED(CBehaviorTree::Initialize(pArg)))
        return E_FAIL;

    // Owner와 몬스터 타겟 설정. => 플레이어로
    QUEEN_KNIGHT_BT_DESC* pDesc = static_cast<QUEEN_KNIGHT_BT_DESC*>(pArg);
    m_pOwner = pDesc->pOwner; // Owner 설정.
    m_pTarget = m_pOwner->Get_Target(); // 몬스터에 설정되어있는가?

    if (nullptr == m_pTarget)
    {
        CRASH("CMonsterTree Target Failed");
        return E_FAIL;
    }

    // 1. Root Node 생성 무조건 Selector
    CBTSelector* pRootSelector = CBTSelector::Create();

    // 2. 특수한 상태일때 행동 변경 => 필수.
    pRootSelector->Add_Child(Create_SpecialStates_ToSelector());

    // 3. 행동 Selector
    pRootSelector->Add_Child(Create_ActionStates_ToSelector());
    
    // 4. 모두 실패했을 경우.
    pRootSelector->Add_Child(Create_IdleAction());
    Set_Root_Node(pRootSelector);

    return S_OK;
}

CBTSelector* CQueenKnightTree::Create_SpecialStates_ToSelector()
{
    CBTSelector* pSpecialState_Selector = CBTSelector::Create();
    pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());
    pSpecialState_Selector->Add_Child(Create_DownState_ToSequence());
    pSpecialState_Selector->Add_Child(Create_HitReaction_ToSequence());

    return pSpecialState_Selector;
}

CBTSequence* CQueenKnightTree::Create_SurvivalCheck_ToSequence()
{
    CBTSequence* pDeadCheck_Sequence = CBTSequence::Create();

    // 1. Condition 체크 => 죽었는가?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

    // 2. Action => 죽었다면?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

    return pDeadCheck_Sequence;
}

CBTSequence* CQueenKnightTree::Create_DownState_ToSequence()
{
    CBTSequence* pDownCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Down 상황인가?
    pDownCheck_Sequence->Add_Child(CBT_Monster_IsDown::Create(m_pOwner));

    // 2. Action => 다운되었다면?
    pDownCheck_Sequence->Add_Child(CBT_Monster_DownAction::Create(m_pOwner));

    return pDownCheck_Sequence;
}

CBTSequence* CQueenKnightTree::Create_HitReaction_ToSequence()
{
    CBTSequence* pHitCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Hit 상황인가?
    pHitCheck_Sequence->Add_Child(CBT_Monster_IsHit::Create(m_pOwner));

    // 2. Action => Hit 상태라면?
    pHitCheck_Sequence->Add_Child(CBT_Monster_HitAction::Create(m_pOwner));

    return pHitCheck_Sequence;
}

CBTSelector* CQueenKnightTree::Create_ActionStates_ToSelector()
{
    CBTSelector* pActionState_Selector = CBTSelector::Create();

    // 액션 상태에서는 Sequence를 제어합니다.
    // 우선 순위가 높은 공격 상태부터 먼저 검증.
    pActionState_Selector->Add_Child(Create_TripleDownAttack_ToSequence());
    pActionState_Selector->Add_Child(Create_DashAttack_ToSequence());
    
    pActionState_Selector->Add_Child(Create_FirstPhaseAttack_ToSequence());
    pActionState_Selector->Add_Child(Create_AttackAction_ToSequence());
    pActionState_Selector->Add_Child(Create_SearchAction_ToSequence());
    pActionState_Selector->Add_Child(Create_Rotate_ToSequence());

    return pActionState_Selector;
}




CBTSequence* CQueenKnightTree::Create_TripleDownAttack_ToSequence()
{
    CBTSequence* pTrippleDownAttack_Sequence = CBTSequence::Create();
    pTrippleDownAttack_Sequence->Add_Child(CBT_QueenKnight_IsTripleDownStrikeCondition::Create(m_pOwner));
    pTrippleDownAttack_Sequence->Add_Child(CBT_QueenKnight_DownStrikeAction::Create(m_pOwner));
    pTrippleDownAttack_Sequence->Add_Child(CBT_QueenKnight_DownStrikeAction::Create(m_pOwner));
    pTrippleDownAttack_Sequence->Add_Child(CBT_QueenKnight_TripleDownStrikeAction::Create(m_pOwner));
    return pTrippleDownAttack_Sequence;
}

CBTSequence* CQueenKnightTree::Create_DashAttack_ToSequence()
{
    CBTSequence* pDashAttack_Sequence = CBTSequence::Create();
    pDashAttack_Sequence->Add_Child(CBT_QueenKnight_IsDashAttackCondition::Create(m_pOwner));
    pDashAttack_Sequence->Add_Child(CBT_QueenKnight_DashAttackAction::Create(m_pOwner));

    return pDashAttack_Sequence;
}

CBTSequence* CQueenKnightTree::Create_FirstPhaseAttack_ToSequence()
{
    CBTSequence* pAttack_Sequence = CBTSequence::Create();
    pAttack_Sequence->Add_Child(CBT_Monster_IsAttackRange::Create(m_pOwner));
    pAttack_Sequence->Add_Child(CBT_QueenKnight_FirstPhase_AttackAction::Create(m_pOwner));

    return pAttack_Sequence;
}

CBTSequence* CQueenKnightTree::Create_SecondPhaseAttack_ToSequence()
{
    return nullptr;
}




CBTSequence* CQueenKnightTree::Create_AttackAction_ToSequence()
{
    CBTSequence* pAttack_Sequence = CBTSequence::Create();
    pAttack_Sequence->Add_Child(CBT_Monster_IsAttackRange::Create(m_pOwner));
    pAttack_Sequence->Add_Child(CBT_Monster_AttackAction::Create(m_pOwner));

    return pAttack_Sequence;
}

CBTSequence* CQueenKnightTree::Create_Rotate_ToSequence()
{
    CBTSequence* pRotate_Sequence = CBTSequence::Create();
    pRotate_Sequence->Add_Child(CBT_Monster_IsRotate::Create(m_pOwner));
    pRotate_Sequence->Add_Child(CBT_Monster_RotateAction::Create(m_pOwner));
    return pRotate_Sequence;
}

CBTSequence* CQueenKnightTree::Create_SearchAction_ToSequence()
{
    // DetectAction을 독립 Action으로 사용 (조건을 내부에서 처리)
    CBTSequence* pSearch_Sequence = CBTSequence::Create();
    pSearch_Sequence->Add_Child(CBT_Monster_IsDetectRange::Create(m_pOwner));
    pSearch_Sequence->Add_Child(CBT_Monster_DetectAction::Create(m_pOwner));
    return pSearch_Sequence;
}

CBTAction* CQueenKnightTree::Create_IdleAction()
{
    return CBT_Monster_IdleAction::Create(m_pOwner);
}

void CQueenKnightTree::Update(_float fTimeDelta)
{
    if (m_pRootNode)
    {
        m_pRootNode->Execute(fTimeDelta);
    }
        
}

void CQueenKnightTree::Set_Target(CPlayer* pPlayer)
{
    m_pTarget = pPlayer;
}

CQueenKnightTree* CQueenKnightTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CQueenKnightTree* pInstance = new CQueenKnightTree(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Create Failed CMonsterTree"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CQueenKnightTree::Free()
{
    CBehaviorTree::Free();
}
