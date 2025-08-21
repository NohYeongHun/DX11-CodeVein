#pragma once
#include "BTCondition.h"

NS_BEGIN(Client)
class CBT_QueenKnight_IsDashAttackCondition final : public CBTCondition
{
public:
    explicit CBT_QueenKnight_IsDashAttackCondition(class CQueenKnight* pOwner);
    _bool Check_Condition();


private:
    class CQueenKnight* m_pOwner;

public:
    static CBT_QueenKnight_IsDashAttackCondition* Create(class CQueenKnight* pOwner);

    virtual void Free() override;
};
NS_END

