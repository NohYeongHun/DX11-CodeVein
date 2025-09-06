#pragma once
#include "EnginePch.h"
#include "Tool_Defines.h"

#pragma region PARTCILE COMPONENT
#include "VIBuffer_PointParticleDir_Instance.h"
#pragma endregion

#pragma region EFFECT
#include "Tool_EffectMesh.h"
#include "Tool_EffectTexture.h"
#include "Tool_EffectParticle.h"

#include "TestParticle.h"
#pragma endregion




#pragma region TOOL 관련
#include "PrototypeName.h"
#include "Picking_Manager.h"
#include "SaveFile_Define.h"
#include "SaveFile_Loader.h"
#include "NavigationManager.h"
#include "Tool_MainApp.h"
#include "Map_Tool.h"
#pragma endregion



#pragma region IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Imgui_Define.h"
#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"
#pragma endregion



#pragma region LOADER
#include "Loader.h"
#pragma endregion

#pragma region LEVEL
#include "Level_Loading.h"
#include "Level_Logo.h"
#pragma endregion

#pragma region Model
#include "Tool_Bone.h"
#include "Tool_Channel.h"
#include "Tool_Animation.h"
#include "Tool_Mesh.h"
#include "Tool_MeshMaterial.h"
#include "Tool_Model.h"
#pragma endregion


#pragma region GAMEOBJECT
#include "ToolMap_Part.h" // 저장 용
#include "Map_Part.h"	  // Load 용
#include "Map.h"
#include "Camera_Free.h"
#include "Player.h"

#pragma endregion





//static void Transform_Print_Imgui(const char* szName, _float* vPos)
//{
//	ImGui::Begin(szName);
//	ImGui::InputFloat3("pos", vPos);
//	ImGui::End();
//}