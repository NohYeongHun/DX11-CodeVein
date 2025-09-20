#pragma once

#include "../Default/framework.h"
#include <process.h>

#pragma region FMOD


#pragma endregion


/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	enum class LEVEL { STATIC, LOADING, LOGO, STAGEONE, GAMEPLAY, DEBUG, END };

	// Player용 키 비트플래그 (최대 16개)
	enum class PLAYER_KEY : uint16_t
	{
		MOVE_FORWARD = 1 << 0,   // W
		MOVE_BACKWARD = 1 << 1,  // S  
		MOVE_LEFT = 1 << 2,		 // A
		MOVE_RIGHT = 1 << 3,	 // D
		DODGE = 1 << 4,			 // SPACE
		INTERACT = 1 << 5,		 // F
		INVENTORY = 1 << 6,		 // I
		SKILL_1 = 1 << 7,		 // Z
		SKILL_2 = 1 << 8,		 // X
		SKILL_3 = 1 << 9,		 // C
		SKILL_4 = 1 << 10,		 // V
		GUARD = 1 << 11,		 // DIK_LSHIFT
		/* Mouse */
		ATTACK = 1 << 12,		 // MOUSE_L
		LOCK_ON = 1 << 13,		 // MOUSE MB
		STRONG_ATTACK = 1 << 14,  // MOUSE RB
		// 필요한 키들 추가...
		NONE = 0
	};

	enum TEXTURE { TEXTURE_DIFFUSE, TEXTURE_GRADIENT, TEXTURE_GRADIENT_ALPHA, TEXTURE_MASK,  TEXTURE_OTHER, TEXTURE_NOISE, TEXTURE_SWIRL, TEXTURE_END };

	enum class TRAIL_DIFFUSE : _uint { SWORD = 0, THICK_BLOOD = 1, BLOOD = 2, NONE };
	enum class ANIMESH_SHADERPATH : _uint { DEFAULT = 0, NORMAL = 1, DISSOLVE = 2, SHADOW = 3, NONE };
	enum class MESH_SHADERPATH : _uint { DEFAULT = 0, NORMAL = 1, DISSOLVE = 2, BLOOD_PILLARA = 3, BLOOD_PILLARB = 4, BLOOD_PILLARC = 5, SWORD_WIND = 6, SHADOW = 7, NONE };
	enum class POSTEX_SHADERPATH : _uint { 
		DEFAULT = 0, LOADINGSLOT = 1, SKILLSLOT = 2,
		FADEOUT = 3, TITLEBACKGROUND = 4, HPPROGRESSBAR = 5,
		BLACKCOLOR = 6, LOCKON = 7, FADEIN = 8, MONSTER_LINESLASH = 9, 
		MONSTERHITFLASH = 10, MONSTERHP_PROGRESSBAR = 11, NONE
	};


	enum class EFFECTPOSTEX_SHADERPATH : _uint
	{
		DEFAULT = 0, HITFLASH = 1, MONSTER_LINESLASH = 2, NONE
	};
	enum class EFFECTTRAIL_SHADERPATH : _uint {
		TRAIL = 0, MULTITRAIL = 1, STRETCH_TRAIL = 2
		, NONE
	};

	enum class EFFECTTRAIL_DISTORTIONSHADERPATH : _uint {
		SWORD_TRAIL = 0, NONE
	};

	enum class POINTDIRPARTICLE_SHADERPATH : _uint
	{
		DEFAULT = 0, QUEENKNIGHTWARP = 1, EXPLOSION = 2,QUEENKINGHT_PARTICLE = 3, HITPARTICLE = 4, PLAYERHITPARTICLE = 5,
		TORNADO = 6, DEBUG = 7,
		NONE
	};

	enum class DISTORTION_SHADERPATH : _uint {
		DEFAULT = 0, SWORD_WIND = 1, NONE
	};

	/* 기본 스탯 구조체 */
	typedef struct tagDefaultMonsterStat
	{
		_float fMaxHP;
		_float fHP;
		_float fAttackPower;
		_float fDetectionRange;
		_float fAttackRange;
		_float fMoveSpeed;
		_float fRotationSpeed;
	}MONSTER_STAT;

	typedef struct ParticleInitInfo
	{
		_float3 pos;
		_float3 dir;
		_float lifeTime;
		_float fRadius = { 0.f };
		_float fHeight = { 0.f }; // 높이.
		_float fGatherTime = { 0.f };
		_float fExplositionTime = { 0.f };
	}PARTICLE_INIT_INFO;

	
	static const float normalize(float x, float min_val, float max_val) {
		return (x - min_val) / (max_val - min_val);
	}


}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;


