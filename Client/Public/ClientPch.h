#pragma once
#include "EnginePch.h"
#include "Client_Defines.h"

#pragma region IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Imgui_Define.h"
#include "Imgui_Manager.h"
#pragma endregion

#include "MainApp.h"

#pragma region LOADER
#include "Loader_Logo.h"
#include "Loader_Personal.h"
#include "Loader_StageOne.h"
#include "Loader_GamePlay.h"

#include "Loader_Debug.h"
#include "Loader.h"
#pragma endregion

#pragma region LEVEL
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_StageOne.h"
#include "Level_GamePlay.h"
#include "Level_Debug.h"
#pragma endregion


#pragma region COMPONENT
#include "VIBuffer_PointParticleDir_Instance.h"
#pragma endregion

#pragma region EFFECT

#include "Particle.h"
#include "Snow.h"
#include "EffectParticle.h"
#include "SwordTrail.h"
#pragma endregion



#pragma region GAMEOBJECT

#include "BackGround.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
/* =========================================== Title Object ===================================== */
#include "Title_BackGround.h"
#include "TitleText.h"
#include "Title.h"

/* =========================================== HUD ===================================== */
#include "HUD.h"
#include "Skill_Icon.h"
#include "Skill_Slot.h"
#include "Skill_Panel.h"
#include "HPBar.h"
#include "SteminaBar.h"
#include "StatusPanel.h"
#include "LockOnUI.h"



/* ======================================= Inventory =================================== */
#include "Inventory.h"
#include "Inventory_Panel.h"
#include "InventorySkill_Icon.h"
#include "InventorySkill_Slot.h"
#include "InventoryItem_Icon.h"
#include "InventoryItem_Slot.h"
#include "InventoryStatus_Icon.h"
#include "InventoryStatus_Info.h"


/* ======================================= SKillUI =================================== */
#include "SkillUI.h"
#include "SkillUI_Panel.h"
#include "SkillUI_Slot.h"
#include "SkillUI_Icon.h"


/* ======================================= Loading Object =================================== */
#include "Fade_Out.h"
#include "Loading_BackGround.h"
#include "Loading_Panel.h"
#include "Loading_Slot.h"

/* ======================================= Terrain Object =================================== */
#include "Terrain.h"
#include "Map.h"

/* ======================================= SkyBox Object =================================== */
#include "Sky.h"

/* ======================================= Weapon Object =================================== */
#include "Weapon.h"  // Weapon이 가져야 되는 기능들.

/* ======================================== Player Object ===================================== */
#include "PlayerDefine.h"
#include "PlayerWeapon.h"
#include "Player.h"
#include "PlayerState.h"
#include "Player_IdleState.h"
#include "Player_WalkState.h"
#include "Player_RunState.h"
#include "Player_DodgeState.h"
#include "Player_StrongAttackState.h"
#include "Player_AttackState.h"
#include "Player_GuardState.h"
#include "Player_DamageState.h"
#include "Player_FirstSkillState.h"
#include "Player_SecondSkillState.h"



#pragma region Effect
#include "Slash.h"
#include "HitFlashEffect.h"
#pragma endregion



/* ======================================== BT Object ===================================== */

#pragma region 특수 상태
#include "BT_Monster_IsDead.h"
#include "BT_Monster_DeadAction.h"
#include "BT_Monster_IsDown.h"
#include "BT_Monster_DownAction.h"
#include "BT_Monster_IsHit.h"
#include "BT_Monster_HitAction.h"
#include "BT_Monster_IsEncounterCondition.h"
#include "BT_Monster_EncounterAction.h"
#include "BT_Monster_PrevEncounterAction.h"
#pragma endregion

#include "BT_Monster_IsAttackRange.h"
#include "BT_Monster_AttackAction.h"
#include "BT_Monster_IsDetectRange.h"
#include "BT_Monster_DetectAction.h"
#include "BT_Monster_IdleAction.h"
#include "BT_Monster_IsRotate.h"
#include "BT_Monster_RotateAction.h"

/* ======================================== Monster Object ===================================== */
#include "BossHpBarUI.h"
#include "Monster.h"
#include "MonsterTree.h"
/* ======================================== WolfDevil Object ===================================== */
#include "WolfDevilDefine.h"
#include "WolfWeapon.h"
#include "WolfDevil.h"

/* ======================================== SlaveVampire Object ===================================== */
#include "SlaveVampireDefine.h"
#include "SlaveVampireSword.h"
#include "SlaveVampire.h"



/* ======================================== QueenKnight Boss BT ===================================== */
#include "BT_QueenKnight_FirstPhase_AttackAction.h"
#include "BT_QueenKnight_IsDashAttackCondition.h"
#include "BT_QueenKnight_DashAttackAction.h"
#include "BT_QueenKnight_IsTripleDownStrikeCondition.h"
#include "BT_QueenKnight_TripleDownStrikeAction.h"
#include "BT_QueenKnight_DownStrikeAction.h"



#include "QueenKnightTree.h"
/* ======================================== QueenKnight Boss Object ===================================== */
#include "QueenKnightDefine.h"

#include "KnightShield.h"
#include "KnightLance.h"
#include "QueenKnight.h"

/* ======================================== GiantWhiteDevil Boss BT ===================================== */
#include "BT_GiantWhiteDevil_ComboAttackAction.h"
#include "Giant_WhiteDevilTree.h"

/* ======================================== GiantWhiteDevil Boss Object ===================================== */
#include "Giant_WhiteDevilDefine.h"
#include "WhiteLargeHalberd.h"
#include "Giant_WhiteDevil.h"



/* ======================================== Sky Boss Object ===================================== */
#include "SkyBossDefine.h"
/* ======================================== Sky Boss BT ===================================== */

#include "BT_SkyBoss_IsInAttackRange.h"
#include "BT_SkyBoss_Attack.h"
#include "BT_SkyBoss_Idle.h"
#include "BT_SkyBoss_IsDown.h"


#include "BT_SkyBoss_IsStrongHit.h"

#include "BT_SkyBoss_StrongHitReaction.h"
#include "BT_SkyBoss_NormalHitReaction.h"

#include "BT_SkyBoss_Wait.h"
#include "BT_SkyBoss_CanStrongAttack.h"
#include "SkyBossTree.h"
/* ======================================== Sky Boss ===================================== */
#include "SkyBoss.h"



/* ======================================== DEBUG 용도 ===================================== */


#pragma region Effect 이미지


#include "PrototypeName.h"

#pragma endregion


#ifdef _DEBUG
#include "Client_Debug.h"
#endif
#pragma endregion


