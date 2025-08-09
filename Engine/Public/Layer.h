#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);
	list<class CGameObject*>& Get_GameObjects() { return m_GameObjects; }
	RAYHIT_DESC Get_PickingLocalObject(_float* pOutDist);
	MODEL_PICKING_INFO Get_PickingLocalObject();



public:
	HRESULT Add_GameObject(class CGameObject* pGameObject) {
		m_GameObjects.push_back(pGameObject);
		return S_OK;
	}

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	size_t PopDestroyedTo(std::list<CGameObject*>& out);

private:
	list<class CGameObject*> m_GameObjects;
	list<class CGameObject*> m_Destroyed;

public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END