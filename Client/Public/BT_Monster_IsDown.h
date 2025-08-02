#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_Monster_IsDown final : public CBTCondition
{
public:
    explicit CBT_Monster_IsDown(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsDown* Create(class CMonster* pOwner);
    virtual void Free() override;

};
NS_END

