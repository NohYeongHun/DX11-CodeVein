#include "MonsterTree.h"

CMonsterTree::CMonsterTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBehaviorTree(pDevice, pContext)
{
}

CMonsterTree::CMonsterTree(const CMonsterTree& Prototype)
    : CBehaviorTree(Prototype)
{
}

#pragma region 1. Behaviour 트리 초기화
HRESULT CMonsterTree::Initialize(void* pArg)
{
    // 기본 상태들 초기화.
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // Owner와 몬스터 타겟 설정. => 플레이어로
    MONSTER_BT_DESC* pDesc = static_cast<MONSTER_BT_DESC*>(pArg);
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

    // 3. 일반 행동 
    pRootSelector->Add_Child(Create_ActionStates_ToSelector());

    // 4. 모두 실패했을 경우.
    pRootSelector->Add_Child(Create_IdleAction());
    Set_Root_Node(pRootSelector);

    return S_OK;
}

#pragma region 특수한 버프/디버프 (죽음, 다운, HIT)를 컨트롤해야 함.
// 1. 특수 상태를 제어할 Selector 생성. => 1번 우선 순위로 실행됨.
CBTSelector* CMonsterTree::Create_SpecialStates_ToSelector()
{
    CBTSelector* pSpecialState_Selector = CBTSelector::Create();
    pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());
    pSpecialState_Selector->Add_Child(Create_DownState_ToSequence());
    pSpecialState_Selector->Add_Child(Create_HitReaction_ToSequence());

    return pSpecialState_Selector;
}

CBTSequence* CMonsterTree::Create_SurvivalCheck_ToSequence()
{
    CBTSequence* pDeadCheck_Sequence = CBTSequence::Create();

    // 1. Condition 체크 => 죽었는가?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

    // 2. Action => 죽었다면?
    pDeadCheck_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

    return pDeadCheck_Sequence;
}

CBTSequence* CMonsterTree::Create_DownState_ToSequence()
{
    CBTSequence* pDownCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Down 상황인가?
    pDownCheck_Sequence->Add_Child(CBT_Monster_IsDown::Create(m_pOwner));

    // 2. Action => 다운되었다면?
    pDownCheck_Sequence->Add_Child(CBT_Monster_DownAction::Create(m_pOwner));

    return pDownCheck_Sequence;
}

CBTSequence* CMonsterTree::Create_HitReaction_ToSequence()
{
    CBTSequence* pHitCheck_Sequence = CBTSequence::Create();
    // 1. Condition 체크 => Hit 상황인가?
    pHitCheck_Sequence->Add_Child(CBT_Monster_IsHit::Create(m_pOwner));

    // 2. Action => Hit 상태라면?
    pHitCheck_Sequence->Add_Child(CBT_Monster_HitAction::Create(m_pOwner));

    return pHitCheck_Sequence;
}
#pragma endregion



// 액션 상태를 제어할 Selector 생성.
CBTSelector* CMonsterTree::Create_ActionStates_ToSelector()
{
    CBTSelector* pActionState_Selector = CBTSelector::Create();

    // 액션 상태에서는 Sequence를 제어합니다.
    pActionState_Selector->Add_Child(Create_AttackAction_ToSequence());
    pActionState_Selector->Add_Child(Create_SearchAction_ToSequence());

    return pActionState_Selector;
}

/* Attack Action은 가지고 있는 Resource를 이용해야 확인 가능합니다. */
CBTSequence* CMonsterTree::Create_AttackAction_ToSequence()
{
    CBTSequence* pAttack_Sequence = CBTSequence::Create();
    pAttack_Sequence->Add_Child(CBT_Monster_IsAttackRange::Create(m_pOwner));
    pAttack_Sequence->Add_Child(CBT_Monster_AttackAction::Create(m_pOwner));

    return pAttack_Sequence;
}

CBTSequence* CMonsterTree::Create_SearchAction_ToSequence()
{
    CBTSequence* pSearch_Sequence = CBTSequence::Create();
    pSearch_Sequence->Add_Child(CBT_Monster_IsDetectRange::Create(m_pOwner));
    pSearch_Sequence->Add_Child(CBT_Monster_DetectAction::Create(m_pOwner,  false));

    return pSearch_Sequence;
}


CBTAction* CMonsterTree::Create_IdleAction()
{
    return nullptr;
}

#pragma endregion


void CMonsterTree::Update(_float fTimeDelta)
{
    if (m_pRootNode)
        m_pRootNode->Execute(fTimeDelta);
}

void CMonsterTree::Set_Target(CPlayer* pPlayer)
{
}



CMonsterTree* CMonsterTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CMonsterTree* pInstance = new CMonsterTree(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Create Failed CMonsterTree"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMonsterTree::Free()
{
    __super::Free();
}
