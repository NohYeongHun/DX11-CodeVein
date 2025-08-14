#pragma once
#include "BTAction.h"

/*
* 플레이어 머리위로 삼연 내려찍기 동작
*/
NS_BEGIN(Client)
class CBT_QueenKnight_TripleDownStrikeAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,           // 초기 상태
        READY,          // Attack 상태.
        ASCEND,         // 상승
        WAIT_TELEPORT,  // 상승 후 대기 (텔레포트 전)
        TELEPORT,       // 맵 중앙으로 텔레포트
        SKILL,          // 스킬.
        COMPLETED       // IDLE로 변환되어야 함.
    };

private:
    class CQueenKnight* m_pOwner = { nullptr };
    ATTACK_PHASE m_eAttackPhase = { ATTACK_PHASE::NONE };


private:
    // 중간 중간 애니메이션 속도 조절.
    _bool m_IsChangeSpeed = { false };
    _float m_fAddSpeed = {};    // 추가할. 애니메이션 스피드 
    _float m_fOriginSpeed = {}; // 원본 애니메이션 스피드


private:
    // 애니메이션 Ratio 조절
    _float m_fJump_StartRatio = {};
    _float m_fJump_EndRatio = {};

    // 총 250 프레임.
    _float m_fSkill_StartRatio = {} ;
    _float m_fSKill_EndRatio = {};

    // 목표 상승 지점.
    _float3 m_vAscendTarget = {};
    // 목표 하강 지점.
    _float3 m_vDesecndTarget = {};

    // 텔레포트 대기 시간
    _float m_fWaitTime = { 0.f };
    _float m_fMaxWaitTime = { 1.0f }; // 2초 대기

public:
    explicit CBT_QueenKnight_TripleDownStrikeAction(class CQueenKnight* pOwner);
    virtual ~CBT_QueenKnight_TripleDownStrikeAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT Enter_Attack(_float fTimeDelta);
    BT_RESULT Update_Ready(_float fTimeDelta);
    BT_RESULT Update_Ascend(_float fTimeDelta);
    BT_RESULT Update_WaitTeleport(_float fTimeDelta);
    BT_RESULT Update_Teleport(_float fTimeDelta);
    BT_RESULT Update_Skill(_float fTimeDelta);

public:
    static CBT_QueenKnight_TripleDownStrikeAction* Create(class CQueenKnight* pOwner);
    virtual void Free() override;
};
NS_END

