#include "BT_Monster_DownAction.h"

CBT_Monster_DownAction::CBT_Monster_DownAction(CMonster* pOwner)
    : m_pOwner{ pOwner }
{
}

BT_RESULT CBT_Monster_DownAction::Perform_Action(_float fTimeDelta)
{
    switch (m_eDownPhase)
    {
    case DOWN_PHASE::NONE:
        return EnterDown();
    case DOWN_PHASE::START:
        return StartDown(fTimeDelta);
    case DOWN_PHASE::LOOP:
        return LoopDown(fTimeDelta);
    case DOWN_PHASE::END:
        return EndDown(fTimeDelta);
    case DOWN_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_DownAction::Reset()
{
    m_eDownPhase = DOWN_PHASE::NONE;
}

BT_RESULT CBT_Monster_DownAction::EnterDown()
{
    // NONE이 아닌데 호출되면 문제가 생긴것.
    if (m_eDownPhase != DOWN_PHASE::NONE)
    {
        CRASH("Failed Tree Down Start Logic");
        return BT_RESULT::RUNNING;
    }

    // 1. Animation 탐색 시작.
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_START");

    

    // 2. 현재 애니메이션으로 블렌딩하면서 변경.
    m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
    m_eDownPhase = DOWN_PHASE::START;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DownAction::StartDown(_float fTimeDelta)
{
    // 현재 Start Animation이 끝나면?
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eDownPhase = DOWN_PHASE::LOOP;
        // 1. Animation 탐색 시작.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_LOOP");

        // 2. 현재 애니메이션으로 블렌딩하면서 변경.
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
    }


    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DownAction::LoopDown(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eDownPhase = DOWN_PHASE::END;
        // 1. Animation 탐색 시작.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"DOWN_END");

        // 2. 현재 애니메이션으로 블렌딩하면서 변경.
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);
    }

    return BT_RESULT::RUNNING;
}



BT_RESULT CBT_Monster_DownAction::EndDown(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        m_eDownPhase = DOWN_PHASE::COMPLETED;
        // 1. Animation 탐색 시작
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. 현재 애니메이션으로 블렌딩하면서 변경.
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

        // 3. 현재 상태에 대한 판단 기준 =>  쿨타임은 유지되게 해야합니다..
        m_pOwner->RemoveBuff(CMonster::BUFF_DOWN); 
    }

    return BT_RESULT::RUNNING;
}

CBT_Monster_DownAction* CBT_Monster_DownAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_DownAction");
        return nullptr;
    }

    return new CBT_Monster_DownAction(pOwner);
}

void CBT_Monster_DownAction::Free()
{
    __super::Free();
}
