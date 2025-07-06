NS_BEGIN(Engine)
enum class EventType : _uint
{
	NONE = 0,
	HUD_DISPLAY = 1,
	SKILL_CHANGE = 2,
	SKILL_EXECUTE = 3,
	LOAIDNG_DISPLAY = 4,
	LOADING_END = 5,
	OPEN_LEVEL = 6,
	EVENT_END
};

#pragma region HUD
typedef struct tagHUDEventDesc
{
	_bool isVisibility;
}HUDEVENT_DESC;

typedef struct tagSkillEventDesc
{
	_uint iSkillPanelIdx = {};
	_uint iSlotIdx = {};
	const _tchar* pText = {};
	_uint iTextureIdx = {};
}SKILLEVENT_DESC;

#pragma endregion

#pragma region LOADING
typedef struct tagLoadingEventDesc
{
	_bool isVisibility;
}LOADINGEVENT_DESC;
#pragma endregion



NS_END
