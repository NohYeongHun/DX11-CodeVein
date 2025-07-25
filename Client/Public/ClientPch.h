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



/* ======================================== Actor Object ===================================== */
#include "PlayerState.h"
#include "Player_IdleState.h"
#include "Player_WalkState.h"
#include "Player_RunState.h"
#include "Player.h"

#pragma endregion


