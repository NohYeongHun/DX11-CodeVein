#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_IdleAction final : public CBTAction
{

private:
    class CMonster* m_pOwner = { nullptr };

public:
    explicit CBT_Monster_IdleAction(class CMonster* pOwner);
    virtual ~CBT_Monster_IdleAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT LoopIdle(_float fTimeDelta);
   

public:
    static CBT_Monster_IdleAction* Create(class CMonster* pOwner);
    virtual void Free() override;

};
NS_END

