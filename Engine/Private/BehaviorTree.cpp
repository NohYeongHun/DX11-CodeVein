#include "BehaviorTree.h"


CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice {pDevice}
    , m_pContext { pContext }
    , m_pGameInstance {CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& Prototype)
{
}



HRESULT CBehaviorTree::Initialize(void* pArg)
{
    BT_DESC* pDesc = static_cast<BT_DESC*>(pArg);

    return S_OK;
}

void CBehaviorTree::Update(_float fTimeDelta)
{
   
}

void CBehaviorTree::Free()
{
    CBase::Free();
    m_pRootNode->Destroy_Recursive(); // 모든 자식 노드까지 해제
    Safe_Release(m_pRootNode);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}


