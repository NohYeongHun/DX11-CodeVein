
#pragma once

// ============ BehaviourTree_Component.h ============
NS_BEGIN(Engine)

class ENGINE_DLL CBehaviourTree final : public CComponent
{
public:
    typedef struct tagBTDesc
    {
        class CGameObject* pOwner = nullptr;
    } BT_DESC;

private:
    CBehaviourTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBehaviourTree(const CBehaviourTree& Prototype);
    virtual ~CBehaviourTree() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
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

private:
    CBTNode* m_pRootNode = nullptr;
    class CGameObject* m_pOwner = nullptr;

public:
    static CBehaviourTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END