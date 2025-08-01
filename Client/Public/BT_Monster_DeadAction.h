#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_DeadAction : public CBTAction
{
public:
    enum class DEAD_PHASE : _ubyte
    {
        NONE,           // 초기 상태
        DYING,          // 죽는 애니메이션 재생 중
        CORPSE,         // 시체 상태 (잠시 유지)
        DISSOLVING,     // 디졸브 효과 중
        COMPLETED       // 완전히 제거됨
    };

private:
    class CSkyBoss* m_pOwner = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    // 공격 단계 관리
    DEAD_PHASE m_eDeadPhase = { DEAD_PHASE::NONE };
    _float m_fDeadTimer = 0.f;
    _float m_fDissolveTimer = 0.f;
    _float m_fCorpseLifetime = 3.0f;

    _uint m_iSelectedAnim = 0;
    _bool m_bAnimationSet = false;

public:
    explicit CBT_Monster_DeadAction(class CSkyBoss* pOwner);
    virtual ~CBT_Monster_DeadAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT StartDead();
    BT_RESULT UpdateDying(_float fTimeDelta);
    BT_RESULT UpdateCorpse(_float fTimeDelta);
    BT_RESULT UpdateDissolve(_float fTimeDelta);

private:
    void HandleDeathEvents();
    _uint SelectAnimation();

public:
    static CBT_Monster_DeadAction* Create(class CSkyBoss* pOwner);
    virtual void Free() override;

};
NS_END

