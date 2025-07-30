#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_IsTargetInRange final : public CBTCondition
{
public:
    explicit CBT_IsTargetInRange(CGameObject* pOwner, _float fRange);
    _bool Check_Condition();


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CGameObject* m_pOwner;
    _float m_fDetectionRange;

public:
    static CBT_IsTargetInRange* Create(CGameObject* pOwner, _float fRange);

    virtual void Free() override;
};
NS_END

