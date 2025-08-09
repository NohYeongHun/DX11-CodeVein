#include "BT_Monster_AttackAction.h"

CBT_Monster_AttackAction::CBT_Monster_AttackAction(CMonster* pOwner)
    : m_pOwner(pOwner)
{
}

BT_RESULT CBT_Monster_AttackAction::Perform_Action(_float fTimeDelta)
{
    if (m_pOwner->HasBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return EnterAttack(fTimeDelta);
    case ATTACK_PHASE::ROTATING:
        return UpdateRotating(fTimeDelta);
    case ATTACK_PHASE::LOOP:
        return UpdateAttack(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
        return EndAttack(fTimeDelta);
    }
    return BT_RESULT::FAILURE;
}

// Action Node들의 경우에는 해당 Reset시 Phase를 초기화해줍니다.
void CBT_Monster_AttackAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;
}

BT_RESULT CBT_Monster_AttackAction::EnterAttack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }



    // 1. 다음 단계로 진행
    m_eAttackPhase = ATTACK_PHASE::ROTATING;

    // 2. 루트모션 설정.
    

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_AttackAction::UpdateRotating(_float fTimeDelta)
{
    // 0. 한번에 회전.
    m_pOwner->Rotate_ToTarget(fTimeDelta);

    if (m_pOwner->IsRotateFinished(XMConvertToRadians(10.f))) // 라디안 10도 차이까지만 허용
    {
        m_eAttackPhase = ATTACK_PHASE::LOOP;

        m_pOwner->Set_RootMotionTranslate(true);
        // 1. 공격 애니메이션 선택
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"ATTACK");

        // 2. 공격 상태로 변경
        m_pOwner->Change_Animation_Blend(iNextAnimationIdx);

        // 3. Collider 활성화 필요. => 공격용 콜라이더만 활성화.(Weapon?)
    }
        

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_AttackAction::UpdateAttack(_float fTimeDelta)
{
    if (m_pOwner->Is_Animation_Finished())
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_AttackAction::EndAttack(_float fTimeDleta)
{
    if (m_pOwner->Is_Animation_Finished())
    {
        // 1. Animation 탐색 시작
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        // 2. 현재 애니메이션으로 NON 블렌딩하면서 변경. => Idle은 NonBlend로 변경.
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx);


        m_pOwner->Set_RootMotionTranslate(false);
        // 디버그용 함수.
        //m_pOwner->Print_Position();
    }

    return BT_RESULT::SUCCESS;
}

CBT_Monster_AttackAction* CBT_Monster_AttackAction::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_AttackAction");
        return nullptr;
    }

    return new CBT_Monster_AttackAction(pOwner);
}

void CBT_Monster_AttackAction::Free()
{
    __super::Free();
    m_pOwner = nullptr;
}
