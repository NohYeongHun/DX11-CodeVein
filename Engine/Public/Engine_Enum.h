#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	
	enum class CELLPOINT { A, B, C, END };

	enum class MODELTYPE : _ubyte { 
		ANIM = 0
		, NONANIM = 1
		, STATIC  = 2
	};
	enum class ACTORDIR { U, RU, R, RD, D, LD, L, LU, END};

	enum class MONSTERTYPE { BOSS, ELITE, NORMAL, END };
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, STATIC_UI, CAMERA, END };
	enum class WINMODE { FULL, WIN, END };
	enum class D3DTS { VIEW, PROJ, END };
	
	enum class MOUSEKEYSTATE { LB, RB, MB, END	};
	enum class MOUSEMOVESTATE {	X, Y, Z, END	};

	enum class COLLIDERSHAPE { NONE = 0, AABB, OBB, SPHERE, CAPSULE, END };

	// 콜라이더 타입 별로 배열에 담을 것.
	enum class COLLIDERLAYER : uint32_t {
		  NONE = 0
		, PLAYER = 1 << 1
		, ENEMY = 1 << 2
		, STATIC = 1 << 3
		, DYNAMIC = 1 << 4
		, END
	};


	// EditObject.h
	enum class EEditType : uint8_t
	{
		Transform,          // 위치·회전·스케일
		Rename,             // 태그·이름 바꾸기
		ComponentValue,     // 컴포넌트 파라미터 (예: 머티리얼 색상)
		Custom              // 툴 전용 스크립트 등
	};

	// 세부 페이로드
	struct TransformData
	{
		_float4 pos;
		_float3 rot;
		_float3 scale;
	};

	struct RenameData { std::wstring newName; };
	struct ComponentData      // 예시: 머티리얼 파라미터 하나 바꾸기
	{
		std::wstring componentTag;
		std::wstring paramName;
		float        fValue;
	};

	// 공용 래퍼
	struct EditPayload
	{
		EEditType type;
		std::variant<TransformData, RenameData, ComponentData> data;
	};

	enum class BT_RESULT : _ubyte
	{
		SUCCESS = 0,
		FAILURE,
		RUNNING,
		INVALID
	};

	// BT 노드 타입
	enum class BT_NODE_TYPE : _ubyte
	{
		COMPOSITE = 0,  // Sequence, Selector, Parallel
		DECORATOR,      // Inverter, Repeater, etc
		LEAF            // Action, Condition
	};

	//enum KEY_CODE
	//{
	//	KEY_START = 0,

	//	// 1, 2, 3, 4, 5, 6 ,7, 8, 9...
	//	Alpha0 = 0x30, Alpha1 = 0x31, Alpha2 = 0x32, Alpha3 = 0x33, Alpha4 = 0x34, Alpha5 = 0x35, Alpha6 = 0x36, Alpha7 = 0x37, Alpha8 = 0x38, Alpha9 = 0x39,

	//	Q = 'Q', W = 'W', E = 'E', R = 'R', T = 'T', Y = 'Y', U = 'U', I = 'I', O = 'O', P = 'P',
	//	A = 'A', S = 'S', D = 'D', F = 'F', G = 'G', H = 'H', J = 'J', K = 'K', L = 'L',
	//	Z = 'Z', X = 'X', C = 'C', V = 'V', B = 'B', N = 'N', M = 'M',

	//	SHIFT = VK_SHIFT, L_SHIFT = VK_LSHIFT, R_SHIFT = VK_RSHIFT, CONTROL = VK_CONTROL, L_CONTROL = VK_LCONTROL, R_CONTROL = VK_RCONTROL,
	//	ALT = VK_MENU, L_ALT = VK_LMENU, R_ALT = VK_RMENU,

	//	RETURN = VK_RETURN, ESCAPE = VK_ESCAPE, SPACE = VK_SPACE, TAB = VK_TAB, KEY_DELETE = VK_DELETE,

	//	UP = VK_UP, DOWN = VK_DOWN, LEFT = VK_LEFT, RIGHT = VK_RIGHT,

	//	MOUSE_L = VK_LBUTTON, MOUSE_R = VK_RBUTTON, MOUSE_WHILL = VK_MBUTTON,

	//	KEY_END
	//};
}

#endif // Engine_Enum_h__
