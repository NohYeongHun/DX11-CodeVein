#pragma once
#include "BehaviourTree.h"

NS_BEGIN(Client)
class CSkyBossTree final : public CBehaviourTree
{
public:
    typedef struct tagSkyBossDesc : CBehaviourTree::BT_DESC
    {
        class CSkyBoss* pOwner = { nullptr };
    } SKYBOSS_BT_DESC;

private:
    explicit CSkyBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CSkyBossTree(const CSkyBossTree& Prototype);
    virtual ~CSkyBossTree() = default;

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
    void Set_Player(class CPlayer* pPlayer);
#pragma endregion


#pragma region Node 생성 함수들.

#pragma region DEPTH2 Node
    /* Initialize에서 생성 */
private:
    CBTSelector* Create_SpecialStates_ToSelector(); 
    CBTNode* Create_HitBranch();
    //CBTNode* Create_StunBranch();
    CBTNode* Create_NormalBehaviorBranch();
#pragma endregion


#pragma region DEPTH3 Node

    /* 각 상위 노드들에서 생성할 때 생성.. */
private:
    CBTSequence* Create_SurvivalCheck_ToSequence();
    CBTSequence* Create_DownState_ToSequence();
    CBTSequence* Create_HitReaction_ToSequence();


private:
    // Noraml Behaviour
    CBTNode* Create_ComBatBehavior();
    //CBTNode* Create_PatrolBranch();
    CBTNode* Create_IdleBranch();
#pragma endregion



#pragma endregion

#pragma region 고급 전투
    CBTNode* Create_MeleeAttackBranch();
    CBTNode* Create_RangedAttackBranch();


#pragma endregion


private:
    //CBTNode* m_pRootNode = nullptr;
    class CSkyBoss* m_pOwner = nullptr;
    class CPlayer* m_pPlayer = nullptr;

public:
    static CSkyBossTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free() override;

};
NS_END

