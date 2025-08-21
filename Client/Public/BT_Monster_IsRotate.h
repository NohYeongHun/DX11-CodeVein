#pragma once

NS_BEGIN(Client)
class CBT_Monster_IsRotate final : public CBTCondition
{
public:
    explicit CBT_Monster_IsRotate(class CMonster* pOwner);
    _bool Check_Condition();


private:
    class CMonster* m_pOwner;

public:
    static CBT_Monster_IsRotate* Create(class CMonster* pOwner);

    virtual void Free() override;

};
NS_END

