#include "BehaviourTree.h"


CBehaviourTree::CBehaviourTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice {pDevice}
    , m_pContext { pContext }
    , m_pGameInstance {CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CBehaviourTree::CBehaviourTree(const CBehaviourTree& Prototype)
{
}



HRESULT CBehaviourTree::Initialize(void* pArg)
{
    BT_DESC* pDesc = static_cast<BT_DESC*>(pArg);

    return S_OK;
}

void CBehaviourTree::Update(_float fTimeDelta)
{
   
}

void CBehaviourTree::Free()
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}


