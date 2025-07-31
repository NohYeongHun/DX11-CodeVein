#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_SkyBoss_Attack final : public CBTAction
{
public:
    explicit CBT_SkyBoss_Attack(class CSkyBoss* pOwner);
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

    static constexpr _float STRONG_ATTACK_MIN_RANGE = 2.0f;    // 최소 거리
    static constexpr _float STRONG_ATTACK_MAX_RANGE = 8.0f;    // 최대 거리

public:
    static CBT_SkyBoss_Attack* Create(class CSkyBoss* pOwner);
    virtual void Free() override;
};
NS_END

