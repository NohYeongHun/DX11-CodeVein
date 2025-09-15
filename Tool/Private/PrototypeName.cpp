#pragma once

NS_BEGIN(Tool)

/* 사용할 Prototype Name들을 배열에 미리 저장 */
const MODEL_DESC Model_Prototypes[] =
{
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Circle_Floor"), "../Bin/Resources/Models/Map/BossMap/CircleFloor.fbx", "textures/CircleFloor/"},
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Side_Floor"),   "../Bin/Resources/Models/Map/BossMap/SideFloor.fbx",   "textures/Floor/" },
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_Floor"),        "../Bin/Resources/Models/Map/BossMap/Floor.fbx",        "textures/Floor/" },
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_BossStage"),        "../Bin/Resources/Models/Map/BossMap/BossMap.fbx",        "textures/BossStage/" },
	//{ MODELTYPE::NONANIM,  TEXT("MapPart_BossStageType1"),        "../Bin/Resources/Models/Map/BossMap/BossMapType1.fbx",        "textures/BossStage/" },
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_Player"), "../Bin/Resources/Models/Player/Player.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_SkyBoss"), "../Bin/Resources/Models/SkyBoss/SkyBoss.fbx",        "textures/"},

	
	

	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Sword"), "../Bin/Resources/Models/Weapon/Player/Sword.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodPillar"), "../Bin/Resources/Models/EffectMesh/BloodPillar.fbx",        "textures/"},
	// 
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodPillarA"), "../Bin/Resources/Models/EffectMesh/BloodPillarA.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodPillarB"), "../Bin/Resources/Models/EffectMesh/BloodPillarB.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodPillarC"), "../Bin/Resources/Models/EffectMesh/BloodPillarC.fbx",        "textures/"},
	// 
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodA"), "../Bin/Resources/Models/EffectMesh/BloodA.fbx",        "textures/"},
	{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodB"), "../Bin/Resources/Models/EffectMesh/BloodB.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_BloodC"), "../Bin/Resources/Models/EffectMesh/BloodC.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_SprialWind"), "../Bin/Resources/Models/EffectMesh/SpiralWind.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_Sprial"), "../Bin/Resources/Models/EffectMesh/Spiral.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_Sylinder"), "../Bin/Resources/Models/EffectMesh/Sylinder.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_WindSpiral"), "../Bin/Resources/Models/EffectMesh/WinSpiral.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_Effect_Wind"), "../Bin/Resources/Models/EffectMesh/EffectWind.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_WindEffect"), "../Bin/Resources/Models/EffectMesh/WindEffect.fbx",        "textures/"},
	
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_WolfDevil"), "../Bin/Resources/Models/WolfDevil/WolfDevil.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_BlackKnight"), "../Bin/Resources/Models/BlackKnight/BlackKnight.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_GodChildLance"), "../Bin/Resources/Models/Weapon/GodsChildServant/GodChildLance.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_GodChildShield"), "../Bin/Resources/Models/Weapon/GodsChildServant/GodChildShield.fbx",        "textures/"}	
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_QueenKnight"), "../Bin/Resources/Models/QueenKnight/QueenKnight.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_SlaveVampire"), "../Bin/Resources/Models/SlaveVampire/SlaveVampire.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_GiantWhiteDevil"), "../Bin/Resources/Models/GiantWhiteDevil/GiantWhiteDevil.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_WhiteLargeHalberd"), "../Bin/Resources/Models/Weapon/WhiteLargeHalberd/WhiteLargeHalberd.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_SkyBox"), "../Bin/Resources/Models/Sky/SkySphere.fbx",        "textures/"},
	//{ MODELTYPE::NONANIM,  TEXT("Prototype_Component_Model_StageOne"), "../Bin/Resources/Models/Map/StageOne/StageOne.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_SlaveVampire_Sword"), "../Bin/Resources/Models/Weapon/SlaveSword/SlaveSword.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_SlaveVampire_GreatSword"), "../Bin/Resources/Models/Weapon/SlaveGreatSword/SlaveGreatSword.fbx",        "textures/"},
	//{ MODELTYPE::ANIM,  TEXT("Prototype_Component_Model_Fiona"), "../Bin/Resources/Models/Fiona/Fiona.fbx",        "/" }
};

extern unsigned int Model_PrototypeSize = 1;

// Diffuse, Gradient, Gradient_Alpha, Mask, Noise
const EFFECT_TEXTURE_DESC Effect_TexturePrototypes[] =
{
	{ TEXT("Prototype_Component_Effect_DiffuseTexture")
	, TEXT("../Bin/Resources/Textures/Effects/Particle/Diffuse/Diffuse%d.png"), 3},

	{TEXT("Prototype_Component_Effect_GradientTexture") 
	, TEXT("../Bin/Resources/Textures/Effects/Particle/Gradient/Gradient%d.png"), 2},

	{TEXT("Prototype_Component_Effect_GradientAlphaTexture") 
	, TEXT("../Bin/Resources/Textures/Effects/Particle/Gradient/Gradient_A%d.png"), 1},

	{TEXT("Prototype_Component_Texture_EffectMask")
	, TEXT("../Bin/Resources/Textures/Effects/Particle/Mask/Mask%d.png"), 16},

	//{TEXT("Prototype_Component_Texture_EffectNoise")
	//, TEXT("../Bin/Resources/Textures/Effects/Texture/Noise/Noise%d.png"), 2},
};

extern unsigned int Effect_TexturePrototypeSize = 4;

NS_END