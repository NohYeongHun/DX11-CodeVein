#pragma once
NS_BEGIN(Client)
typedef struct EffectTextureDesc
{
	const _tchar* prototypeName;
	const _tchar* textureFilePath;
	_uint iNumTextures;
}EFFECT_TEXTURE_DESC;

extern const EFFECT_TEXTURE_DESC Effect_TexturePrototypes[];

extern unsigned int Effect_TexturePrototypeSize;
NS_END