#include "SKyBossTree.h"

CSkyBossTree::CSkyBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehaviorTree(pDevice, pContext)
{
}

CSkyBossTree::CSkyBossTree(const CSkyBossTree& Prototype)
	: CBehaviorTree(Prototype)
{
}

/* 이 안에서 노드 만들기*/
HRESULT CSkyBossTree::Initialize(void* pArg)
{
	if (FAILED(CBehaviorTree::Initialize(pArg)))
		return E_FAIL;

	SKYBOSS_BT_DESC* pDesc = static_cast<SKYBOSS_BT_DESC*>(pArg);
	m_pOwner = pDesc->pOwner; // Owner 설정.

	//m_pPlayer = m_pOwner->Get_Target(); // 몬스터에 설정되어있는가?
	if (nullptr == m_pPlayer)
		CRASH("SkyBoss Tree Target Failed");

	// 1. RootNode 생성 -> Root를 Selector로 설정
	CBTSelector* pRootSelector = CBTSelector::Create();

	// 생존해 있으면 다음 노드로 움직여야하는데?
	
	// 2. 특수한 상태일때 행동 변경 => 필수.
	pRootSelector->Add_Child(Create_SpecialStates_ToSelector());

	// 5. 일반 행동 브랜치 (모든 상태가 false일 때만 실행)
	pRootSelector->Add_Child(Create_NormalBehaviorBranch());

	Set_Root_Node(pRootSelector);


	// 3. 피격 브랜치
	//pRootSelector->Add_Child(Create_HitBranch());
	//
	//// 4. 스턴 브랜치  
	//pRootSelector->Add_Child(Create_StunBranch());


	return S_OK;
}

void CSkyBossTree::Update(_float fTimeDelta)
{
	if (m_pRootNode)
		m_pRootNode->Execute(fTimeDelta);
}


void CSkyBossTree::Set_Player(CPlayer* pPlayer)
{
	if (nullptr == m_pPlayer)
		CRASH("Failed Set Player To SkyBossTree");

	m_pPlayer = pPlayer;
}
CBTSelector* CSkyBossTree::Create_SpecialStates_ToSelector()
{
	CBTSelector* pSpecialState_Selector = CBTSelector::Create();

	// 1. Dead Check Sequence 생성
	pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());


	// 2. Down Check Sequence 생성
	//pSpecialState_Selector->Add_Child(Create_SurvivalCheck_ToSequence());

	//CBTSequence* pDeadCheck_Sequence = CBTSequence::Create();
	//pDeadCheck_Sequence->Add_Child(Create_SurvivalCheck_ToSequence());

	//pSurvivalSequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

	/*pSpecialState_Selector->Add_Child(pDeadCheck_Sequence);*/
	return pSpecialState_Selector;
}

CBTNode* CSkyBossTree::Create_HitBranch()
{
	// 피격 처리 Sequence: 맞았는지 확인 → 피격 반응
	CBTSequence* pHitSequence = CBTSequence::Create();

	// 조건: 현재 피격 상태인가?
	pHitSequence->Add_Child(CBT_Monster_IsHit::Create(m_pOwner));

	// 피격 반응 선택 (피격 강도에 따라)
	CBTSelector* pHitActionSelector = CBTSelector::Create();

	// 강한 피격 처리
	/*CBTSequence* pStrongHitSeq = CBTSequence::Create();
	pStrongHitSeq->Add_Child(CBT_SkyBoss_IsStrongHit::Create(m_pOwner));
	pStrongHitSeq->Add_Child(CBT_SkyBoss_StrongHitReaction::Create(m_pOwner));
	pHitActionSelector->Add_Child(pStrongHitSeq)*/;

	// 일반 피격 처리 (항상 성공)
	pHitActionSelector->Add_Child(CBT_SkyBoss_NormalHitReaction::Create(m_pOwner));

	pHitSequence->Add_Child(pHitActionSelector);

	return pHitSequence;
}
CBTNode* CSkyBossTree::Create_NormalBehaviorBranch()
{
	// 일반 행동 Selector: 모든 특수 상태가 아닐 때 실행
	CBTSelector* pNormalSelector = CBTSelector::Create();

	// 1순위: 전투 행동 (플레이어가 범위 내에 있을 때)
	pNormalSelector->Add_Child(Create_ComBatBehavior());

	// 2순위: 순찰 행동
	//pNormalSelector->Add_Child(Create_PatrolBranch());

	// 3순위: 대기 행동 (항상 성공)
	pNormalSelector->Add_Child(Create_IdleBranch());

	return pNormalSelector;
}


CBTSequence* CSkyBossTree::Create_SurvivalCheck_ToSequence()
{
	CBTSequence* pDeadCheck_Sequence = CBTSequence::Create();

	// 1. Condition 체크 => 죽었는가?
	pDeadCheck_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

	// 2. Action => 죽었다면?
	pDeadCheck_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

	return pDeadCheck_Sequence;
}


CBTSequence* CSkyBossTree::Create_DownState_ToSequence()
{
	CBTSequence* pDownState_Sequence = CBTSequence::Create();

	// 1. Condition 체크 => 죽었는가?
	pDownState_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

	// 2. Action => 죽었다면?
	pDownState_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

	return pDownState_Sequence;
}

CBTSequence* CSkyBossTree::Create_HitReaction_ToSequence()
{
	CBTSequence* pHitReaction_Sequence = CBTSequence::Create();

	// 1. Condition 체크 => 죽었는가?
	pHitReaction_Sequence->Add_Child(CBT_Monster_IsDead::Create(m_pOwner));

	// 2. Action => 죽었다면?
	pHitReaction_Sequence->Add_Child(CBT_Monster_DeadAction::Create(m_pOwner));

	return pHitReaction_Sequence;
}


CBTNode* CSkyBossTree::Create_ComBatBehavior()
{

	// 0. 전투 시퀀스: 타겟이 범위 내에 있을 때만 전투 수행
	CBTSequence* pCombatSequence = CBTSequence::Create();

	// 1. 조건: 타겟이 감지 범위 내에 있는가?
	pCombatSequence->Add_Child(CBT_Monster_IsDetectRange::Create(m_pOwner));

	// 2. 전투 행동 선택 (Selector로 우선순위 설정)
	CBTSelector* pCombatActions = CBTSelector::Create();

	// 1순위: 강공격 (특정 조건에서)
		//CBTSequence* pStrongAttackSeq = CBTSequence::Create();
		//pStrongAttackSeq->Add_Child(CBT_SkyBoss_CanStrongAttack::Create(m_pOwner));
		//pStrongAttackSeq->Add_Child(CBT_SkyBoss_StrongAttack::Create(m_pOwner));

	// 2순위: 일반 공격 (공격 범위 내에서)
	CBTSequence* pNormalAttackSeq = CBTSequence::Create();
	//pNormalAttackSeq->Add_Child(CBT_IsTargetInRange::Create(m_pOwner, m_pOwner->Get_AttackRange()));
	pNormalAttackSeq->Add_Child(CBT_SkyBoss_Attack::Create(m_pOwner));

	//// 3순위: 플레이어 추격
	////pCombatActions->Add_Child(pStrongAttackSeq);
	pCombatActions->Add_Child(pNormalAttackSeq);
	//pCombatActions->Add_Child(CBT_SkyBoss_ChaseTarget::Create(m_pOwner));

	pCombatSequence->Add_Child(pCombatActions);

	return pCombatSequence;
}

//CBTNode* CSkyBossTree::Create_PatrolBranch()
//{
//	// 순찰 Sequence: 순찰 조건 확인 → 순찰 행동
//	CBTSequence* pPatrolSequence = CBTSequence::Create();
//
//	// 조건: 순찰해야 하는 상황인가?
//	pPatrolSequence->Add_Child(CBT_SkyBoss_ShouldPatrol::Create(m_pOwner));
//
//	// 순찰 행동 선택
//	CBTSelector* pPatrolActionSelector = CBTSelector::Create();
//
//	// 순찰 지점에 도달했는가? → 다음 지점 설정 및 대기
//	CBTSequence* pReachedSeq = CBTSequence::Create();
//	pReachedSeq->Add_Child(CBT_SkyBoss_ReachedPatrolPoint::Create(m_pOwner));
//	pReachedSeq->Add_Child(CBT_SkyBoss_SetNextPatrolPoint::Create(m_pOwner));
//	pReachedSeq->Add_Child(CBT_SkyBoss_PatrolWait::Create(m_pOwner, 2.0f)); // 2초 대기
//	pPatrolActionSelector->Add_Child(pReachedSeq);
//
//	// 순찰 지점으로 이동
//	pPatrolActionSelector->Add_Child(CBT_SkyBoss_MoveToPatrolPoint::Create(m_pOwner));
//
//	pPatrolSequence->Add_Child(pPatrolActionSelector);
//
//	return pPatrolSequence;
//}

CBTNode* CSkyBossTree::Create_IdleBranch()
{
	return CBT_SkyBoss_Idle::Create(m_pOwner);
}

#pragma region 고급 전투
CBTNode* CSkyBossTree::Create_MeleeAttackBranch()
{
	return nullptr;
}

CBTNode* CSkyBossTree::Create_RangedAttackBranch()
{
	return nullptr;
}
#pragma endregion



CSkyBossTree* CSkyBossTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{

	CSkyBossTree* pInstance = new CSkyBossTree(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg))) {
	    MSG_BOX(TEXT("Create Failed BehaviourTree"));
	    Safe_Release(pInstance);
	}

	return pInstance;
}


void CSkyBossTree::Free()
{
	CBehaviorTree::Free();



	m_pOwner = nullptr;
	m_pPlayer = nullptr;
}
