#pragma once
#include "BehaviorTree.h"

NS_BEGIN(Client)
class CQueenKnightTree final : public CBehaviorTree
{
public:
    typedef struct tagQueenKnightTreeDesc : CBehaviorTree::BT_DESC
    {
        class CQueenKnight* pOwner = { nullptr };
    } QUEEN_KNIGHT_BT_DESC;

private:
    explicit CQueenKnightTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CQueenKnightTree(const CMonsterTree& Prototype);
    virtual ~CQueenKnightTree() = default;

#pragma region 1. TREE 초기화
public:
    virtual HRESULT Initialize(void* pArg) override;


#pragma region 특수 상태 제어. Selector -> Buff Sequence
private:
    CBTSelector* Create_SpecialStates_ToSelector();

    CBTSequence* Create_SurvivalCheck_ToSequence();
    CBTSequence* Create_DownState_ToSequence();
    CBTSequence* Create_HitReaction_ToSequence();
#pragma endregion


#pragma region 기본 상태 제어 Selector -> Action Sequence
private:
    CBTSelector* Create_ActionStates_ToSelector();

private:
    /* 우선순위가 높은 공격 행동 체크 */
    // 특수 공격 => 쿨타임 돌았다면?
    CBTSequence* Create_SpecialAttack_ToSequence(); 

    
    CBTSequence* Create_FirstPhaseAttack_ToSequence(); // Phase1 기본 공격.
    CBTSequence* Create_SecondPhaseAttack_ToSequence();   // Phase2 기본 공격
    CBTSequence* Create_AttackAction_ToSequence();

private:
    CBTSequence* Create_SearchAction_ToSequence();

#pragma endregion



    /* 특수 상태 Health 체크, Down, Hit Reaction*/




    //CBTNode* Create_PatrolBranch();

private:
    /* 모든 상태가 실패하면? */
    CBTAction* Create_IdleAction();

public:
    virtual void Update(_float fTimeDelta);


public:
    void Set_Root_Node(CBTNode* pRootNode) {
        if (m_pRootNode) {
            Safe_Release(m_pRootNode);
        }
        m_pRootNode = pRootNode;
    }

    CBTNode* Get_Root_Node() const { return m_pRootNode; }

#pragma region 실제 Behaviour Tree에서 실행할 함수들
public:
    void Set_Target(class CPlayer* pPlayer);
#pragma endregion






#pragma endregion


private:
    //CBTNode* m_pRootNode = nullptr;
    class CQueenKnight* m_pOwner = nullptr;
    class CPlayer* m_pTarget = nullptr;

public:
    static CQueenKnightTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free() override;
};
NS_END

