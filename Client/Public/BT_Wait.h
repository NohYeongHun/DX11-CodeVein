#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Wait : public CBTAction
{
public:
    explicit CBT_Wait(_float fWaitTime = 2.f);
    virtual ~CBT_Wait() = default;

    virtual BT_RESULT Execute(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    class CGameObject* m_pOwner = { nullptr };
    _float m_fWaitTime;
    _float m_fCurrentTime = 0.f;

public:
    virtual void Free();
};
NS_END

