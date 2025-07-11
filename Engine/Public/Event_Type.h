NS_BEGIN(Engine)
enum class EventType : _uint
{
	NONE = 0,
	HUD_DISPLAY = 1,
	SKILL_CHANGE = 2,
	SKILL_EXECUTE = 3,
	LOAIDNG_DISPLAY = 4,
	LOADING_END = 5,
	LOGO_END = 6,
	OPEN_LEVEL = 7,
	OPEN_GAMEPAY = 8,
	HP_CHANGE = 9,
	STEMINA_CHANGE = 10,
	EVENT_END
};

#pragma region HUD
typedef struct tagHUDEventDesc
{
	_bool isVisibility;
}HUDEVENT_DESC;

typedef struct tagSkillChangeDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	const _tchar* pText = {};
	_uint iTextureIdx = {};
}SKILLCHANGE_DESC;


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





NS_END
