#pragma once
#include "PrototypeName.h"

NS_BEGIN(Tool)

/* 사용할 Prototype Name들을 배열에 미리 저장 */
const MODEL_DESC Model_Prototypes[] =
{
	{ TEXT("MapPart_Circle_Floor"), "../Bin/Resources/Models/Map/BossMap/CircleFloor.fbx", "textures/CircleFloor/" },
	{ TEXT("MapPart_Side_Floor"),   "../Bin/Resources/Models/Map/BossMap/SideFloor.fbx",   "textures/Floor/" },
	{ TEXT("MapPart_Floor"),        "../Bin/Resources/Models/Map/BossMap/Floor.fbx",        "textures/Floor/" }
};

extern unsigned int Model_PrototypeSize = 3;
NS_END