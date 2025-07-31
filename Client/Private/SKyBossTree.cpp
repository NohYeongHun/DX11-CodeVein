#include "SKyBossTree.h"

CSKyBossTree::CSKyBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehaviourTree(pDevice, pContext)
{
}

CSKyBossTree::CSKyBossTree(const CSKyBossTree& Prototype)
	: CBehaviourTree(Prototype)
{
}

/* 이 안에서 노드 만들기*/
HRESULT CSKyBossTree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SKYBOSS_BT_DESC* pDesc = static_cast<SKYBOSS_BT_DESC*>(pArg);
	m_pOwner = pDesc->pOwner; // Owner 설정.

	m_pPlayer = m_pOwner->Get_Target(); // 몬스터에 설정되어있는가?
	if (nullptr == m_pPlayer)
		CRASH("SkyBoss Tree Target Failed");

	// 1. Node들 생성. Selector부터.
	m_pRootNode = CBT_SkyBoss_IsTargetInRange::Create(m_pOwner, m_pOwner->Get_DetectionRange());


	return S_OK;
}

void CSKyBossTree::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pRootNode)
		m_pRootNode->Execute(fTimeDelta);
}

void CSKyBossTree::Set_Player(CPlayer* pPlayer)
{
	if (nullptr == m_pPlayer)
		CRASH("Failed Set Player To SkyBossTree");

	m_pPlayer = pPlayer;
};

CSKyBossTree* CSKyBossTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{

	CSKyBossTree* pInstance = new CSKyBossTree(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg))) {
	    MSG_BOX(TEXT("Create Failed BehaviourTree"));
	    Safe_Release(pInstance);
	}

	return pInstance;
}


void CSKyBossTree::Free()
{
	__super::Free();
	m_pOwner = nullptr;
	m_pPlayer = nullptr;
}
