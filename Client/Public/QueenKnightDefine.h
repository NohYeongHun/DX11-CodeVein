#pragma once


#pragma once

NS_BEGIN(Client)

// TStdKnight 애니메이션 키 정의
enum TSTDKNIGHT_ANIMATION_KEY : _uint
{
    // ===== 데미지 애니메이션 (0-14) =====
    AS_TStdKnight_TCmn_Damage01_BL = 0, AS_TStdKnight_TCmn_Damage01_BR = 1, AS_TStdKnight_TCmn_Damage01_FL = 2,
    AS_TStdKnight_TCmn_Damage02_BL = 3, AS_TStdKnight_TCmn_Damage02_BR = 4, AS_TStdKnight_TCmn_Damage02_FL = 5,
    AS_TStdKnight_TCmn_Damage02_FR = 6, AS_TStdKnight_TCmn_Damage03_BL = 7, AS_TStdKnight_TCmn_Damage03_BR = 8,
    AS_TStdKnight_TCmn_Damage03_FL = 9, AS_TStdKnight_TCmn_Damage03_FR = 10, AS_TStdKnight_TCmn_DamageBackStab_N = 11,
    AS_TStdKnight_TCmn_DamageBlow_B = 12, AS_TStdKnight_TCmn_DamageBlow_F = 13, AS_TStdKnight_TCmn_DamageStrike_P = 14,

    // ===== 죽음 애니메이션 (15-16) =====
    AS_TStdKnight_TCmn_Death_F = 15, AS_TStdKnight_TCmn_Death_N = 16,

    // ===== 다운 상태 애니메이션 (17-18) =====
    AS_TStdKnight_TCmn_Down_P_End = 17, AS_TStdKnight_TCmn_Down_P_Loop = 18,

    // ===== 라이트 소드 공격 애니메이션 (19-27) =====
    AS_TStdKnight_TLSword_AttackKick01_N = 19, AS_TStdKnight_TLSword_AttackNormal01_N = 20, AS_TStdKnight_TLSword_AttackNormal02_N = 21,
    AS_TStdKnight_TLSword_AttackNormal03_N = 22, AS_TStdKnight_TLSword_AttackSmash01_N = 23, AS_TStdKnight_TLSword_AttackStrong01_N = 24,
    AS_TStdKnight_TLSword_AttackStrong02_N = 25, AS_TStdKnight_TLSword_AttackStrong03_N = 26, AS_TStdKnight_TLSword_AttackSwing01_N = 27,

    // ===== 라이트 소드 대기 애니메이션 (28) =====
    AS_TStdKnight_TLSword_Idle_N_Loop = 28,

    // ===== 쉴드+라이트소드 가드 애니메이션 (29-30) =====
    AS_TStdKnight_TShieldLSword_Guard_N_End = 29, AS_TStdKnight_TShieldLSword_Guard_N_Start = 30,

    // ===== 쉴드+소드 공격 애니메이션 (31-36) =====
    AS_TStdKnight_TShieldSword_AttackBehindBash01_N = 31, AS_TStdKnight_TShieldSword_AttackShield01_N = 32, AS_TStdKnight_TShieldSword_AttackShield02A_N = 33,
    AS_TStdKnight_TShieldSword_AttackShield02B_N = 34, AS_TStdKnight_TShieldSword_AttackShield02C_N = 35, AS_TStdKnight_TShieldSword_AttackShield02D_N = 36,

    // ===== 쉴드+소드 방어 애니메이션 (37-44) =====
    AS_TStdKnight_TShieldSword_Bash_N = 37, AS_TStdKnight_TShieldSword_Guard_IdleTurn_L180 = 38, AS_TStdKnight_TShieldSword_Guard_IdleTurn_L90 = 39,
    AS_TStdKnight_TShieldSword_Guard_IdleTurn_R180 = 40, AS_TStdKnight_TShieldSword_Guard_IdleTurn_R90 = 41, AS_TStdKnight_TShieldSword_Guard_N_End = 42,
    AS_TStdKnight_TShieldSword_Guard_N_Loop = 43, AS_TStdKnight_TShieldSword_Guard_N_Start = 44,

    // ===== 쉴드+소드 이동 애니메이션 (45-55) =====
    AS_TStdKnight_TShieldSword_Guard_Run_F_Loop = 45, AS_TStdKnight_TShieldSword_Guard_Walk_B_Loop = 46, AS_TStdKnight_TShieldSword_Guard_Walk_BL_Loop = 47,
    AS_TStdKnight_TShieldSword_Guard_Walk_BR_LeftFoot_Loop = 48, AS_TStdKnight_TShieldSword_Guard_Walk_BR_RightFoot_Loop = 49, AS_TStdKnight_TShieldSword_Guard_Walk_F_Loop = 50,
    AS_TStdKnight_TShieldSword_Guard_Walk_FL_LeftFoot_Loop = 51, AS_TStdKnight_TShieldSword_Guard_Walk_FL_RightFoot_Loop = 52, AS_TStdKnight_TShieldSword_Guard_Walk_FR_Loop = 53,
    AS_TStdKnight_TShieldSword_Guard_Walk_L_Loop = 54, AS_TStdKnight_TShieldSword_Guard_Walk_R_Loop = 55,

    // ===== 쉴드+소드 방어 관련 애니메이션 (56-59) =====
    AS_TStdKnight_TShieldSword_GuardBreak_N = 56, AS_TStdKnight_TShieldSword_GuardHit01_N = 57, AS_TStdKnight_TShieldSword_GuardHit02_N = 58,
    AS_TStdKnight_TShieldSword_ShieldBlow01_N = 59,

    // ===== 소드 공격 애니메이션 (60-75) =====
    AS_TStdKnight_TSword_AttackJump01_N = 60, AS_TStdKnight_TSword_AttackNormal00_N = 61, AS_TStdKnight_TSword_AttackNormal01_N = 62,
    AS_TStdKnight_TSword_AttackNormal02_N = 63, AS_TStdKnight_TSword_AttackNormal03_N = 64, AS_TStdKnight_TSword_AttackNormal03B_N = 65,
    AS_TStdKnight_TSword_AttackRaiseUp01_N = 66, AS_TStdKnight_TSword_AttackRange01_N = 67, AS_TStdKnight_TSword_AttackSpecial01_N = 68,
    AS_TStdKnight_TSword_AttackSpecial02_N = 69, AS_TStdKnight_TSword_AttackSpecial03_N_End = 70, AS_TStdKnight_TSword_AttackSpecial03_N_Loop = 71,
    AS_TStdKnight_TSword_AttackSpecial03_N_Start = 72, AS_TStdKnight_TSword_AttackStrong01_N = 73, AS_TStdKnight_TSword_AttackStrong02_N = 74,
    AS_TStdKnight_TSword_AttackStrong03_N = 75,

    // ===== 소드 대기 애니메이션 (76) =====
    AS_TStdKnight_TSword_Idle_N_Loop = 76,

    KNIGHT_ANIMATION_END
};

NS_END