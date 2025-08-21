#pragma once
#include "BehaviorTree.h"

NS_BEGIN(Client)
class CMonsterTree final : public CBehaviorTree
{
public:
    typedef struct tagMonsterTreeDesc : CBehaviorTree::BT_DESC
    {
        class CMonster* pOwner = { nullptr };
    } MONSTER_BT_DESC;

private:
    explicit CMonsterTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMonsterTree(const CMonsterTree& Prototype);
    virtual ~CMonsterTree() = default;

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
    CBTSequence* Create_Rotate_ToSequence();

    /* 액션 행동 체크 */
    CBTSequence* Create_AttackAction_ToSequence();

private:
    
    CBTSequence* Create_SearchAction_ToSequence();

#pragma endregion



    /* 특수 상태 Health 체크, Down, Hit Reaction*/




    //CBTNode* Create_PatrolBranch();

private:
    CBTSequence* Create_IdleAction_ToSequence();
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
    class CMonster* m_pOwner = nullptr;
    class CPlayer*  m_pTarget = nullptr;

public:
    static CMonsterTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free() override;

};
NS_END

