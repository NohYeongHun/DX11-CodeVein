#pragma once

NS_BEGIN(Tool)

typedef struct ModelDesc
{
	MODELTYPE eModelType;
	const _tchar* prototypeName;
	const _char* modelPath;
	const _char* texturePath;
}MODEL_DESC;


/* 사용할 Prototype Name들을 배열에 미리 저장 */
//extern const _tchar* Model_PrototypeNames[MAX_PATH];
extern const MODEL_DESC Model_Prototypes[];
extern unsigned int Model_PrototypeSize;

#pragma region EFFECT

/* Loading Logo Level에서 게임오브젝트 생성.*/
typedef struct EffectTextureDesc
{
	const _tchar* prototypeName;
	const _tchar* textureFilePath;
	_uint iNumTextures;
}EFFECT_TEXTURE_DESC;

extern const EFFECT_TEXTURE_DESC Effect_TexturePrototypes[];
extern unsigned int Effect_TexturePrototypeSize;

#pragma endregion

NS_END