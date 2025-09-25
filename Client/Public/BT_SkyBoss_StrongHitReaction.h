#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_SkyBoss_StrongHitReaction final : public CBTAction
{
public:
    explicit CBT_SkyBoss_StrongHitReaction(class CSkyBoss* pOwner, _uint iAnimationIndex);
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CSkyBoss* m_pOwner;

    _uint m_iAnimationIndex = {};
    _float m_fReactionTime = 0.f;
    const _float m_fMaxReactionTime = 1.2f;  // 강한 피격은 더 오래 경직
    _bool m_IsAnimationStarted = false;
    

public:
    static CBT_SkyBoss_StrongHitReaction* Create(class CSkyBoss* pOwner, _uint iAnimationIndex);
    virtual void Free() override;
};
NS_END

