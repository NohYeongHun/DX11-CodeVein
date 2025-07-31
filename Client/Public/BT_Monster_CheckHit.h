#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_Monster_CheckHit final : public CBTCondition
{
public:
    explicit CBT_Monster_CheckHit(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CMonster* m_pOwner;

public:
    static CBT_Monster_CheckHit* Create(class CMonster* pOwner);

    virtual void Free() override;
};
NS_END

