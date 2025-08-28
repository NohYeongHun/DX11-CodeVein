#pragma once

#include "../Default/framework.h"
#include <process.h>

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


	/* 애니메이션 바뀌더라도 이값만 바꾸게 */
	/*enum PLAYER_ANIMATION_KEY
	{
		PLAYER_ANIM_IDLE = 16,			PLAYER_ANIM_DODGE = 25,
		PLAYER_ANIM_RUN = 6,			PLAYER_ANIM_STRONG_ATTACK = 48,
		PLAYER_ANIM_GUARD_START = 30,	PLAYER_ANIM_ATTACK1 = 32,
		PLAYER_ANIM_GUARD_LOOP = 29,    PLAYER_ANIM_ATTACK2 = 33,
		PLAYER_ANIM_GUARD_END = 28,     PLAYER_ANIM_ATTACK3 = 34,
		PLAYER_ANIM_X = 50,			    PLAYER_ANIM_ATTACK4 = 35,
		PLAYER_ANIM_Y = 51,				PLAYER_ANIM_ATTACK5 = 36,
		PLAYER_ANIM_Z = 52,				PLAYER_ANIM_ATTACK6 = 37,
	};*/

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

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;


