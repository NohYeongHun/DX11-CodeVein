#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_AttackAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        ROTATING,     // 전환 상태.
        LOOP,         // Attack 상태.
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CMonster* m_pOwner = { nullptr };
    ATTACK_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };

public:
    explicit CBT_Monster_AttackAction(class CMonster* pOwner);
    virtual ~CBT_Monster_AttackAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterAttack(_float fTimeDelta);
    BT_RESULT UpdateRotating(_float fTimeDelta);
    BT_RESULT UpdateAttack(_float fTimeDelta);
    BT_RESULT EndAttack(_float fTimeDleta);

public:
    static CBT_Monster_AttackAction* Create(class CMonster* pOwner);
    virtual void Free() override;
};
NS_END

