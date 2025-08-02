#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_Monster_IsAttackRange final : public CBTCondition
{
public:
    explicit CBT_Monster_IsAttackRange(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsAttackRange* Create(class CMonster* pOwner);

    virtual void Free() override;

};
NS_END

