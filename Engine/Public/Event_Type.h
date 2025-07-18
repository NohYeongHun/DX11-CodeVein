NS_BEGIN(Engine)
enum class EventType : _uint
{
	/* Client */
	NONE = 0,
	HUD_DISPLAY = 1,
	HUD_SKILL_CHANGE = 2,
	SKILL_EXECUTE = 3,
	LOAIDNG_DISPLAY = 4,
	LOADING_END = 5,
	LOGO_END = 6,
	OPEN_LEVEL = 7,
	OPEN_GAMEPAY = 8,
	HP_CHANGE = 9,
	STEMINA_CHANGE = 10,
	INVENTORY_DISPLAY = 11,
	INVENTORY_SKILL_CHANGE = 12,
	SKILLINFO_DISPLAY = 13,
	SKILLINFO_SKILL_CHANGE = 14,
	/* Map Tool */
	SELECTED_MODEL = 15,
	EVENT_END
};

#pragma region HUD
typedef struct tagHUDEventDesc
{
	_bool isVisibility;
}HUDEVENT_DESC;

typedef struct tagInventoryDisplayDesc
{
	_bool isVisibility;
}INVENTORY_DISPLAY_DESC;

typedef struct tagHUDSkillChangeDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	_uint iTextureIdx = {};
	void* pSkillIcon = { nullptr };
}HUD_SKILLCHANGE_DESC;


typedef struct tagSkillExecuteDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	_float fSkillCoolTime = {};
}SKILLEXECUTE_DESC;


typedef struct tagHPChangeDesc
{
	_uint iHp;
	_float fTime;
	_bool bIncrease;
}HPCHANGE_DESC;


typedef struct tagSteminaChangeDesc
{
	_uint iStemina;
	_float fTime;
	_bool bIncrease;
}STEMINA_CHANGE_DESC;
#pragma endregion

#pragma region LOADING
typedef struct tagLoadingEventDesc
{
	_bool isVisibility;
}LOADINGEVENT_DESC;
#pragma endregion


#pragma region INVENTORY
typedef struct tagInventorySkillChangeDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	_uint iTextureIdx = {};
	void* pSkillIcon = { nullptr };
}INVENTORY_SKILLCHANGE_DESC;
#pragma endregion

#pragma region SKILLINFO
typedef struct tagSkillInfoDisplayDesc
{
	_bool Isvisibility;
	_uint iPanelType;  // 패널 타입 (0: 스킬, 1: 아이템, 2: 상태, 3: 상태 정보)
	_uint iPanelIndex; // 스킬을 교체할 패널
	_uint iSlotIndex;  // 스킬을 교체할 슬롯
}SKILLINFO_DISPLAY_DESC;

typedef struct tagSkillInfoSkillChangeDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	const _tchar* pText = {};
	_uint iTextureIdx = {};
}SKILLINFO_SKILL_CHANGE_DESC;
#pragma endregion


/* Map Tool*/

typedef struct tagMapToolSelectedEventDesc
{
	class CGameObject* pSelectedObject = { nullptr }; // 선택한 객체 포인터 전달해주기.
}TOOL_SELECT_OBJECT_DESC;

NS_END
