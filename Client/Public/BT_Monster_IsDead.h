#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_Monster_IsDead final : public CBTCondition
{
public:
    explicit CBT_Monster_IsDead(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsDead* Create(class CMonster* pOwner);

    virtual void Free() override;

   
};
NS_END

