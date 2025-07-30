#pragma once
#include "BTAction.h"
class CBT_PlayAnimation : public CBTAction
{
public:
    explicit CBT_PlayAnimation(_uint iAnimIndex, _bool bLoop = false, _float fMinDuration = 1.f);
    virtual ~CBT_PlayAnimation() = default;

    virtual BT_RESULT Execute(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    class CGameObject* m_pOwner = { nullptr };
    _uint m_iAnimIndex;
    _bool m_bLoop;
    _float m_fMinDuration;
    _float m_fCurrentTime = 0.f;
    _bool m_bAnimStarted = false;

public:
    virtual void Free();
};

