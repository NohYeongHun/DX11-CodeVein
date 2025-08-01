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

public:
    virtual HRESULT Initialize(void* pArg) override;
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


#pragma region Node 생성 함수들.

#pragma region DEPTH2 Node
    /* Initialize에서 생성 */
private:
    CBTSelector* Create_SpecialStates_ToSelector();
    CBTSelector* Create_ActionStates_ToSelector();

    //CBTNode* Create_StunBranch();
    CBTNode* Create_NormalBehaviorBranch();
#pragma endregion


#pragma region DEPTH3 Node
/* 특수 상태 Health 체크, Down, Hit Reaction*/
private:
    CBTSequence* Create_SurvivalCheck_ToSequence();
    CBTSequence* Create_DownState_ToSequence();
    CBTSequence* Create_HitReaction_ToSequence();


private:
    /* 액션 행동 체크 */
    CBTSequence* Create_AttackAction_ToSequence();
    CBTSequence* Create_SearchAction_ToSequence();
    //CBTNode* Create_PatrolBranch();

private:
    /* 모든 상태가 실패하면? */
    CBTNode* Create_IdleBranch();
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

