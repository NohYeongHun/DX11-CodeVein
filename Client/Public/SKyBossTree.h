#pragma once
#include "BehaviourTree.h"

NS_BEGIN(Client)
class CSKyBossTree final : public CBehaviourTree
{
public:
    typedef struct tagSkyBossDesc : CBehaviourTree::BT_DESC
    {
        class CSkyBoss* pOwner = { nullptr };
    } SKYBOSS_BT_DESC;

private:
    CSKyBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSKyBossTree(const CSKyBossTree& Prototype);
    virtual ~CSKyBossTree() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta);

public:
    void Set_Root_Node(CBTNode* pRootNode) {
        if (m_pRootNode) {
            Safe_Release(m_pRootNode);
        }
        m_pRootNode = pRootNode;
        if (m_pRootNode) {
            Safe_AddRef(m_pRootNode);
        }
    }

    CBTNode* Get_Root_Node() const { return m_pRootNode; }

#pragma region 실제 Behaviour Tree에서 실행할 함수들
public:
    void Set_Player(class CPlayer* pPlayer);
#pragma endregion




private:
    CBTNode* m_pRootNode = nullptr;
    class CSkyBoss* m_pOwner = nullptr;
    class CPlayer* m_pPlayer = nullptr;

public:
    static CSKyBossTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free() override;

};
NS_END

