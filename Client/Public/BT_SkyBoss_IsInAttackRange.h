#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_SkyBoss_IsInAttackRange :
    public CBTCondition
{
public:
    explicit CBT_SkyBoss_IsInAttackRange(class CSkyBoss* pOwner, _float fAttackRange);
    virtual _bool Check_Condition() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

    static constexpr _float ATTACK_MIN_RANGE = 2.0f;    // 최소 거리
    static constexpr _float ATTACK_MAX_RANGE = 8.0f;    // 최대 거리

public:
    static CBT_SkyBoss_IsInAttackRange* Create(class CSkyBoss* pOwner, _float fAttackRange);
    virtual void Free() override;
};
NS_END

