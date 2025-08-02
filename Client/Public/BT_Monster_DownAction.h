#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_DownAction final : public CBTAction
{
public:
    enum class DOWN_PHASE : _ubyte
    {
        NONE,          // 초기 상태
        START,          // 다운된 상태.
        LOOP,          // 다운된 상태.
        END,          // 다운된 상태.
        COMPLETED      // IDLE로 변환되어야 함.
    };



public:
    explicit CBT_Monster_DownAction(class CMonster* pOwner);
    virtual ~CBT_Monster_DownAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterDown();
    BT_RESULT StartDown(_float fTimeDelta);
    BT_RESULT LoopDown(_float fTimeDelta);
    BT_RESULT EndDown(_float fTimeDelta);

private:
    class CMonster* m_pOwner = { nullptr };
    // Down Phase 관리.
    DOWN_PHASE m_eDownPhase = { DOWN_PHASE::NONE };


public:
    static CBT_Monster_DownAction* Create(class CMonster* pOwner);
    virtual void Free() override;

};
NS_END

