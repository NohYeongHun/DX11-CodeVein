#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_DetectAction final : public CBTAction
{
public:
    enum class DETECT_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        ROTATING,     // 전환 상태.
        LOOP,         // WALK 상태.
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CMonster* m_pOwner = { nullptr };
    DETECT_PHASE m_eDetectPhase = { DETECT_PHASE::NONE };
    _bool m_IsRootMotion = { false };

public:
    explicit CBT_Monster_DetectAction(class CMonster* pOwner,  _bool IsRootMotion = false);
    virtual ~CBT_Monster_DetectAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterDetect(_float fTimeDelta);
    BT_RESULT UpdateRotating(_float fTimeDelta);
    BT_RESULT UpdateWalk(_float fTimeDelta);
    BT_RESULT EndDetect(_float fTimeDleta);

public:
    static CBT_Monster_DetectAction* Create(class CMonster* pOwner, _bool IsRootMotion = false);
    virtual void Free() override;
};
NS_END

