#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_SkyBoss_IsTargetInRange final : public CBTCondition
{
public:
    explicit CBT_SkyBoss_IsTargetInRange(class CSkyBoss* pOwner, _float fRange);
    _bool Check_Condition();


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;
    _float m_fDetectionRange;

public:
    static CBT_SkyBoss_IsTargetInRange* Create(class CSkyBoss* pOwner, _float fRange);

    virtual void Free() override;
};
NS_END

