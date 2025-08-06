#include "BT_Monster_IdleAction.h"

CBT_Monster_IdleAction::CBT_Monster_IdleAction(CMonster* pOwner)
	: m_pOwner(pOwner)
{
}

BT_RESULT CBT_Monster_IdleAction::Perform_Action(_float fTimeDelta)
{
    return LoopIdle(fTimeDelta);
}

void CBT_Monster_IdleAction::Reset()
{
}

BT_RESULT CBT_Monster_IdleAction::LoopIdle(_float fTimeDelta)
{
	m_pOwner->Set_RootMotionTranslate(false);
    m_pOwner->RotateTurn_ToTarget();
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
	m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);

    return BT_RESULT::SUCCESS;
}

CBT_Monster_IdleAction* CBT_Monster_IdleAction::Create(CMonster* pOwner)
{
	if (nullptr == pOwner)
	{
		CRASH("Failed Create CBT_Monster_HitAction");
		return nullptr;
	}

	return new CBT_Monster_IdleAction(pOwner);
}

void CBT_Monster_IdleAction::Free()
{
	__super::Free();
	m_pOwner = nullptr;

}
