#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_SkyBoss_DownCondition :
    public CBTCondition
{
public:
    explicit CBT_SkyBoss_DownCondition(class CSkyBoss* pOwner, _float fAttackRange);
    virtual _bool Check_Condition() override;
    virtual void Reset() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;
    _float m_fStaggerThreshold = 100.0f;  // 스태거 값 임계점

public:
    static CBT_SkyBoss_DownCondition* Create(class CSkyBoss* pOwner, _float fAttackRange);
    virtual void Free() override;
};
NS_END

