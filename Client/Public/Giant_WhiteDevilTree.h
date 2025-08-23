#pragma once
#include "BehaviorTree.h"

NS_BEGIN(Client)
class CGiant_WhiteDevilTree final : public CBehaviorTree
{
public:
    typedef struct tagGiant_WhiteDevilTreeDesc : CBehaviorTree::BT_DESC
    {
        class CGiant_WhiteDevil* pOwner = { nullptr };
    }GIANT_WHITEDEVIL_BT_DESC;

private:
    explicit CGiant_WhiteDevilTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CGiant_WhiteDevilTree() = default;

#pragma region 1. TREE 초기화
public:
    virtual HRESULT Initialize(void* pArg) override;

#pragma region 조우 상태 제어.
    CBTSequence* Create_EncounterState_ToSequence();
#pragma endregion



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

#pragma region 특수 공격
private:
    /* 우선 순위 가 높음.*/
    CBTSequence* Create_ComboAttack_ToSequence(); // 연속공격
#pragma endregion


#pragma region 일반 공격


private:
    CBTSequence* Create_AttackAction_ToSequence();
#pragma endregion

private:
    CBTSequence* Create_Rotate_ToSequence();


private:
    CBTSequence* Create_SearchAction_ToSequence();

#pragma endregion

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

#pragma endregion


private:
    class CGiant_WhiteDevil* m_pOwner = nullptr;
    class CPlayer* m_pTarget = nullptr;

public:
    static CGiant_WhiteDevilTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free() override;
};
NS_END

