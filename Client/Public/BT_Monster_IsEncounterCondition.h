#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
/* 플레이어와 몬스터가 조우한 상태인가? */
class CBT_Monster_IsEncounterCondition final : public CBTCondition
{
public:
    explicit CBT_Monster_IsEncounterCondition(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsEncounterCondition* Create(class CMonster* pOwner);

    virtual void Free() override;
};
NS_END

