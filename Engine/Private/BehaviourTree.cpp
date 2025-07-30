
CBehaviourTree::CBehaviourTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CBehaviourTree::CBehaviourTree(const CBehaviourTree& Prototype)
    : CComponent(Prototype)
{
}

HRESULT CBehaviourTree::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBehaviourTree::Initialize_Clone(void* pArg)
{
    BT_DESC* pDesc = static_cast<BT_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    return S_OK;
}

void CBehaviourTree::Update(_float fTimeDelta)
{
    if (m_pRootNode) 
    {
        m_pRootNode->Execute(fTimeDelta);
    }
}

CBehaviourTree* CBehaviourTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBehaviourTree* pInstance = new CBehaviourTree(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Create Failed BehaviourTree"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CBehaviourTree::Clone(void* pArg)
{
    CBehaviourTree* pInstance = new CBehaviourTree(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg))) {
        MSG_BOX(TEXT("Clone Failed BehaviourTree"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBehaviourTree::Free()
{
    __super::Free();
    Safe_Release(m_pRootNode);
    m_pOwner = nullptr;
}