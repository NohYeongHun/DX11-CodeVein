#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CMap_Tool final : public CBase
{
public:
	enum RENDERTYPE 
	{
		// 1. 프로토타입 하이어라키를 선택했을 때 메시를 원하는 위치에 배치하도록.
		// 메시에 클릭 했을 때 해당 메시 옆에 배치되도록.
		
		MODEL_CREATE = 0, 
		MODEL_EDIT = 1,
		RENDER_END
	};


private:
	explicit CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(LEVEL eLevel);
	HRESULT Ready_Events();

public:
	void Change_SelectObject(class CGameObject* pSelectedObject);

	void Render();

public:
#pragma region Prototype Manager Hierarchy (생성 가능한 객체) 
	void Render_Prototype_Hierarchy();
	void Render_Prototype_Inspector();
	//void Register_Prototype_HierarchyObjects(class CGameObject* pGameObject);
	void Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix);
#pragma endregion

	

#pragma region Object Manager Hierarchy (생성된 객체)
public:
	void Render_Layer_Hierarchy();

	void Register_Layer_HierarchyObjects(class CGameObject* pGameObject);
	void Register_Layer_Hierarchy(class CLayer* pLayer);
#pragma endregion

	
	

public:
	RENDERTYPE m_eRenderType = { RENDER_END };

public:
#pragma region Map Tool의 기능들
	void Transform_Render(const string& name, class CTransform* pTransform);
	//void Spawn_Object(const _wstring& prototypeTag, const _float3& position);
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
	LEVEL m_eCurLevel = {};
	_float m_Interval = 1.f;
	class CGameObject* m_pSelectedObject = { nullptr };
	vector<EventType> m_Events = {};

private:
	list<pair<string, class CGameObject*>> m_Layer_Objects = {};
	list<pair<string, string>> m_PrototypeNames = {};


	/* 현재 선택된 SelectedObjTag */
private:
	string m_Selected_PrototypeObjTag = {};
	string m_Selected_PrototypeModelTag = {};
	_wstring m_wSelected_PrototypeObjTag = {};
	_wstring m_wSelected_PrototypeModelTag = {};

private:
	HRESULT Ready_Imgui();
	


//private:
//	void ImGui_MenuBar_Render();


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual void Free();
};
NS_END

