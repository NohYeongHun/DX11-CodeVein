#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_SkyBoss_IsStrongHit final : public CBTCondition
{
public:
    explicit CBT_SkyBoss_IsStrongHit(class CSkyBoss* pOwner);
    virtual _bool Check_Condition() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

public:
    static CBT_SkyBoss_IsStrongHit* Create(class CSkyBoss* pOwner);
    virtual void Free() override;
};
NS_END

