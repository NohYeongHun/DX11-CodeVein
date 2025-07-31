#include "BT_SkyBoss_Idle.h"

CBT_SkyBoss_Idle::CBT_SkyBoss_Idle(CSkyBoss* pOwner)
	: m_pOwner{ pOwner }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{

	Safe_AddRef(m_pGameInstance);
}


BT_RESULT CBT_SkyBoss_Idle::Perform_Action(_float fTimeDelta)
{
	m_pOwner->Chanage_Animation(SKYBOSS_ANIM_AXE_IDLE, true);

	


    return BT_RESULT::SUCCESS;
}

CBT_SkyBoss_Idle* CBT_SkyBoss_Idle::Create(CSkyBoss* pOwner)
{
	if (nullptr == pOwner)
	{
		CRASH("Failed Create CBT_SkyBoss_Attack");
		return nullptr;
	}

	return new CBT_SkyBoss_Idle(pOwner);
}


void CBT_SkyBoss_Idle::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
