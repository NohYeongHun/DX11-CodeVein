#pragma once
//0. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_AttackJump01_N
//1. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_AttackNormal01_N
//2. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Damage_B
//3. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Damage_F
//4. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Death_B
//5. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Death_N
//6. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Dodge_B
//7. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_P_End
//8. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_P_Loop
//9. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_P_Start
//10. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_S_End
//11. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_S_Loop
//12. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Down_S_Start
//13. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Idle_N_Loop
//14. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_IdleTurn_L180
//15. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_IdleTurn_L90
//16. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_IdleTurn_R180
//17. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_IdleTurn_R90
//18. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightIdleEat_N_Loop
//19. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightIdleIdle_N_Loop
//20. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightIdleSit_N_Loop
//21. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightThreatEat01_N
//22. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightThreatEat02_N
//23. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightThreatSit01_N
//24. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightThreatSit02_N
//25. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_NoneFightWalk_F_Loop
//26. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_StatusAilment_Stun_N
//27. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Threat01_N
//28. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Threat02_N
//29. Animation Name : SK_WolfDevil_LOD0.ao | AS_TWolfDevil_TCmn_Walk_F_Loop

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