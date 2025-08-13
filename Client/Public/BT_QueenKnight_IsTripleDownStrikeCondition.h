#pragma once
#include "BTCondition.h"

/*
* 플레이어 머리위로 삼연 내려찍기 동작이 작동 가능한지 판별.
*/
NS_BEGIN(Client)
class CBT_QueenKnight_IsTripleDownStrikeCondition final : public CBTCondition
{
public:
    explicit CBT_QueenKnight_IsTripleDownStrikeCondition(class CQueenKnight* pOwner);
    _bool Check_Condition();


private:
    class CQueenKnight* m_pOwner;

public:
    static CBT_QueenKnight_IsTripleDownStrikeCondition* Create(class CQueenKnight* pOwner);

    virtual void Free() override;


};
NS_END

