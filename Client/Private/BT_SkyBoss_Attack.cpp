#include "BT_SkyBoss_Attack.h"

CBT_SkyBoss_Attack::CBT_SkyBoss_Attack(CSkyBoss* pOwner)
	: m_pOwner { pOwner }
	, m_pGameInstance {CGameInstance::GetInstance()}
{

	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pGameInstance);
}

BT_RESULT CBT_SkyBoss_Attack::Perform_Action(_float fTimeDelta)
{

	return BT_RESULT();
}

CBT_SkyBoss_Attack* CBT_SkyBoss_Attack::Create(CSkyBoss* pOwner)
{
	if (nullptr == pOwner)
	{
		CRASH("Failed Create CBT_SkyBoss_Attack");
		return nullptr;
	}

	return new CBT_SkyBoss_Attack(pOwner);
}

void CBT_SkyBoss_Attack::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pOwner);
}
