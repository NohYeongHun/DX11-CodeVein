#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_Monster_PrevEncounterAction final : public CBTAction
{
public:
    enum class ENCOUNTER_PHASE : _ubyte
    {
        ENTER,         // 초기 상태
        LOOP,
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CMonster* m_pOwner = { nullptr };
    ENCOUNTER_PHASE m_eEncounterPhase = { ENCOUNTER_PHASE::ENTER };
    _bool m_IsAnimationFinished = { false };
public:
    explicit CBT_Monster_PrevEncounterAction(class CMonster* pOwner);
    virtual ~CBT_Monster_PrevEncounterAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT Enter_Encounter(_float fTimeDelta);
    BT_RESULT Update_Encounter(_float fTimeDelta);
    BT_RESULT Complete(_float fTimeDleta);

public:
    static CBT_Monster_PrevEncounterAction* Create(class CMonster* pOwner);
    virtual void Free() override;
};
NS_END

