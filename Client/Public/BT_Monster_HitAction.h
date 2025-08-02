#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_HitAction final : public CBTAction
{
public:
    enum class HIT_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        LOOP,         // Hit 상태.
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CMonster* m_pOwner = { nullptr };

    // 죽는 단계 관리?
    HIT_PHASE m_eHitPhase = { HIT_PHASE::NONE };

public:
    explicit CBT_Monster_HitAction(class CMonster* pOwner);
    virtual ~CBT_Monster_HitAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterHit(_float fTimeDelta);
    BT_RESULT UpdateHit(_float fTimeDelta);
    BT_RESULT EndHit(_float fTimeDleta);

public:
    static CBT_Monster_HitAction* Create(class CMonster* pOwner);
    virtual void Free() override;

};
NS_END

