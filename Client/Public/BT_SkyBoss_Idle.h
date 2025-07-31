#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_SkyBoss_Idle final : public CBTAction
{
public:
    explicit CBT_SkyBoss_Idle(class CSkyBoss* pOwner);
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

public:
    static CBT_SkyBoss_Idle* Create(class CSkyBoss* pOwner);
    virtual void Free() override;
};

NS_END
