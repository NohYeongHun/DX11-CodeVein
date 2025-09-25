#include "BT_Monster_HitAction.h"
CBT_Monster_HitAction::CBT_Monster_HitAction(CMonster* pOwner)
	: m_pOwner { pOwner }
{
    m_strTag = L"MonsterIdle_HitActionNode";
}

BT_RESULT CBT_Monster_HitAction::Perform_Action(_float fTimeDelta)
{
    switch (m_eHitPhase)
    {
    case HIT_PHASE::NONE:
        return EnterHit(fTimeDelta);
    case HIT_PHASE::LOOP:
        return UpdateHit(fTimeDelta);
    case HIT_PHASE::COMPLETED:
        return EndHit(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_HitAction::Reset()
{
    m_eHitPhase = HIT_PHASE::NONE;
}

BT_RESULT CBT_Monster_HitAction::EnterHit(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eHitPhase != HIT_PHASE::NONE)
    {
        CRASH("Failed Tree Hit Enter Logic");
    }

    m_pOwner->Hit_Action();

    // 0. 즉시 회전 시키기
    m_pOwner->RotateTurn_ToTarget();

    // 1. 맞는 애니메이션 선택
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"HIT");

    // 2. 맞는 상태로 변경
    m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);

    // 3. 맞았으면 무적시간 부여하기. => 충돌 시 부여.
    // m_pOwner->AddBuff(CMonster::BUFF_INVINCIBLE);

    m_pOwner->PlayHitSound();

    // 4. 다음 단계로 진행
    m_eHitPhase = HIT_PHASE::LOOP;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_HitAction::UpdateHit(_float fTimeDelta)
{
    _bool bIsAnimationEnd = m_pOwner->Is_Animation_Finished();

    if (bIsAnimationEnd)
        m_eHitPhase = HIT_PHASE::COMPLETED;

	return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_HitAction::EndHit(_float fTimeDleta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        // 1. Animation 탐색 시작
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. 현재 애니메이션으로 블렌딩하면서 변경.
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

        // 3. 현재 상태에 대한 판단 기준 =>  쿨타임은 유지되게 해야합니다..
        m_pOwner->RemoveBuff(CMonster::BUFF_HIT);

        m_pOwner->Hit_EndAction();
    }

    return BT_RESULT::SUCCESS;
}

CBT_Monster_HitAction* CBT_Monster_HitAction::Create(CMonster* pOwner)
{
	if (nullptr == pOwner)
	{
		CRASH("Failed Create CBT_Monster_HitAction");
		return nullptr;
	}

	return new CBT_Monster_HitAction(pOwner);
}

void CBT_Monster_HitAction::Free()
{
	CBTAction::Free();
	m_pOwner = nullptr;
}
