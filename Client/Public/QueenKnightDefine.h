
#pragma once

NS_BEGIN(Client)

// QueenKnight 애니메이션 키 정의
enum QUEEN_KNIGHT_ANIMATION_KEY : _uint
{
    // ===== 데미지 애니메이션 (0-14) =====
    AS_TStdKnight_TCmn_Damage01_BL = 0,
    AS_TStdKnight_TCmn_Damage01_BR = 1,
    AS_TStdKnight_TCmn_Damage01_FL = 2,
    AS_TStdKnight_TCmn_Damage02_BL = 3,
    AS_TStdKnight_TCmn_Damage02_BR = 4,
    AS_TStdKnight_TCmn_Damage02_FL = 5,
    AS_TStdKnight_TCmn_Damage02_FR = 6,
    AS_TStdKnight_TCmn_Damage03_BL = 7,
    AS_TStdKnight_TCmn_Damage03_BR = 8,
    AS_TStdKnight_TCmn_Damage03_FL = 9,
    AS_TStdKnight_TCmn_Damage03_FR = 10,
    AS_TStdKnight_TCmn_DamageBackStab_N = 11,
    AS_TStdKnight_TCmn_DamageBlow_B = 12,
    AS_TStdKnight_TCmn_DamageBlow_F = 13,
    AS_TStdKnight_TCmn_DamageStrike_P = 14,

    // ===== 죽음 애니메이션 (15-16) =====
    AS_TStdKnight_TCmn_Death_F = 15,
    AS_TStdKnight_TCmn_Death_N = 16,

    // ===== 회피 애니메이션 (17) =====
    AS_TStdKnight_TCmn_Dodge_B = 17,

    // ===== 다운 상태 애니메이션 (18-19) =====
    AS_TStdKnight_TCmn_Down_P_End = 18,
    AS_TStdKnight_TCmn_Down_P_Loop = 19,

    // ===== 낙하 애니메이션 (20-21) =====
    AS_TStdKnight_TCmn_Fall_N_End = 20,
    AS_TStdKnight_TCmn_Fall_N_Loop = 21,

    // ===== 전투 외 대기 애니메이션 (22-23) =====
    AS_TStdKnight_TCmn_NoneFightIdleCrouch_N = 22,
    AS_TStdKnight_TCmn_NoneFightIdleCrouch_N_Loop = 23,

    // ===== 라이트 소드 공격 애니메이션 (24-32) =====
    AS_TStdKnight_TLSword_AttackKick01_N = 24,
    AS_TStdKnight_TLSword_AttackNormal01_N = 25,
    AS_TStdKnight_TLSword_AttackNormal02_N = 26,
    AS_TStdKnight_TLSword_AttackNormal03_N = 27,
    AS_TStdKnight_TLSword_AttackSmash01_N = 28,
    AS_TStdKnight_TLSword_AttackStrong01_N = 29,
    AS_TStdKnight_TLSword_AttackStrong02_N = 30,
    AS_TStdKnight_TLSword_AttackStrong03_N = 31,
    AS_TStdKnight_TLSword_AttackSwing01_N = 32,

    // ===== 라이트 소드 대기 애니메이션 (33) =====
    AS_TStdKnight_TLSword_Idle_N_Loop = 33,

    // ===== 쉴드+라이트소드 가드 애니메이션 (34-35) =====
    AS_TStdKnight_TShieldLSword_Guard_N_End = 34,
    AS_TStdKnight_TShieldLSword_Guard_N_Start = 35,

    // ===== 쉴드+소드 공격 애니메이션 (36-41) =====
    AS_TStdKnight_TShieldSword_AttackBehindBash01_N = 36,
    AS_TStdKnight_TShieldSword_AttackShield01_N = 37,
    AS_TStdKnight_TShieldSword_AttackShield02A_N = 38,
    AS_TStdKnight_TShieldSword_AttackShield02B_N = 39,
    AS_TStdKnight_TShieldSword_AttackShield02C_N = 40,
    AS_TStdKnight_TShieldSword_AttackShield02D_N = 41,

    // ===== 쉴드+소드 방어 애니메이션 (42-49) =====
    AS_TStdKnight_TShieldSword_Bash_N = 42,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_L180 = 43,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_L90 = 44,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_R180 = 45,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_R90 = 46,
    AS_TStdKnight_TShieldSword_Guard_N_End = 47,
    AS_TStdKnight_TShieldSword_Guard_N_Loop = 48,
    AS_TStdKnight_TShieldSword_Guard_N_Start = 49,

    // ===== 쉴드+소드 이동 애니메이션 (50-60) =====
    AS_TStdKnight_TShieldSword_Guard_Run_F_Loop = 50,
    AS_TStdKnight_TShieldSword_Guard_Walk_B_Loop = 51,
    AS_TStdKnight_TShieldSword_Guard_Walk_BL_Loop = 52,
    AS_TStdKnight_TShieldSword_Guard_Walk_BR_LeftFoot_Loop = 53,
    AS_TStdKnight_TShieldSword_Guard_Walk_BR_RightFoot_Loop = 54,
    AS_TStdKnight_TShieldSword_Guard_Walk_F_Loop = 55,
    AS_TStdKnight_TShieldSword_Guard_Walk_FL_LeftFoot_Loop = 56,
    AS_TStdKnight_TShieldSword_Guard_Walk_FL_RightFoot_Loop = 57,
    AS_TStdKnight_TShieldSword_Guard_Walk_FR_Loop = 58,
    AS_TStdKnight_TShieldSword_Guard_Walk_L_Loop = 59,
    AS_TStdKnight_TShieldSword_Guard_Walk_R_Loop = 60,

    // ===== 쉴드+소드 방어 관련 애니메이션 (61-64) =====
    AS_TStdKnight_TShieldSword_GuardBreak_N = 61,
    AS_TStdKnight_TShieldSword_GuardHit01_N = 62,
    AS_TStdKnight_TShieldSword_GuardHit02_N = 63,
    AS_TStdKnight_TShieldSword_ShieldBlow01_N = 64,

    // ===== 소드 공격 애니메이션 (65-80) =====
    AS_TStdKnight_TSword_AttackJump01_N = 65,
    AS_TStdKnight_TSword_AttackNormal00_N = 66,
    AS_TStdKnight_TSword_AttackNormal01_N = 67,
    AS_TStdKnight_TSword_AttackNormal02_N = 68,
    AS_TStdKnight_TSword_AttackNormal03_N = 69,
    AS_TStdKnight_TSword_AttackNormal03B_N = 70,
    AS_TStdKnight_TSword_AttackRaiseUp01_N = 71,
    AS_TStdKnight_TSword_AttackRange01_N = 72,
    AS_TStdKnight_TSword_AttackSpecial01_N = 73,
    AS_TStdKnight_TSword_AttackSpecial02_N = 74,
    AS_TStdKnight_TSword_AttackSpecial03_N_End = 75,
    AS_TStdKnight_TSword_AttackSpecial03_N_Loop = 76,
    AS_TStdKnight_TSword_AttackSpecial03_N_Start = 77,
    AS_TStdKnight_TSword_AttackStrong01_N = 78,
    AS_TStdKnight_TSword_AttackStrong02_N = 79,
    AS_TStdKnight_TSword_AttackStrong03_N = 80,

    // ===== 소드 대기 애니메이션 (81) =====
    AS_TStdKnight_TSword_Idle_N_Loop = 81,

    QUEEN_KNIGHT_ANIMATION_END
};

NS_END

//NS_BEGIN(Client)
//
//// TStdKnight 애니메이션 키 정의 (수정된 버전)
//enum TSTDKNIGHT_ANIMATION_KEY : _uint
//{
//    // ===== 데미지 애니메이션 (0-14) =====
//    AS_TStdKnight_TCmn_Damage01_BL = 0, AS_TStdKnight_TCmn_Damage01_BR = 1, AS_TStdKnight_TCmn_Damage01_FL = 2,
//    AS_TStdKnight_TCmn_Damage02_BL = 3, AS_TStdKnight_TCmn_Damage02_BR = 4, AS_TStdKnight_TCmn_Damage02_FL = 5,
//    AS_TStdKnight_TCmn_Damage02_FR = 6, AS_TStdKnight_TCmn_Damage03_BL = 7, AS_TStdKnight_TCmn_Damage03_BR = 8,
//    AS_TStdKnight_TCmn_Damage03_FL = 9, AS_TStdKnight_TCmn_Damage03_FR = 10, AS_TStdKnight_TCmn_DamageBackStab_N = 11,
//    AS_TStdKnight_TCmn_DamageBlow_B = 12, AS_TStdKnight_TCmn_DamageBlow_F = 13, AS_TStdKnight_TCmn_DamageStrike_P = 14,
//
//    // ===== 죽음 애니메이션 (15-16) =====
//    AS_TStdKnight_TCmn_Death_F = 15, AS_TStdKnight_TCmn_Death_N = 16,
//
//    // ===== 회피 애니메이션 (17) ===== 
//    AS_TStdKnight_TCmn_Dodge_B = 17,
//
//    // ===== 다운 상태 애니메이션 (18-19) =====
//    AS_TStdKnight_TCmn_Down_P_End = 18, AS_TStdKnight_TCmn_Down_P_Loop = 19,
//
//    // ===== 낙하 애니메이션 (20-21) =====
//    AS_TStdKnight_TCmn_Fall_N_End = 20, AS_TStdKnight_TCmn_Fall_N_Loop = 21,
//
//    // ===== 라이트 소드 공격 애니메이션 (22-30) =====
//    AS_TStdKnight_TLSword_AttackKick01_N = 22, AS_TStdKnight_TLSword_AttackNormal01_N = 23, AS_TStdKnight_TLSword_AttackNormal02_N = 24,
//    AS_TStdKnight_TLSword_AttackNormal03_N = 25, AS_TStdKnight_TLSword_AttackSmash01_N = 26, AS_TStdKnight_TLSword_AttackStrong01_N = 27,
//    AS_TStdKnight_TLSword_AttackStrong02_N = 28, AS_TStdKnight_TLSword_AttackStrong03_N = 29, AS_TStdKnight_TLSword_AttackSwing01_N = 30,
//
//    // ===== 라이트 소드 대기 애니메이션 (31) =====
//    AS_TStdKnight_TLSword_Idle_N_Loop = 31,
//
//    // ===== 쉴드+라이트소드 가드 애니메이션 (32-33) =====
//    AS_TStdKnight_TShieldLSword_Guard_N_End = 32, AS_TStdKnight_TShieldLSword_Guard_N_Start = 33,
//
//    // ===== 쉴드+소드 공격 애니메이션 (34-39) =====
//    AS_TStdKnight_TShieldSword_AttackBehindBash01_N = 34, AS_TStdKnight_TShieldSword_AttackShield01_N = 35, AS_TStdKnight_TShieldSword_AttackShield02A_N = 36,
//    AS_TStdKnight_TShieldSword_AttackShield02B_N = 37, AS_TStdKnight_TShieldSword_AttackShield02C_N = 38, AS_TStdKnight_TShieldSword_AttackShield02D_N = 39,
//
//    // ===== 쉴드+소드 방어 애니메이션 (40-47) =====
//    AS_TStdKnight_TShieldSword_Bash_N = 40, AS_TStdKnight_TShieldSword_Guard_IdleTurn_L180 = 41, AS_TStdKnight_TShieldSword_Guard_IdleTurn_L90 = 42,
//    AS_TStdKnight_TShieldSword_Guard_IdleTurn_R180 = 43, AS_TStdKnight_TShieldSword_Guard_IdleTurn_R90 = 44, AS_TStdKnight_TShieldSword_Guard_N_End = 45,
//    AS_TStdKnight_TShieldSword_Guard_N_Loop = 46, AS_TStdKnight_TShieldSword_Guard_N_Start = 47,
//
//    // ===== 쉴드+소드 이동 애니메이션 (48-58) =====
//    AS_TStdKnight_TShieldSword_Guard_Run_F_Loop = 48, AS_TStdKnight_TShieldSword_Guard_Walk_B_Loop = 49, AS_TStdKnight_TShieldSword_Guard_Walk_BL_Loop = 50,
//    AS_TStdKnight_TShieldSword_Guard_Walk_BR_LeftFoot_Loop = 51, AS_TStdKnight_TShieldSword_Guard_Walk_BR_RightFoot_Loop = 52, AS_TStdKnight_TShieldSword_Guard_Walk_F_Loop = 53,
//    AS_TStdKnight_TShieldSword_Guard_Walk_FL_LeftFoot_Loop = 54, AS_TStdKnight_TShieldSword_Guard_Walk_FL_RightFoot_Loop = 55, AS_TStdKnight_TShieldSword_Guard_Walk_FR_Loop = 56,
//    AS_TStdKnight_TShieldSword_Guard_Walk_L_Loop = 57, AS_TStdKnight_TShieldSword_Guard_Walk_R_Loop = 58,
//
//    // ===== 쉴드+소드 방어 관련 애니메이션 (59-62) =====
//    AS_TStdKnight_TShieldSword_GuardBreak_N = 59, AS_TStdKnight_TShieldSword_GuardHit01_N = 60, AS_TStdKnight_TShieldSword_GuardHit02_N = 61,
//    AS_TStdKnight_TShieldSword_ShieldBlow01_N = 62,
//
//    // ===== 소드 공격 애니메이션 (63-78) =====
//    AS_TStdKnight_TSword_AttackJump01_N = 63, AS_TStdKnight_TSword_AttackNormal00_N = 64, AS_TStdKnight_TSword_AttackNormal01_N = 65,
//    AS_TStdKnight_TSword_AttackNormal02_N = 66, AS_TStdKnight_TSword_AttackNormal03_N = 67, AS_TStdKnight_TSword_AttackNormal03B_N = 68,
//    AS_TStdKnight_TSword_AttackRaiseUp01_N = 69, AS_TStdKnight_TSword_AttackRange01_N = 70, AS_TStdKnight_TSword_AttackSpecial01_N = 71,
//    AS_TStdKnight_TSword_AttackSpecial02_N = 72, AS_TStdKnight_TSword_AttackSpecial03_N_End = 73, AS_TStdKnight_TSword_AttackSpecial03_N_Loop = 74,
//    AS_TStdKnight_TSword_AttackSpecial03_N_Start = 75, AS_TStdKnight_TSword_AttackStrong01_N = 76, AS_TStdKnight_TSword_AttackStrong02_N = 77,
//    AS_TStdKnight_TSword_AttackStrong03_N = 78,
//
//    // ===== 소드 대기 애니메이션 (79) =====
//    AS_TStdKnight_TSword_Idle_N_Loop = 79,
//
//    KNIGHT_ANIMATION_END
//};
//
//NS_END