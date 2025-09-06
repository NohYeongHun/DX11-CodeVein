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
	OPEN_STAGEONE = 9,
	HP_CHANGE = 10,
	HP_SYNCRONIZE = 11, // HP_SYNCRONIZE는 HP_CHANGE와 동일한 이벤트로 사용됨.
	STEMINA_CHANGE = 12,
	INVENTORY_DISPLAY = 13,
	INVENTORY_SKILL_CHANGE = 14,
	SKILLINFO_DISPLAY = 15,
	SKILLINFO_SKILL_CHANGE = 16,
	/* Map Tool */
	SELECTED_MODEL = 17,
	OPEN_DEBUG = 18,
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
	_float fHp;
	_float fTime;
	_bool bIncrease;
}HPCHANGE_DESC;

typedef struct tagHPSyncronizeDesc
{
	_float fHp;
	_float fMaxHp;
}HPSYNCRONIZE_DESC;


typedef struct tagSteminaChangeDesc
{
	_float fStemina;
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


#pragma region MAP_TOOL

#pragma endregion

NS_END
