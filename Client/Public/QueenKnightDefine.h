#pragma once

NS_BEGIN(Client)

// QueenKnight 애니메이션 키 정의 (최신 버전)
enum QUEEN_KNIGHT_ANIMATION_KEY : _uint
{
    // ===== 데미지 애니메이션 (0-14) =====
    AS_TStdKnight_TCmn_Damage01_BL = 0, AS_TStdKnight_TCmn_Damage01_BR = 1, AS_TStdKnight_TCmn_Damage01_FL = 2,
    AS_TStdKnight_TCmn_Damage02_BL = 3, AS_TStdKnight_TCmn_Damage02_BR = 4, AS_TStdKnight_TCmn_Damage02_FL = 5,
    AS_TStdKnight_TCmn_Damage02_FR = 6, AS_TStdKnight_TCmn_Damage03_BL = 7, AS_TStdKnight_TCmn_Damage03_BR = 8,
    AS_TStdKnight_TCmn_Damage03_FL = 9, AS_TStdKnight_TCmn_Damage03_FR = 10, AS_TStdKnight_TCmn_DamageBackStab_N = 11,
    AS_TStdKnight_TCmn_DamageBlow_B = 12, AS_TStdKnight_TCmn_DamageBlow_F = 13, AS_TStdKnight_TCmn_DamageGauntletComb_B = 14,

    // ===== 추가 데미지 및 죽음 애니메이션 (15-17) =====
    AS_TStdKnight_TCmn_DamageStrike_P = 15, AS_TStdKnight_TCmn_Death_F = 16, AS_TStdKnight_TCmn_Death_N = 17,

    // ===== 회피 애니메이션 (18) =====
    AS_TStdKnight_TCmn_Dodge_B = 18,

    // ===== 다운 상태 애니메이션 (19-20) =====
    AS_TStdKnight_TCmn_Down_P_End = 19, AS_TStdKnight_TCmn_Down_P_Loop = 20,

    // ===== 전투 외 대기 애니메이션 (21-22) =====
    AS_TStdKnight_TCmn_NoneFightIdleCrouch_N = 21, AS_TStdKnight_TCmn_NoneFightIdleCrouch_N_Loop = 22,

    // ===== 랜스 변신 애니메이션 (23-24) =====
    AS_TStdKnight_TLanceGCS_Appearance_N = 23, AS_TStdKnight_TLanceGCS_Appearance_N_Loop = 24,

    // ===== 랜스 회피 애니메이션 (25-26) =====
    AS_TStdKnight_TLanceGCS_AttackAvoid01_B = 25, AS_TStdKnight_TLanceGCS_AttackAvoid02_B = 26,

    // ===== 랜스 백스텝 애니메이션 (27) =====
    AS_TStdKnight_TLanceGCS_AttackBackStep01_N = 27,

    // ===== 랜스 콤보 공격 애니메이션 (28-29) =====
    AS_TStdKnight_TLanceGCS_AttackCombo01_N = 28, AS_TStdKnight_TLanceGCS_AttackCombo02_N = 29,

    // ===== 랜스 필드 및 가드 공격 애니메이션 (30-31) =====
    AS_TStdKnight_TLanceGCS_AttackField01_N = 30, AS_TStdKnight_TLanceGCS_AttackGuard01_N = 31,

    // ===== 랜스 일반 공격 애니메이션 (32-34) =====
    AS_TStdKnight_TLanceGCS_AttackNormal01_N = 32, AS_TStdKnight_TLanceGCS_AttackNormal02_N = 33, AS_TStdKnight_TLanceGCS_AttackNormal02_N_Combo = 34,

    // ===== 랜스 회전 공격 애니메이션 (35) =====
    AS_TStdKnight_TLanceGCS_AttackRotation01_N = 35,

    // ===== 랜스 쉴드 공격 애니메이션 (36-37) =====
    AS_TStdKnight_TLanceGCS_AttackShield01_N = 36, AS_TStdKnight_TLanceGCS_AttackShield01_N_Combo = 37,

    // ===== 랜스 슬라이드 회전 공격 애니메이션 (38-39) =====
    AS_TStdKnight_TLanceGCS_AttackSlideRotation01_N = 38, AS_TStdKnight_TLanceGCS_AttackSlideRotation02_N = 39,

    // ===== 랜스 찌르기 공격 애니메이션 (40-41) =====
    AS_TStdKnight_TLanceGCS_AttackThrust01_N = 40, AS_TStdKnight_TLanceGCS_AttackThrust01_N_Combo = 41,

    // ===== 랜스 워프 공격 애니메이션 (42-45) =====
    AS_TStdKnight_TLanceGCS_AttackWarp_End01 = 42, AS_TStdKnight_TLanceGCS_AttackWarp_End02 = 43, AS_TStdKnight_TLanceGCS_AttackWarp_Start = 44,
    AS_TStdKnight_TLanceGCS_AttackWarpJump01_N = 45,

    // ===== 랜스 죽음 및 변형 애니메이션 (46-47) =====
    AS_TStdKnight_TLanceGCS_Death_N = 46, AS_TStdKnight_TLanceGCS_Deformation_N = 47,

    // ===== 랜스 다운 상태 애니메이션 (48-50) =====
    AS_TStdKnight_TLanceGCS_Down_P_End = 48, AS_TStdKnight_TLanceGCS_Down_P_Loop = 49, AS_TStdKnight_TLanceGCS_Down_P_Start = 50,

    // ===== 랜스 가드 이동 애니메이션 (51) =====
    AS_TStdKnight_TLanceGCS_Guard_Walk_B_Loop = 51,

    // ===== 랜스 대기 애니메이션 (52) =====
    AS_TStdKnight_TLanceGCS_Idle_N_Loop = 52,

    // ===== 랜스 달리기 애니메이션 (53-54) =====
    AS_TStdKnight_TLanceGCS_Run_F_Loop = 53, AS_TStdKnight_TLanceGCS_Run_F_Loop_001 = 54,

    // ===== 랜스 상태이상 애니메이션 (55) =====
    AS_TStdKnight_TLanceGCS_StatusAilment_Stun_N = 55,

    // ===== 라이트 소드 공격 애니메이션 (56-64) =====
    AS_TStdKnight_TLSword_AttackKick01_N = 56, AS_TStdKnight_TLSword_AttackNormal01_N = 57, AS_TStdKnight_TLSword_AttackNormal02_N = 58,
    AS_TStdKnight_TLSword_AttackNormal03_N = 59, AS_TStdKnight_TLSword_AttackSmash01_N = 60, AS_TStdKnight_TLSword_AttackStrong01_N = 61,
    AS_TStdKnight_TLSword_AttackStrong02_N = 62, AS_TStdKnight_TLSword_AttackStrong03_N = 63, AS_TStdKnight_TLSword_AttackSwing01_N = 64,

    // ===== 라이트 소드 대기 애니메이션 (65) =====
    AS_TStdKnight_TLSword_Idle_N_Loop = 65,

    // ===== 쉴드+라이트소드 가드 애니메이션 (66-67) =====
    AS_TStdKnight_TShieldLSword_Guard_N_End = 66, AS_TStdKnight_TShieldLSword_Guard_N_Start = 67,

    // ===== 쉴드+소드 공격 애니메이션 (68-72) =====
    AS_TStdKnight_TShieldSword_AttackShield01_N = 68, AS_TStdKnight_TShieldSword_AttackShield02A_N = 69, AS_TStdKnight_TShieldSword_AttackShield02B_N = 70,
    AS_TStdKnight_TShieldSword_AttackShield02C_N = 71, AS_TStdKnight_TShieldSword_AttackShield02D_N = 72,

    // ===== 쉴드+소드 가드 회전 애니메이션 (73-76) =====
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_L180 = 73, AS_TStdKnight_TShieldSword_Guard_IdleTurn_L90 = 74, AS_TStdKnight_TShieldSword_Guard_IdleTurn_R180 = 75,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_R90 = 76,

    // ===== 쉴드+소드 가드 기본 애니메이션 (77-79) =====
    AS_TStdKnight_TShieldSword_Guard_N_End = 77, AS_TStdKnight_TShieldSword_Guard_N_Loop = 78, AS_TStdKnight_TShieldSword_Guard_N_Start = 79,

    // ===== 쉴드+소드 가드 이동 애니메이션 (80-90) =====
    AS_TStdKnight_TShieldSword_Guard_Run_F_Loop = 80, AS_TStdKnight_TShieldSword_Guard_Walk_B_Loop = 81, AS_TStdKnight_TShieldSword_Guard_Walk_BL_Loop = 82,
    AS_TStdKnight_TShieldSword_Guard_Walk_BR_LeftFoot_Loop = 83, AS_TStdKnight_TShieldSword_Guard_Walk_BR_RightFoot_Loop = 84, AS_TStdKnight_TShieldSword_Guard_Walk_F_Loop = 85,
    AS_TStdKnight_TShieldSword_Guard_Walk_FL_LeftFoot_Loop = 86, AS_TStdKnight_TShieldSword_Guard_Walk_FL_RightFoot_Loop = 87, AS_TStdKnight_TShieldSword_Guard_Walk_FR_Loop = 88,
    AS_TStdKnight_TShieldSword_Guard_Walk_L_Loop = 89, AS_TStdKnight_TShieldSword_Guard_Walk_R_Loop = 90,

    // ===== 쉴드+소드 가드 히트 애니메이션 (91-94) =====
    AS_TStdKnight_TShieldSword_GuardBreak_N = 91, AS_TStdKnight_TShieldSword_GuardHit01_N = 92, AS_TStdKnight_TShieldSword_GuardHit02_N = 93,
    AS_TStdKnight_TShieldSword_ShieldBlow01_N = 94,

    // ===== 소드 공격 애니메이션 (95-110) =====
    AS_TStdKnight_TSword_AttackJump01_N = 95, AS_TStdKnight_TSword_AttackNormal00_N = 96, AS_TStdKnight_TSword_AttackNormal01_N = 97,
    AS_TStdKnight_TSword_AttackNormal02_N = 98, AS_TStdKnight_TSword_AttackNormal03_N = 99, AS_TStdKnight_TSword_AttackNormal03B_N = 100,
    AS_TStdKnight_TSword_AttackRaiseUp01_N = 101, AS_TStdKnight_TSword_AttackRange01_N = 102, AS_TStdKnight_TSword_AttackSpecial01_N = 103,
    AS_TStdKnight_TSword_AttackSpecial02_N = 104, AS_TStdKnight_TSword_AttackSpecial03_N_End = 105, AS_TStdKnight_TSword_AttackSpecial03_N_Loop = 106,
    AS_TStdKnight_TSword_AttackSpecial03_N_Start = 107, AS_TStdKnight_TSword_AttackStrong01_N = 108, AS_TStdKnight_TSword_AttackStrong02_N = 109,
    AS_TStdKnight_TSword_AttackStrong03_N = 110,

    // ===== 소드 대기 애니메이션 (111) =====
    AS_TStdKnight_TSword_Idle_N_Loop = 111,

    QUEEN_KNIGHT_ANIMATION_END
};

NS_END