#pragma once
/*
애니메이션 리스트:
0. AS_TSkgKing_T2Sword_AttackCombo01_N
1. AS_TSkgKing_T2Sword_AttackNormal01_N
2. AS_TSkgKing_T2Sword_AttackNormal03_N
3. AS_TSkgKing_T2Sword_AttackNormal05_N
4. AS_TSkgKing_T2Sword_AttackParry_N_End
5. AS_TSkgKing_T2Sword_AttackParry_N_Loop
6. AS_TSkgKing_T2Sword_AttackParry_N_Start
7. AS_TSkgKing_T2Sword_AttackParryAttack_N
8. AS_TSkgKing_T2Sword_AttackSpawn_N
9. AS_TSkgKing_T2Sword_Deformation_N
10. AS_TSkgKing_T2Sword_Idle_N_Loop
11. AS_TSkgKing_T2Sword_Idle_N_Pose
12. AS_TSkgKing_TAxe_AttackJump01_N
13. AS_TSkgKing_TAxe_AttackKetsugi_N
14. AS_TSkgKing_TAxe_AttackNormal01_N
15. AS_TSkgKing_TAxe_AttackNormal02_N
16. AS_TSkgKing_TAxe_AttackNormal03_N
17. AS_TSkgKing_TAxe_AttackNormal04_N
18. AS_TSkgKing_TAxe_Damage_BL
19. AS_TSkgKing_TAxe_Damage_BL1
20. AS_TSkgKing_TAxe_Damage_BR
21. AS_TSkgKing_TAxe_Damage_FL
22. AS_TSkgKing_TAxe_Damage_FR
23. AS_TSkgKing_TAxe_Death_N
24. AS_TSkgKing_TAxe_Down_N_End
25. AS_TSkgKing_TAxe_Down_N_Loop
26. AS_TSkgKing_TAxe_Down_N_Start
27. AS_TSkgKing_TAxe_Idle_N_Loop
28. AS_TSkgKing_TAxe_StatusAilment_Stun_N
29. AS_TSkgKing_TCmn_Damage_BL
30. AS_TSkgKing_TCmn_Damage_BR
31. AS_TSkgKing_TCmn_Damage_FL
32. AS_TSkgKing_TCmn_Damage_FR
33. AS_TSkgKing_TCmn_Dodge_B
34. AS_TSkgKing_TCmn_Dodge_L
35. AS_TSkgKing_TCmn_Dodge_R
36. AS_TSkgKing_TCmn_Down_N_End
37. AS_TSkgKing_TCmn_Down_N_Loop
38. AS_TSkgKing_TCmn_Down_N_Start
39. AS_TSkgKing_TCmn_IdleTurn_L180
40. AS_TSkgKing_TCmn_IdleTurn_L90
41. AS_TSkgKing_TCmn_IdleTurn_R180
42. AS_TSkgKing_TCmn_IdleTurn_R90
43. AS_TSkgKing_TCmn_StatusAilment_Stun_N
44. AS_TSkgKing_TCmn_Walk_B_Loop
45. AS_TSkgKing_TCmn_Walk_BL_Loop
46. AS_TSkgKing_TCmn_Walk_BR_Loop
47. AS_TSkgKing_TCmn_Walk_F_Loop
48. AS_TSkgKing_TCmn_Walk_FL_Loop
49. AS_TSkgKing_TCmn_Walk_FR_Loop
50. AS_TSkgKing_TCmn_Walk_L_Loop
51. AS_TSkgKing_TCmn_Walk_R_Loop
*/

NS_BEGIN(Client)
enum SKYBOSS_ANIMATION_KEY
{
    // ========== SWORD 계열 애니메이션 (0-11) ==========
    // 검 공격 애니메이션
    SKYBOSS_ANIM_SWORD_COMBO_ATTACK = 0,          // 콤보 공격
    SKYBOSS_ANIM_SWORD_NORMAL_ATTACK1 = 1,        // 일반 공격 1
    SKYBOSS_ANIM_SWORD_NORMAL_ATTACK2 = 2,        // 일반 공격 2  
    SKYBOSS_ANIM_SWORD_NORMAL_ATTACK3 = 3,        // 일반 공격 3

    // 검 패리 애니메이션
    SKYBOSS_ANIM_SWORD_PARRY_END = 4,             // 패리 종료
    SKYBOSS_ANIM_SWORD_PARRY_LOOP = 5,            // 패리 루프
    SKYBOSS_ANIM_SWORD_PARRY_START = 6,           // 패리 시작
    SKYBOSS_ANIM_SWORD_PARRY_ATTACK = 7,          // 패리 반격

    // 검 특수 애니메이션
    SKYBOSS_ANIM_SWORD_SPAWN_ATTACK = 8,          // 소환 공격
    SKYBOSS_ANIM_SWORD_DEFORMATION = 9,           // 변형
    SKYBOSS_ANIM_SWORD_IDLE_LOOP = 10,            // 대기 루프
    SKYBOSS_ANIM_SWORD_IDLE_POSE = 11,            // 대기 포즈

    // ========== AXE 계열 애니메이션 (12-28) ==========
    // 도끼 공격 애니메이션
    SKYBOSS_ANIM_AXE_JUMP_ATTACK = 12,            // 점프 공격 (강공격)
    SKYBOSS_ANIM_AXE_KETSUGI_ATTACK = 13,         // 결기 공격 (강공격)
    SKYBOSS_ANIM_AXE_NORMAL_ATTACK1 = 14,         // 일반 공격 1
    SKYBOSS_ANIM_AXE_NORMAL_ATTACK2 = 15,         // 일반 공격 2
    SKYBOSS_ANIM_AXE_NORMAL_ATTACK3 = 16,         // 일반 공격 3
    SKYBOSS_ANIM_AXE_NORMAL_ATTACK4 = 17,         // 일반 공격 4

    // 도끼 피격 애니메이션
    SKYBOSS_ANIM_AXE_DAMAGE_BL = 18,              // 뒤왼쪽 피격
    SKYBOSS_ANIM_AXE_DAMAGE_BL_HEAVY = 19,        // 뒤왼쪽 강한 피격
    SKYBOSS_ANIM_AXE_DAMAGE_BR = 20,              // 뒤오른쪽 피격
    SKYBOSS_ANIM_AXE_DAMAGE_FL = 21,              // 앞왼쪽 피격
    SKYBOSS_ANIM_AXE_DAMAGE_FR = 22,              // 앞오른쪽 피격

    // 도끼 상태 애니메이션
    SKYBOSS_ANIM_AXE_DEATH = 23,                  // 죽음
    SKYBOSS_ANIM_AXE_DOWN_END = 24,               // 다운 종료
    SKYBOSS_ANIM_AXE_DOWN_LOOP = 25,              // 다운 루프
    SKYBOSS_ANIM_AXE_DOWN_START = 26,             // 다운 시작
    SKYBOSS_ANIM_AXE_IDLE = 27,                   // 대기
    SKYBOSS_ANIM_AXE_STUN = 28,                   // 스턴

    // ========== COMMON 계열 애니메이션 (29-51) ==========
    // 공통 피격 애니메이션
    SKYBOSS_ANIM_DAMAGE_BL = 29,                  // 뒤왼쪽 피격
    SKYBOSS_ANIM_DAMAGE_BR = 30,                  // 뒤오른쪽 피격
    SKYBOSS_ANIM_DAMAGE_FL = 31,                  // 앞왼쪽 피격
    SKYBOSS_ANIM_DAMAGE_FR = 32,                  // 앞오른쪽 피격

    // 공통 회피 애니메이션
    SKYBOSS_ANIM_DODGE_B = 33,                    // 뒤로 회피
    SKYBOSS_ANIM_DODGE_L = 34,                    // 왼쪽 회피
    SKYBOSS_ANIM_DODGE_R = 35,                    // 오른쪽 회피

    // 공통 다운 애니메이션
    SKYBOSS_ANIM_DOWN_END = 36,                   // 다운 종료
    SKYBOSS_ANIM_DOWN_LOOP = 37,                  // 다운 루프
    SKYBOSS_ANIM_DOWN_START = 38,                 // 다운 시작

    // 공통 회전 애니메이션
    SKYBOSS_ANIM_TURN_L180 = 39,                  // 왼쪽 180도 회전
    SKYBOSS_ANIM_TURN_L90 = 40,                   // 왼쪽 90도 회전
    SKYBOSS_ANIM_TURN_R180 = 41,                  // 오른쪽 180도 회전
    SKYBOSS_ANIM_TURN_R90 = 42,                   // 오른쪽 90도 회전

    // 공통 상태이상 애니메이션
    SKYBOSS_ANIM_STUN = 43,                       // 스턴

    // 공통 이동 애니메이션
    SKYBOSS_ANIM_WALK_B = 44,                     // 뒤로 걷기
    SKYBOSS_ANIM_WALK_BL = 45,                    // 뒤왼쪽 걷기
    SKYBOSS_ANIM_WALK_BR = 46,                    // 뒤오른쪽 걷기
    SKYBOSS_ANIM_WALK_F = 47,                     // 앞으로 걷기
    SKYBOSS_ANIM_WALK_FL = 48,                    // 앞왼쪽 걷기
    SKYBOSS_ANIM_WALK_FR = 49,                    // 앞오른쪽 걷기
    SKYBOSS_ANIM_WALK_L = 50,                     // 왼쪽 걷기
    SKYBOSS_ANIM_WALK_R = 51,                     // 오른쪽 걷기

    // 애니메이션 개수
    SKYBOSS_ANIM_COUNT = 52
};

// ========== 애니메이션 그룹 별칭 (편의성을 위해) ==========

// 🎯 BT 노드에서 사용할 주요 애니메이션들
namespace SkyBossAnims
{
    // 대기 애니메이션
    constexpr SKYBOSS_ANIMATION_KEY IDLE = SKYBOSS_ANIM_AXE_IDLE;
    constexpr SKYBOSS_ANIMATION_KEY IDLE_SWORD = SKYBOSS_ANIM_SWORD_IDLE_LOOP;

    // 일반 공격 애니메이션 배열
    constexpr SKYBOSS_ANIMATION_KEY NORMAL_ATTACKS[] = {
        SKYBOSS_ANIM_SWORD_NORMAL_ATTACK1,
        SKYBOSS_ANIM_SWORD_NORMAL_ATTACK2,
        SKYBOSS_ANIM_SWORD_NORMAL_ATTACK3
    };

    // 강공격 애니메이션 배열
    constexpr SKYBOSS_ANIMATION_KEY STRONG_ATTACKS[] = {
        SKYBOSS_ANIM_AXE_JUMP_ATTACK,
        SKYBOSS_ANIM_AXE_KETSUGI_ATTACK
    };

    // 피격 애니메이션 배열 (방향별)
    constexpr SKYBOSS_ANIMATION_KEY HIT_ANIMATIONS[] = {
        SKYBOSS_ANIM_DAMAGE_FL,  // 앞왼쪽
        SKYBOSS_ANIM_DAMAGE_FR,  // 앞오른쪽
        SKYBOSS_ANIM_DAMAGE_BL,  // 뒤왼쪽
        SKYBOSS_ANIM_DAMAGE_BR   // 뒤오른쪽
    };

    // 이동 애니메이션 (방향별)
    constexpr SKYBOSS_ANIMATION_KEY WALK_ANIMATIONS[] = {
        SKYBOSS_ANIM_WALK_F,   // 앞
        SKYBOSS_ANIM_WALK_FR,  // 앞오른쪽
        SKYBOSS_ANIM_WALK_R,   // 오른쪽
        SKYBOSS_ANIM_WALK_BR,  // 뒤오른쪽
        SKYBOSS_ANIM_WALK_B,   // 뒤
        SKYBOSS_ANIM_WALK_BL,  // 뒤왼쪽
        SKYBOSS_ANIM_WALK_L,   // 왼쪽
        SKYBOSS_ANIM_WALK_FL   // 앞왼쪽
    };

    // 특수 상태 애니메이션
    constexpr SKYBOSS_ANIMATION_KEY DEATH = SKYBOSS_ANIM_AXE_DEATH;
    constexpr SKYBOSS_ANIMATION_KEY STUN = SKYBOSS_ANIM_STUN;
    constexpr SKYBOSS_ANIMATION_KEY DOWN_START = SKYBOSS_ANIM_DOWN_START;
    constexpr SKYBOSS_ANIMATION_KEY DOWN_LOOP = SKYBOSS_ANIM_DOWN_LOOP;
    constexpr SKYBOSS_ANIMATION_KEY DOWN_END = SKYBOSS_ANIM_DOWN_END;
}

// ========== 유틸리티 함수들 ==========

// 방향에 따른 걷기 애니메이션 선택
inline SKYBOSS_ANIMATION_KEY GetWalkAnimationByDirection(MONSTERDIR eDir)
{
    switch (eDir)
    {
    case MONSTERDIR::FRONT:       return SKYBOSS_ANIM_WALK_F;
    case MONSTERDIR::FRONT_RIGHT: return SKYBOSS_ANIM_WALK_FR;
    case MONSTERDIR::RIGHT:       return SKYBOSS_ANIM_WALK_R;
    case MONSTERDIR::BACK_RIGHT:  return SKYBOSS_ANIM_WALK_BR;
    case MONSTERDIR::BACK:        return SKYBOSS_ANIM_WALK_B;
    case MONSTERDIR::BACK_LEFT:   return SKYBOSS_ANIM_WALK_BL;
    case MONSTERDIR::LEFT:        return SKYBOSS_ANIM_WALK_L;
    case MONSTERDIR::FRONT_LEFT:  return SKYBOSS_ANIM_WALK_FL;
    default:                      return SKYBOSS_ANIM_WALK_F;
    }
}

// 방향에 따른 피격 애니메이션 선택
inline SKYBOSS_ANIMATION_KEY GetHitAnimationByDirection(MONSTERDIR eHitDirection)
{
    switch (eHitDirection)
    {
    case MONSTERDIR::FRONT:
    case MONSTERDIR::FRONT_LEFT:  return SKYBOSS_ANIM_DAMAGE_FL;
    case MONSTERDIR::FRONT_RIGHT: return SKYBOSS_ANIM_DAMAGE_FR;
    case MONSTERDIR::BACK:
    case MONSTERDIR::BACK_LEFT:   return SKYBOSS_ANIM_DAMAGE_BL;
    case MONSTERDIR::BACK_RIGHT:  return SKYBOSS_ANIM_DAMAGE_BR;
    case MONSTERDIR::LEFT:        return SKYBOSS_ANIM_DAMAGE_FL;
    case MONSTERDIR::RIGHT:       return SKYBOSS_ANIM_DAMAGE_FR;
    default:                      return SKYBOSS_ANIM_DAMAGE_FL;
    }
}

// 랜덤 일반 공격 애니메이션 선택
inline SKYBOSS_ANIMATION_KEY GetRandomNormalAttack()
{
    int index = rand() % (sizeof(SkyBossAnims::NORMAL_ATTACKS) / sizeof(SKYBOSS_ANIMATION_KEY));
    return SkyBossAnims::NORMAL_ATTACKS[index];
}

// 랜덤 강공격 애니메이션 선택
inline SKYBOSS_ANIMATION_KEY GetRandomStrongAttack()
{
    int index = rand() % (sizeof(SkyBossAnims::STRONG_ATTACKS) / sizeof(SKYBOSS_ANIMATION_KEY));
    return SkyBossAnims::STRONG_ATTACKS[index];
}

NS_END