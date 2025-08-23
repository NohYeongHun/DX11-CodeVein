#pragma once

NS_BEGIN(Client)

// GiantWhiteDevil 애니메이션 키 정의
// GiantWhiteDevil 애니메이션 키 정의
enum GIANT_WHITE_DEVIL_ANIMATION_KEY : _uint
{
    // ===== 공통 애니메이션 (0-4) =====
    AS_TGiantWhite_TCmn_AKetsugiPointShoot01_N = 0,
    AS_TGiantWhite_TCmn_Dodge_L = 1,
    AS_TGiantWhite_TCmn_Dodge_R = 2,
    AS_TGiantWhite_TCmn_NoneFightIdleCrouch_N = 3,
    AS_TGiantWhite_TCmn_NoneFightIdleCrouch_N_Loop = 4,

    // ===== 할버드 공격 애니메이션 (5-13) =====
    AS_TGiantWhite_THalberd_AttackLeftRotation01_N = 5,
    AS_TGiantWhite_THalberd_AttackNormal01_N = 6,
    AS_TGiantWhite_THalberd_AttackNormal03_N = 7,
    AS_TGiantWhite_THalberd_AttackRightRotation01_N = 8,
    AS_TGiantWhite_THalberd_AttackSmash01_N = 9,
    AS_TGiantWhite_THalberd_AttackSpin01_N = 10,
    AS_TGiantWhite_THalberd_AttackStrong01_N = 11,
    AS_TGiantWhite_THalberd_AttackStrong02_N = 12,
    AS_TGiantWhite_THalberd_AttackStrong03_N = 13,

    // ===== 할버드 데미지 애니메이션 (14-21) =====
    AS_TGiantWhite_THalberd_Damage01_BL = 14,
    AS_TGiantWhite_THalberd_Damage01_BR = 15,
    AS_TGiantWhite_THalberd_Damage01_FL = 16,
    AS_TGiantWhite_THalberd_Damage01_FR = 17,
    AS_TGiantWhite_THalberd_Damage02_BL = 18,
    AS_TGiantWhite_THalberd_Damage02_BR = 19,
    AS_TGiantWhite_THalberd_Damage02_FL = 20,
    AS_TGiantWhite_THalberd_Damage02_FR = 21,

    // ===== 할버드 대기 및 회전 애니메이션 (22-26) =====
    AS_TGiantWhite_THalberd_Idle_N_Loop = 22,
    AS_TGiantWhite_THalberd_IdleTurn_L180 = 23,
    AS_TGiantWhite_THalberd_IdleTurn_L90 = 24,
    AS_TGiantWhite_THalberd_IdleTurn_R180 = 25,
    AS_TGiantWhite_THalberd_IdleTurn_R90 = 26,

    // ===== 할버드 특수 능력 (27) =====
    AS_TGiantWhite_THalberd_KetsugiBuffSelf_N = 27,

    // ===== 할버드 이동 애니메이션 (28-38) =====
    AS_TGiantWhite_THalberd_Run_N_Loop = 28,
    AS_TGiantWhite_THalberd_Walk_B_Loop = 29,
    AS_TGiantWhite_THalberd_Walk_BL_Loop = 30,
    AS_TGiantWhite_THalberd_Walk_BR_LeftFoot_Loop = 31,
    AS_TGiantWhite_THalberd_Walk_BR_RightFoot_Loop = 32,
    AS_TGiantWhite_THalberd_Walk_F_Loop = 33,
    AS_TGiantWhite_THalberd_Walk_FL_LeftFoot_Loop = 34,
    AS_TGiantWhite_THalberd_Walk_FL_RightFoot_Loop = 35,
    AS_TGiantWhite_THalberd_Walk_FR_Loop = 36,
    AS_TGiantWhite_THalberd_Walk_L_Loop = 37,
    AS_TGiantWhite_THalberd_Walk_R_Loop = 38,

    // ===== 공통 스탠다드 솔져 애니메이션 (39-46) =====
    AS_TStdSoldier_TCmn_DamageStrike_P = 39,
    AS_TStdSoldier_TCmn_DamageStrike_S = 40,
    AS_TStdSoldier_TCmn_Death_N = 41,
    AS_TStdSoldier_TCmn_Down_P_End = 42,
    AS_TStdSoldier_TCmn_Down_P_Loop = 43,
    AS_TStdSoldier_TCmn_Down_S_End = 44,
    AS_TStdSoldier_TCmn_Down_S_Loop = 45,
    AS_TStdSoldier_TCmn_Fall_N_ALL = 46,

    GIANT_WHITE_DEVIL_ANIMATION_END
};

NS_END