#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CPicking_Manager : public CBase
{
private:
	explicit CPicking_Manager();
	virtual ~CPicking_Manager() = default;

public:
	void	Clear_PickingMgr();

public:
	// 1. LayerTag에 해당하는 그룹들을 Picking List에 담는다.
	void Load_PickingGroup(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	
	void  Add_PickingGroup(CGameObject* pGameObject);
	void  Out_PickingGroup(CGameObject* pGameObject);


public: /*Get*/
	_float3 Get_PickingPos() { return m_vPickingPos; }
	class CGameObject* Get_PickedObj() { return m_pPickedObj; }

public:
	_bool	Picking();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CGameObject*  m_pPickedObj = nullptr;
	list<CGameObject*>  m_LoadObjects;
	list<CGameObject*>	m_GameObjects;
	_bool				m_IsMouseInUI = false;
	_float3				m_vPickingPos = _float3(0.f, 0.f, 0.f);

private:


public:
	static CPicking_Manager* Create();
	virtual void Free() override;
};
NS_END;

