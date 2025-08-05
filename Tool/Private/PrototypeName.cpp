#pragma once
#include "PrototypeName.h"

NS_BEGIN(Tool)

/* 사용할 Prototype Name들을 배열에 미리 저장 */
const MODEL_DESC Model_Prototypes[] =
{
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Circle_Floor"), "../Bin/Resources/Models/Map/BossMap/CircleFloor.fbx", "textures/CircleFloor/"},
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Side_Floor"),   "../Bin/Resources/Models/Map/BossMap/SideFloor.fbx",   "textures/Floor/" },
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Floor"),        "../Bin/Resources/Models/Map/BossMap/Floor.fbx",        "textures/Floor/" },
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_BossStage"),        "../Bin/Resources/Models/Map/BossMap/BossMap.fbx",        "textures/BossStage/" },
	{ MODELTYPE::NONANIM,  TEXT("MapPart_BossStageType1"),        "../Bin/Resources/Models/Map/BossMap/BossMapType1.fbx",        "textures/BossStage/" },
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_Player"), "../Bin/Resources/Models/Player/Player.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_SkyBoss"), "../Bin/Resources/Models/SkyBoss/SkyBoss.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Sword"), "../Bin/Resources/Models/Weapon/Player/Sword.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_WolfDevil"), "../Bin/Resources/Models/WolfDevil/WolfDevil.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_BlackKnight"), "../Bin/Resources/Models/BlackKnight/BlackKnight.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_GodChildLance"), "../Bin/Resources/Models/Weapon/GodsChildServant/GodChildLance.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_GodChildShield"), "../Bin/Resources/Models/Weapon/GodsChildServant/GodChildShield.fbx",        "textures/"}	
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_QueenKnight"), "../Bin/Resources/Models/QueenKnight/QueenKnight.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_Fiona"), "../Bin/Resources/Models/Fiona/Fiona.fbx",        "/" }
};

extern unsigned int Model_PrototypeSize = 1;
NS_END