#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_SkyBoss_IsDownStart : public CBTAction
{
public:
    enum class DOWN_PHASE : _ubyte
    {
        NONE = 0,       // 초기 상태
        PREPARING,      // 공격 준비 (조준, 예비동작)
        ATTACKING,      // 실제 공격 수행
        RECOVERY,       // 공격 후 복구
        COMPLETED       // 완료
    };

private:
    CSkyBoss* m_pOwner = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    // 공격 단계 관리
    DOWN_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };
    _float m_fAttackTimer = { 0.f };
    _uint m_iSelectedAttackAnim = { 0 };
    _bool m_IsDamageDealt = { false };
    _bool m_IsAnimationSet = false;

public:
    explicit CBT_SkyBoss_IsDownStart(CSkyBoss* pOwner);
    virtual ~CBT_SkyBoss_IsDownStart() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT StartAttack();
    BT_RESULT UpdatePreparing(_float fTimeDelta);
    BT_RESULT UpdateAttacking(_float fTimeDelta);
    BT_RESULT UpdateRecovery(_float fTimeDelta);

    _uint SelectAttackAnimation();
    void DealDamageToTarget();

public:
    static CBT_SkyBoss_Attack* Create(CSkyBoss* pOwner);
    virtual void Free() override;
};
NS_END

