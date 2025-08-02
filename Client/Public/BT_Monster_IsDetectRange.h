#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
/*
* 기본 탐지 거리.
* => 공격이 가능한가에 대한 탐지거리 노드가 아니라
* => 몬스터가 플레이어를 `탐색` 했다는 거리.
*/
class CBT_Monster_IsDetectRange final : public CBTCondition
{
public:
    explicit CBT_Monster_IsDetectRange(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsDetectRange* Create(class CMonster* pOwner);

    virtual void Free() override;
};
NS_END

