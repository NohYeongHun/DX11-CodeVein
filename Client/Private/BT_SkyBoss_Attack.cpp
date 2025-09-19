// BT_SkyBoss_Attack.cpp
#include "BT_SkyBoss_Attack.h"

CBT_SkyBoss_Attack::CBT_SkyBoss_Attack(CSkyBoss* pOwner)
    : m_pOwner(pOwner)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_eAttackPhase(ATTACK_PHASE::NONE)
    , m_fAttackTimer(0.f)
    , m_iSelectedAttackAnim(0)
{
    Safe_AddRef(m_pGameInstance);
}

BT_RESULT CBT_SkyBoss_Attack::Perform_Action(_float fTimeDelta)
{
   /* if (m_pOwner->Get_Target() && m_eAttackPhase == ATTACK_PHASE::PREPARING)
    {
        m_pOwner->Smooth_Rotate_To_Target(fTimeDelta, m_pOwner->Get_RotationSpeed());
    }*/

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return StartAttack();

    case ATTACK_PHASE::PREPARING:
       /* if (m_pOwner->Get_Target())
        {
            m_pOwner->Smooth_Rotate_To_Target(fTimeDelta, m_pOwner->Get_RotationSpeed());
        }*/
        return UpdatePreparing(fTimeDelta);

    case ATTACK_PHASE::ATTACKING:
        return UpdateAttacking(fTimeDelta);

    case ATTACK_PHASE::RECOVERY:
        return UpdateRecovery(fTimeDelta);

    case ATTACK_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }

    return BT_RESULT::FAILURE;
}

BT_RESULT CBT_SkyBoss_Attack::StartAttack()
{
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
        return BT_RESULT::RUNNING;

    // 1. 랜덤 공격 애니메이션 선택
    m_iSelectedAttackAnim = SelectAttackAnimation();

    // 2. 공격 상태로 변경
    //m_pOwner->Change_State(MONSTER_ATTACK);
    //m_pOwner->Change_Animation(m_iSelectedAttackAnim, false);

    // 3. 다음 단계로 진행
    m_eAttackPhase = ATTACK_PHASE::PREPARING;
    m_fAttackTimer = 0.f;
    m_bDamageDealt = false;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_SkyBoss_Attack::UpdatePreparing(_float fTimeDelta)
{
    m_fAttackTimer += fTimeDelta;

    // 공격 준비 시간 (애니메이션 초반부)
    const _float fPrepareTime = 0.3f;

    if (m_fAttackTimer >= fPrepareTime)
    {
        m_eAttackPhase = ATTACK_PHASE::ATTACKING;
        m_fAttackTimer = 0.f;
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_SkyBoss_Attack::UpdateAttacking(_float fTimeDelta)
{
    m_fAttackTimer += fTimeDelta;

    // 데미지 처리 (한 번만 실행)
    if (!m_bDamageDealt && m_fAttackTimer >= 0.1f)
    {
        DealDamageToTarget();
        m_bDamageDealt = true;
    }

    // 공격 지속 시간
    const _float fAttackDuration = 0.4f;

    if (m_fAttackTimer >= fAttackDuration)
    {
        m_eAttackPhase = ATTACK_PHASE::RECOVERY;
        m_fAttackTimer = 0.f;
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_SkyBoss_Attack::UpdateRecovery(_float fTimeDelta)
{
    m_fAttackTimer += fTimeDelta;

    // 애니메이션이 끝났거나 최대 대기 시간 도달
    //if (m_pOwner->Is_Animation_Finished() || m_fAttackTimer >= 1.0f)
    //{
    //    m_eAttackPhase = ATTACK_PHASE::COMPLETED;

    //    // 대기 상태로 복귀
    //    m_pOwner->Change_State(MONSTER_IDLE);
    //    return BT_RESULT::SUCCESS;
    //}

    return BT_RESULT::RUNNING;
}

_uint CBT_SkyBoss_Attack::SelectAttackAnimation()
{
    //return SkyBossAnims::DEATH;

    // 애니메이션 초기화가 안되나본데?
    return SKYBOSS_ANIM_SWORD_NORMAL_ATTACK1;
    //if (fDistanceToTarget <= 5.f)
    //{
    //    // 근거리: 검 공격
    //    int index = rand() % 3;
    //    return SkyBossAnims::NORMAL_ATTACKS[index];
    //}
    //else
    //{
    //    // 중거리: 도끼 공격
    //    return SKYBOSS_ANIM_AXE_NORMAL_ATTACK1 + (rand() % 4);
    //}
}

void CBT_SkyBoss_Attack::DealDamageToTarget()
{
    if (!m_pOwner->Get_Target())
        return;

    //_float fDistance = m_pOwner->Get_Distance_To_Target();

    //// 공격 범위 내에 있으면 데미지 처리
    //if (fDistance <= m_pOwner->Get_AttackRange())
    //{
    //    // 데미지 계산 및 처리
    //    _float fDamage = m_pOwner->Get_AttackPower();

    //    // 플레이어에게 데미지 전달
    //    // m_pOwner->Get_Target()->Take_Damage(fDamage, m_pOwner);

    //    // 공격 이펙트 생성
    //    // CreateAttackEffect();
    //}
}


void CBT_SkyBoss_Attack::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;
    m_fAttackTimer = 0.f;
    m_bDamageDealt = false;
    m_iSelectedAttackAnim = 0;
    
    // m_bAnimationSet = false; ← 이 줄 제거
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
    CBTAction::Free();
    Safe_Release(m_pGameInstance);
}