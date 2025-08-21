#include "BT_Monster_IdleAction.h"
CBT_Monster_IdleAction::CBT_Monster_IdleAction(CMonster* pOwner)
	: m_pOwner(pOwner)
{
	m_strTag = L"MonsterIdle_ActionNode";
}

BT_RESULT CBT_Monster_IdleAction::Perform_Action(_float fTimeDelta)
{
	if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD | CMonster::BUFF_DETECT))
		return BT_RESULT::FAILURE;

	switch (m_eIdlePhase)
	{
	case IDLE_PHASE::ENTER:
		return EnterIdle(fTimeDelta);
	/*case IDLE_PHASE::LOOP:
		return LoopIdle(fTimeDelta);*/
	case IDLE_PHASE::COMPLETED:
		return BT_RESULT::SUCCESS;
	}


	return BT_RESULT::FAILURE;
    
}

void CBT_Monster_IdleAction::Reset()
{
	m_eIdlePhase = IDLE_PHASE::ENTER;
}

BT_RESULT CBT_Monster_IdleAction::EnterIdle(_float fTimeDelta)
{
	m_pOwner->Set_RootMotionTranslate(false);
	if (m_pOwner->Is_Animation_Finished())
	{ 
		_uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");
		m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
		m_eIdlePhase = IDLE_PHASE::COMPLETED;
	}
	

	return BT_RESULT::RUNNING;
}

//BT_RESULT CBT_Monster_IdleAction::LoopIdle(_float fTimeDelta)
//{
//	m_pOwner->RotateTurn_ToTargetYaw(fTimeDelta * 4.f);
//	if (m_pOwner->IsRotateFinished(XMConvertToRadians(4.f)))
//	{
//		m_eIdlePhase = IDLE_PHASE::COMPLETED;
//	}
//
//
//
//    return BT_RESULT::RUNNING;
//}

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
	CBTAction::Free();
	m_pOwner = nullptr;

}
