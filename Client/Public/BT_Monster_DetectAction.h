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
        END,
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CMonster* m_pOwner = { nullptr };
    DETECT_PHASE m_eDetectPhase = { DETECT_PHASE::NONE };
    _bool m_IsRootMotion = { false };
    _bool m_bIsStopped = { false };  // 추적 중단 상태
    _float m_fMinDetectDuration = { 2.0f }; // 최소 탐지 유지 시간
    _float m_fDetectTimer = { 0.0f }; // 탐지 경과 시간

public:
    explicit CBT_Monster_DetectAction(class CMonster* pOwner);
    virtual ~CBT_Monster_DetectAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterDetect(_float fTimeDelta);
    BT_RESULT UpdateRotating(_float fTimeDelta);
    BT_RESULT UpdateWalk(_float fTimeDelta);
    BT_RESULT EndDetect(_float fTimeDleta);

private:
    _float CalculateDistanceToTarget();

public:
    static CBT_Monster_DetectAction* Create(class CMonster* pOwner);
    virtual void Free() override;
};
NS_END

