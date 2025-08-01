#include "MonsterTree.h"

CMonsterTree::CMonsterTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBehaviorTree(pDevice, pContext)
{
}

CMonsterTree::CMonsterTree(const CMonsterTree& Prototype)
    : CBehaviorTree(Prototype)
{
}

HRESULT CMonsterTree::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MONSTER_BT_DESC* pDesc = static_cast<MONSTER_BT_DESC*>(pArg);
    m_pOwner = pDesc->pOwner; // Owner 설정.

    m_pTarget = m_pOwner->Get_Target(); // 몬스터에 설정되어있는가?

    if (nullptr == m_pTarget)
    {
        CRASH("CMonsterTree Target Failed");
        return E_FAIL;
    }

    // 1. Root Node 생성
    CBTSelector* pRootSelector = CBTSelector::Create();

    // 2. 특수한 상태일때 행동 변경 => 필수.
    pRootSelector->Add_Child(Create_SpecialStates_ToSelector());
    
    // 3. 일반 행동 
    pRootSelector->Add_Child(Create_ActionStates_ToSelector());

    Set_Root_Node(pRootSelector);

    return S_OK;
}

void CMonsterTree::Update(_float fTimeDelta)
{
}

void CMonsterTree::Set_Target(CPlayer* pPlayer)
{
}

// 특수한 상태를 제어할 Selector 생성.
CBTSelector* CMonsterTree::Create_SpecialStates_ToSelector()
{
    
    CBTSelector* pSpecialState_Selector = CBTSelector::Create();
    pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());

    return pSpecialState_Selector;
}

// 액션 상태를 제어할 Selector 생성.
CBTSelector* CMonsterTree::Create_ActionStates_ToSelector()
{
    CBTSelector* pActionState_Selector = CBTSelector::Create();
    
    // 액션 상태에서는 Sequence를 제어합니다.
    pActionState_Selector->Add_Child(Create_AttackAction_ToSequence());
    pActionState_Selector->Add_Child(Create_SearchAction_ToSequence());

    return pActionState_Selector;
}

CBTNode* CMonsterTree::Create_NormalBehaviorBranch()
{
    return nullptr;
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
    return nullptr;
}

CBTSequence* CMonsterTree::Create_HitReaction_ToSequence()
{
    return nullptr;
}

CBTSequence* CMonsterTree::Create_AttackAction_ToSequence()
{
    CBTSequence* pAttack_Sequence = CBTSequence::Create();
    
    return pAttack_Sequence;
}

CBTSequence* CMonsterTree::Create_SearchAction_ToSequence()
{
    CBTSequence* pSearch_Sequence = CBTSequence::Create();

    return pSearch_Sequence;
}


CBTNode* CMonsterTree::Create_IdleBranch()
{
    return nullptr;
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
