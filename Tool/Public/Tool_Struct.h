#pragma once

NS_BEGIN(Tool)
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, OBJECTEDIT, MAPEDIT, MONSTER, MAIN, MAIN2, MAIN3, END };

typedef struct tagBaseStructDesc
{
	LEVEL eLevel = { LEVEL::END };
}BASE_DESC;

typedef struct tagStaticTileDescTEMP_DESC : public BASE_DESC
{
	_float3 vScale = { 1.f, 1.f, 1.f };
	_float3 vRotate = { 0.f, 0.f, 0.f };
	_float3 vPos = { 0.f, 0.f, 0.f };
	_int iIndex = { 0 };
} STATIC_TILE_DESC;


typedef struct tagPlayerStructInfo
{
	_uint iMaxHp;
	_uint iHp;
	_uint iCoin;
	_uint iScore;
}PLAYER_DATA_INFO;

typedef struct tagHierarchyObjectDesc
{
	class CGameObject* pGameObject = { nullptr };
	const char* szName = {}; // 이름.
}HIERARCHY_OBJECT_DSEC;


NS_END
