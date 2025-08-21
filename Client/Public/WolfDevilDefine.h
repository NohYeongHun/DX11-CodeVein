#pragma once


NS_BEGIN(Client)
// ========== WolfDevil 애니메이션 Enum 분류 ==========
// Client_Defines.h 또는 Monster.h에 추가

// WolfDevil 몬스터 애니메이션 키 정의
enum WOLFDEVIL_ANIMATION_KEY : _uint
{
    // ===== 공격 애니메이션 =====
    WOLFDEVIL_ATTACK_JUMP = 0, WOLFDEVIL_ATTACK_NORMAL = 1,

    // ===== 피해 애니메이션 =====
    WOLFDEVIL_DAMAGE_BACK = 2, WOLFDEVIL_DAMAGE_FRONT = 3,

    // ===== 죽음 애니메이션 =====
    WOLFDEVIL_DEATH_BACK = 4, WOLFDEVIL_DEATH_NORMAL = 5,

    // ===== 회피 애니메이션 =====
    WOLFDEVIL_DODGE_BACK = 6,

    // ===== 다운 상태 애니메이션 (P = Primary) =====
    WOLFDEVIL_DOWN_P_END = 7, WOLFDEVIL_DOWN_P_LOOP = 8, WOLFDEVIL_DOWN_P_START = 9,

    // ===== 다운 상태 애니메이션 (S = Secondary) =====
    WOLFDEVIL_DOWN_S_END = 10, WOLFDEVIL_DOWN_S_LOOP = 11, WOLFDEVIL_DOWN_S_START = 12,

    // ===== 기본 상태 애니메이션 =====
    WOLFDEVIL_IDLE_LOOP = 13,

    // ===== 회전 애니메이션 =====
    WOLFDEVIL_TURN_LEFT_180 = 14, WOLFDEVIL_TURN_LEFT_90 = 15, WOLFDEVIL_TURN_RIGHT_180 = 16,
    WOLFDEVIL_TURN_RIGHT_90 = 17,

    // ===== 비전투 상태 애니메이션 =====
    WOLFDEVIL_NONFIGHT_IDLE_EAT = 18, WOLFDEVIL_NONFIGHT_IDLE_IDLE = 19, WOLFDEVIL_NONFIGHT_IDLE_SIT = 20,

    // ===== 위협 행동 애니메이션 (비전투) =====
    WOLFDEVIL_THREAT_EAT_01 = 21, WOLFDEVIL_THREAT_EAT_02 = 22, WOLFDEVIL_THREAT_SIT_01 = 23,
    WOLFDEVIL_THREAT_SIT_02 = 24,

    // ===== 이동 애니메이션 =====
    WOLFDEVIL_WALK_F = 25, WOLFDEVIL_RUN = 29,

    // ===== 상태 이상 애니메이션 =====
    WOLFDEVIL_STUN = 26,

    // ===== 일반 위협 애니메이션 =====
    WOLFDEVIL_THREAT_01 = 27, WOLFDEVIL_THREAT_02 = 28,

    WOLFDEVIL_ANIMATION_END
};

NS_END