#include "BT_Monster_DeadAction.h"
/*
* 애니메이션 시간과 동기화 시킬 방법이 있나?
* 아님 걍 때려맞춰야하나?
*/
CBT_Monster_DeadAction::CBT_Monster_DeadAction(CSkyBoss* pOwner)
    : m_pOwner{ pOwner }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

BT_RESULT CBT_Monster_DeadAction::Perform_Action(_float fTimeDelta)
{
    switch (m_eDeadPhase)
    {
    case DEAD_PHASE::NONE:
        return StartDead();
    case DEAD_PHASE::DYING:
        return UpdateDying(fTimeDelta);
    case DEAD_PHASE::CORPSE:
        return UpdateCorpse(fTimeDelta);
    case DEAD_PHASE::DISSOLVING:
        return UpdateDissolve(fTimeDelta);
    case DEAD_PHASE::COMPLETED:
        return BT_RESULT::SUCCESS;
    }
    return BT_RESULT::FAILURE;
}

void CBT_Monster_DeadAction::Reset()
{
    m_eDeadPhase = DEAD_PHASE::NONE;
    m_fDeadTimer = 0.f;
    m_iSelectedAnim = 0;
    m_bAnimationSet = false;
}

BT_RESULT CBT_Monster_DeadAction::StartDead()
{
    // 안전 코드 추가
    if (m_eDeadPhase != DEAD_PHASE::NONE)
        return BT_RESULT::RUNNING;

    // 1. 죽는 애니메이션 선택
    m_iSelectedAnim = SelectAnimation();
    m_bAnimationSet = true;

    // 2. 죽는 상태로 변경
    m_pOwner->Change_State(MONSTER_DEATH);

    m_pOwner->Change_Animation(m_iSelectedAnim, false);

    // 3. 콜리전 비활성화 (즉시)
    // m_pOwner->Set_CollisionEnabled(false);

    // 4. 죽음 이벤트 처리 (점수, 아이템 드롭 등)
    HandleDeathEvents();

    // 5. 다음 단계로 진행
    m_eDeadPhase = DEAD_PHASE::DYING;
    m_fDeadTimer = 0.f;

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateDying(_float fTimeDelta)
{
    m_fDeadTimer += fTimeDelta;

    bool bIsAnimationEnd = m_pOwner->Is_Animation_Finished();

    // 애니메이션이 끝났다면 시체 상태로 전환
    if (bIsAnimationEnd)
    {
        m_pOwner->Change_State(MONSTER_CORPSE);
        m_eDeadPhase = DEAD_PHASE::CORPSE;
        m_fDeadTimer = 0.f;
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateCorpse(_float fTimeDelta)
{
    m_fDeadTimer += fTimeDelta;

    // 시체 상태를 일정 시간 유지
    if (m_fDeadTimer >= m_fCorpseLifetime)
    {
        // 디졸브 효과 시작
        m_pOwner->Change_State(MONSTER_DISSOLVING);
        //m_pOwner->Start_Dissolve_Effect();
        m_eDeadPhase = DEAD_PHASE::DISSOLVING;
        m_fDissolveTimer = 0.f;
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_Monster_DeadAction::UpdateDissolve(_float fTimeDelta)
{
    m_fDissolveTimer += fTimeDelta;

    // 디졸브 효과 업데이트
    _float dissolveProgress = m_fDissolveTimer / 2.0f; // 2초간 디졸브
    //m_pOwner->Update_Dissolve_Effect(dissolveProgress);

    // 디졸브 완료
    if (dissolveProgress >= 1.0f)
    {
        m_pOwner->Set_Dead(); // 완전히 제거
        m_eDeadPhase = DEAD_PHASE::COMPLETED;
        return BT_RESULT::SUCCESS;
    }

    return BT_RESULT::RUNNING;
}



void CBT_Monster_DeadAction::HandleDeathEvents()
{
    // 사운드 재생
    //m_pGameInstance->Play_Sound(L"Monster_Death", 0.7f);

    // 파티클 이펙트
    //m_pOwner->Play_Death_Effect();

    // AI나 매니저에게 죽음 알림
    //if (auto pMonsterManager = m_pGameInstance->Get_MonsterManager())
    //    pMonsterManager->On_Monster_Death(m_pOwner);
}

_uint CBT_Monster_DeadAction::SelectAnimation()
{
 
    return SkyBossAnims::DEATH;
}


CBT_Monster_DeadAction* CBT_Monster_DeadAction::Create(CSkyBoss* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_DeadAction");
        return nullptr;
    }

    return new CBT_Monster_DeadAction(pOwner);
}

void CBT_Monster_DeadAction::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
