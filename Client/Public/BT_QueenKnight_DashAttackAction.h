#pragma once
#include "BTAction.h"

NS_BEGIN(Client)
class CBT_QueenKnight_DashAttackAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,         // 초기 상태
        ROTATING,     // 전환 상태.
        DODGE_B, // Attack 상태.
        FIRST_ATTACK, // Attack 상태.
        LAST_ATTACK,  // Attack 종료 상태. 
        COMPLETED     // IDLE로 변환되어야 함.
    };

private:
    class CQueenKnight* m_pOwner = { nullptr };
    ATTACK_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };

    _bool m_IsChangeSpeed = { false };
    _float m_fAddSpeed = {};    // 추가할. 애니메이션 스피드 
    _float m_fOriginSpeed = {}; // 원본 애니메이션 스피드


    CBounding_OBB::BOUNDING_OBB_DESC* m_OriginDesc = { nullptr };
    _bool m_IsColliderChange = { false };

    _float m_fDodgeRatio = { 110.f / 208.f};
    _float m_fDashRatio = { 130.f / 137.f };
    _float m_fDashTime = {};
    _bool m_IsSpeedChange = { false };

    _float m_fDodgeSoundFrame = { 61.f / 208.f}; // 동작이 완성됨.
    _float m_fDashSoundFrame = {};
    _bool m_IsDodgeSoundPlay = { false };
    _bool m_IsDashSoundPlay = { false };


public:
    explicit CBT_QueenKnight_DashAttackAction(class CQueenKnight* pOwner);
    virtual ~CBT_QueenKnight_DashAttackAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT Enter_Attack(_float fTimeDelta);
    BT_RESULT Update_Rotating(_float fTimeDelta);
    BT_RESULT Update_Dodge(_float fTimeDelta);
    BT_RESULT Update_FirstAttack(_float fTimeDelta);
    BT_RESULT Update_LastAttack(_float fTimeDelta);
    BT_RESULT Complete(_float fTimeDleta);

public:
    static CBT_QueenKnight_DashAttackAction* Create(class CQueenKnight* pOwner);
    virtual void Free() override;

};
NS_END

