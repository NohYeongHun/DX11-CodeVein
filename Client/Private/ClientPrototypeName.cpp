// Diffuse, Gradient, Gradient_Alpha, Mask, Noise

NS_BEGIN(Client)
const EFFECT_TEXTURE_DESC ClientEffect_TexturePrototypes[] =
{
	{ TEXT("Prototype_Component_Effect_DiffuseTexture")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Diffuse/Diffuse%d.png"), 8},

	{TEXT("Prototype_Component_Effect_GradientTexture")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Gradient/Gradient%d.png"), 2},

	{TEXT("Prototype_Component_Effect_GradientAlphaTexture")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Gradient/Gradient_A%d.png"), 1},

	{TEXT("Prototype_Component_Texture_EffectMask")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Mask/Mask%d.png"), 16},

	{TEXT("Prototype_Component_Texture_EffectOther")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Other/Other%d.png"), 6},

	{TEXT("Prototype_Component_Texture_EffectNoise")
	, TEXT("../Bin/Resources/Textures/Effects/Texture/Noise/Noise%d.png"), 4},
};

extern unsigned int Effect_TexturePrototypeSize = 6;
NS_END