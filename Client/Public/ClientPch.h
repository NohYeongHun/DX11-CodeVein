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
#include "Loader_GamePlay.h"
#include "Loader.h"
#pragma endregion

#pragma region LEVEL
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#pragma endregion


#pragma region COMPONENT

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
#include "Loading_BackGround.h"
#include "Loading_Panel.h"
#include "Loading_Slot.h"

/* ======================================= Terrain Object =================================== */
#include "Terrain.h"
#include "Map.h"

/* ======================================= SkyBox Object =================================== */
#include "Sky.h"

/* ======================================== Player Object ===================================== */
#include "PlayerDefine.h"
#include "Weapon.h"
#include "Player.h"
#include "PlayerState.h"
#include "Player_IdleState.h"
#include "Player_WalkState.h"
#include "Player_RunState.h"
#include "Player_DodgeState.h"
#include "Player_StrongAttackState.h"
#include "Player_AttackState.h"
#include "Player_GuardState.h"



/* ======================================== BT Object ===================================== */
#include "BT_Monster_IsAlive.h"
#include "BT_Monster_CheckHit.h"
#include "BT_IsTargetInRange.h"

/* ======================================== Monster Object ===================================== */
#include "Monster.h"


/* ======================================== Sky Boss Object ===================================== */
#include "SkyBossDefine.h"
/* ======================================== Sky Boss BT ===================================== */
#include "BT_SkyBoss_IsInAttackRange.h"
#include "BT_SkyBoss_Attack.h"
#include "BT_SkyBoss_Idle.h"

#include "BT_SkyBoss_IsStrongHit.h"

#include "BT_SkyBoss_StrongHitReaction.h"
#include "BT_SkyBoss_NormalHitReaction.h"

#include "BT_SkyBoss_Wait.h"
#include "BT_SkyBoss_CanStrongAttack.h"
#include "SkyBossTree.h"
/* ======================================== Sky Boss ===================================== */
#include "SkyBoss.h"
#pragma endregion


