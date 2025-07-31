#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_SkyBoss_CanStrongAttack final : public CBTCondition
{
public:
    explicit CBT_SkyBoss_CanStrongAttack(class CSkyBoss* pOwner);
    virtual _bool Check_Condition() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

    // 강공격 조건 관련 상수들
    static constexpr _float STRONG_ATTACK_COOLDOWN = 5.0f;     // 강공격 쿨타임 (5초)
    static constexpr _float STRONG_ATTACK_MIN_RANGE = 2.0f;    // 최소 거리
    static constexpr _float STRONG_ATTACK_MAX_RANGE = 8.0f;    // 최대 거리
    static constexpr _float STRONG_ATTACK_HP_THRESHOLD = 0.7f; // HP 70% 이하일 때 더 자주 사용
    static constexpr _float PLAYER_LOW_HP_THRESHOLD = 0.3f;    // 플레이어 HP가 낮을 때 마무리용

public:
    static CBT_SkyBoss_CanStrongAttack* Create(class CSkyBoss* pOwner);
    virtual void Free() override;
};
NS_END
