#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_GiantWhiteDevil_ComboAttackAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        FIRST_ATTACK,      // Attack 상태.
        SECOND_ATTACK,
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CGiant_WhiteDevil* m_pOwner = { nullptr };
    ATTACK_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };

    _bool m_IsFirstAttack = { false };
	_bool m_IsSecondAttack = { false };

public:
    explicit CBT_GiantWhiteDevil_ComboAttackAction(class CGiant_WhiteDevil* pOwner);
    virtual ~CBT_GiantWhiteDevil_ComboAttackAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterAttack(_float fTimeDelta);
    BT_RESULT UpdateFirstAttack(_float fTimeDelta);
    BT_RESULT UpdateSecondAttack(_float fTimeDelta);
    BT_RESULT Complete(_float fTimeDleta);

public:
    static CBT_GiantWhiteDevil_ComboAttackAction* Create(class CGiant_WhiteDevil* pOwner);
    virtual void Free() override;
};
NS_END

