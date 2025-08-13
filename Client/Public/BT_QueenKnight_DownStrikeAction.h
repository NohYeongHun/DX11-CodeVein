#pragma once
#include "BTAction.h"

/*
* 플레이어 머리위로 삼연 내려찍기 동작
*/
NS_BEGIN(Client)
class CBT_QueenKnight_DownStrikeAction final : public CBTAction
{
public:
    enum class ATTACK_PHASE : _ubyte
    {
        NONE,       // 초기 상태
        READY,      // Attack 상태.
        ASCEND,     // 상승
        HANG,       // 캐릭터 머리위로 이동.
        DESCEND,    // 하강.
        COMPLETED   // IDLE로 변환되어야 함.
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
    // 애니메이션 DownStrike Ratio 조절
    _float m_fReady_StartRatio = {};
    _float m_fReady_EndRatio = {};

    _float m_fJump_StartRatio = {};
    _float m_fJump_EndRatio = {};

    _float m_fAttack_StartRatio = {};
    _float m_fAttack_EndRatio = {};

    // 목표 상승 지점.
    _float3 m_vAscendTarget = {};
    // 목표 하강 지점.
    _float3 m_vDesecndTarget = {};

public:
    explicit CBT_QueenKnight_DownStrikeAction(class CQueenKnight* pOwner);
    virtual ~CBT_QueenKnight_DownStrikeAction() = default;

public:
    virtual BT_RESULT Perform_Action(_float fTimeDelta) override;
    virtual void Reset() override;

private:
    BT_RESULT Enter_Attack(_float fTimeDelta);
    BT_RESULT Update_Ready(_float fTimeDelta);
    BT_RESULT Update_Ascend(_float fTimeDelta);
    BT_RESULT Update_Hang(_float fTimeDelta);
    BT_RESULT Update_Descend(_float fTimeDelta);
   
    

public:
    static CBT_QueenKnight_DownStrikeAction* Create(class CQueenKnight* pOwner);
    virtual void Free() override;
};
NS_END

