#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CMap_Tool final : public CBase
{
private:
	explicit CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize();
	HRESULT Ready_Events();

public:
	void Change_SelectObject(class CGameObject* pSelectedObject);

	void Render();

public:
	void Render_Hierarchy();
	void Register_Hierarchy_Objects(class CGameObject* pGameObject);
	void Register_Hierarchy_Layer(class CLayer* pLayer);
	

public:


public:
#pragma region Map Tool의 기능들
	void Transform_Render(const string& name, class CTransform* pTransform);
	void Spawn_Object(const _wstring& prototypeTag, const _float3& position);
#pragma endregion

	
	//void Transform_Render(const string& name, _float3& Transform);

//public:
//	void ImGui_Render();
	


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };

private:
	_float m_Interval = 1.f;
	class CGameObject* m_pSelectedObject = { nullptr };
	vector<EventType> m_Events = {};

private:
	list<pair<string, class CGameObject*>> m_HierarchyObjects = {};

private:
	HRESULT Ready_Imgui();
	


//private:
//	void ImGui_MenuBar_Render();


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};
NS_END

