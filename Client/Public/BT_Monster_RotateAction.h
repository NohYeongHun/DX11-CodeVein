#pragma once


/*
* 몬스터 회전 행동 
* 몬스터가 플레이러를 바라보도록 하는 행동을 정의
*/
NS_BEGIN(Client)
class CBT_Monster_RotateAction final : public CBTAction
{
public:
    enum class ROTATION_PHASE : _ubyte
    {
        ENTER,         // 초기 상태
        ROTATE,
        COMPLETED,
        END
    };

private:
    class CMonster* m_pOwner = { nullptr };
    ROTATION_PHASE m_eRotPhase = { ROTATION_PHASE::END };

public:
    explicit CBT_Monster_RotateAction(class CMonster* pOwner);
    virtual ~CBT_Monster_RotateAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT Enter_Rotation(_float fTimeDelta);
    BT_RESULT Update_Rotation(_float fTimeDelta);


public:
    static CBT_Monster_RotateAction* Create(class CMonster* pOwner);
    virtual void Free() override;
};
NS_END

