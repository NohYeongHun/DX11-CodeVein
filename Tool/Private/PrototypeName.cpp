#pragma once
#include "PrototypeName.h"

NS_BEGIN(Tool)
/* 사용할 Prototype Name들을 배열에 미리 저장 */
const _tchar* Model_PrototypeNames[MAX_PATH] =
{
	TEXT("MapPart_Circle_Floor"),
	TEXT("MapPart_Side_Floor"),
	TEXT("MapPart_Floor")
};

/* 사용할 Prototype Name 모델 Path 저장. */
const _char* Model_PrototypePath[MAX_PATH] =
{
	"../Bin/Resources/Models/Map/BossMap/CircleFloor.fbx",
	"../Bin/Resources/Models/Map/BossMap/SideFloor.fbx",
	"../Bin/Resources/Models/Map/BossMap/Floor.fbx"
};

extern unsigned int Model_PrototypeSize = 3;
NS_END