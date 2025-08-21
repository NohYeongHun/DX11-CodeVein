#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_IdleAction final : public CBTAction
{
public:
	enum class IDLE_PHASE : _ubyte
	{
		ENTER,         // 초기 상태
		LOOP,         // IDLE 상태
        COMPLETED,
        END
	};

private:
    class CMonster* m_pOwner = { nullptr };
	IDLE_PHASE m_eIdlePhase = { IDLE_PHASE::END };
public:
    explicit CBT_Monster_IdleAction(class CMonster* pOwner);
    virtual ~CBT_Monster_IdleAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT EnterIdle(_float fTimeDelta);
    
    //BT_RESULT LoopIdle(_float fTimeDelta);
    
   

public:
    static CBT_Monster_IdleAction* Create(class CMonster* pOwner);
    virtual void Free() override;

};
NS_END

