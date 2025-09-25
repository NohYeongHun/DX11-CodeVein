#pragma once
#include "BTAction.h"

NS_BEGIN(Client)

/* 콤보 공격. */
class CBT_QueenKnight_FirstPhase_AttackAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        ROTATING,     // 전환 상태.
        FIRST_ATTACK,      // Attack 상태.
        SECOND_ATTACK,
        LAST_ATTACK,
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CQueenKnight* m_pOwner = { nullptr };
    ATTACK_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };

    _bool m_IsFirstAttack = { false };
    _bool m_IsSecondAttack = { false };
    _bool m_IsLastAttack = { false };

    _bool m_IsFirstSoundPlay = { false };
    _bool m_IsSecondSoundPlay = { false };
    _float m_fFirstAttackSoundFrame = {};
    _float m_fSecondAttackSoundFrame = {};

public:
    explicit CBT_QueenKnight_FirstPhase_AttackAction(class CQueenKnight* pOwner);
    virtual ~CBT_QueenKnight_FirstPhase_AttackAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterAttack(_float fTimeDelta);
    BT_RESULT UpdateRotating(_float fTimeDelta);
    BT_RESULT UpdateFirstAttack(_float fTimeDelta);
    BT_RESULT UpdateSecondAttack(_float fTimeDelta);
    BT_RESULT UpdateLastAttack(_float fTimeDelta);
    BT_RESULT Complete(_float fTimeDleta);

public:
    static CBT_QueenKnight_FirstPhase_AttackAction* Create(class CQueenKnight* pOwner);
    virtual void Free() override;

};
NS_END

