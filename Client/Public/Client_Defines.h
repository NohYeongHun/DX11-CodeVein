#pragma once

#include "../Default/framework.h"
#include <process.h>

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };

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

	enum PLAYER_ANIMATION_KEY
	{
		// DAMAGE 애니메이션 (0-3)
		PLAYER_ANIM_DAMAGE_B = 0, PLAYER_ANIM_DAMAGE_F = 1, PLAYER_ANIM_DAMAGE_L = 2,        
		PLAYER_ANIM_DAMAGE_R = 3,                                                            

		// RUN 애니메이션 (4-11)  
		PLAYER_ANIM_RUN_B_LOOP = 4, PLAYER_ANIM_RUN_B_END = 5, PLAYER_ANIM_RUN_F_LOOP = 6,   
		PLAYER_ANIM_RUN_F_END = 7, PLAYER_ANIM_RUN_L_LOOP = 8, PLAYER_ANIM_RUN_L_END = 9,    
		PLAYER_ANIM_RUN_R_LOOP = 10, PLAYER_ANIM_RUN_R_END = 11,                             

		// WALK 애니메이션 (12-15)
		PLAYER_ANIM_WALK_B_LOOP = 12, PLAYER_ANIM_WALK_F_LOOP = 13, PLAYER_ANIM_WALK_L_LOOP = 14,
		PLAYER_ANIM_WALK_R_LOOP = 15,                                                            

		// IDLE 애니메이션 (16-17)
		PLAYER_ANIM_IDLE_SWORD = 16, PLAYER_ANIM_IDLE_SPA = 17,                                  

		// SKILL 애니메이션 (18-22)
		PLAYER_ANIM_SKILL_BARRIER = 18, PLAYER_ANIM_SKILL_BUFF = 19, PLAYER_ANIM_SKILL_ENCHANT = 20, 
		PLAYER_ANIM_SKILL_GROUND = 21, PLAYER_ANIM_SKILL_SEARCH = 22,                                

		// EXT 애니메이션 (23)
		PLAYER_ANIM_EXT_STRONG = 23,                                                                 

		// DODGE 애니메이션 (24-27)
		PLAYER_ANIM_DODGE_B = 24, PLAYER_ANIM_DODGE_F = 25, PLAYER_ANIM_DODGE_L = 26,      
		PLAYER_ANIM_DODGE_R = 27,                                                          

		// Guard 애니메이션 (28-31)
		PLAYER_ANIM_GUARD_END = 28, PLAYER_ANIM_GUARD_LOOP = 29, PLAYER_ANIM_GUARD_START = 30,
		PLAYER_ANIM_GUARD_HIT = 31,                                                           

		// Attack 애니메이션 (32-47)
		PLAYER_ANIM_ATTACK1 = 32, PLAYER_ANIM_ATTACK2 = 33, PLAYER_ANIM_ATTACK3 = 34,        
		PLAYER_ANIM_ATTACK4 = 35, PLAYER_ANIM_ATTACK5 = 36, PLAYER_ANIM_ATTACK6 = 37,        
		PLAYER_ANIM_ATTACK7 = 38, PLAYER_ANIM_ATTACK8 = 39, PLAYER_ANIM_ATTACK9 = 40,        
		PLAYER_ANIM_ATTACK10 = 41, PLAYER_ANIM_ATTACK11 = 42, PLAYER_ANIM_ATTACK12 = 43,     
		PLAYER_ANIM_ATTACK13 = 44, PLAYER_ANIM_ATTACK14 = 45, PLAYER_ANIM_ATTACK15 = 46,     
		PLAYER_ANIM_ATTACK16 = 47,                                                           

		// Special Attack 애니메이션 (48-53)
		PLAYER_ANIM_SPECIAL_DOWN2 = 48, PLAYER_ANIM_SPECIAL_DOWN3 = 49, PLAYER_ANIM_SPECIAL_DOWN4 = 50,  
		PLAYER_ANIM_SPECIAL_DOWN = 51, PLAYER_ANIM_SPECIAL_LAUNCH = 52, PLAYER_ANIM_STRONG_ATTACK = 53,  
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;


