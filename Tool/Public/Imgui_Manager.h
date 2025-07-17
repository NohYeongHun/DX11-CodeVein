#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CImgui_Manager final : public CBase
{


private:
	CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CImgui_Manager() = default;
public:
	void Render_Begin();
	void Render();
	void Change_Level();
	void Render_End();

public:
	HRESULT Initialize_Clone(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };


#pragma region HIERARCHY
public:
	void Render_Hierarchy();
	void Register_Hierarchy_Objects(class CGameObject* pGameObject);
	void Register_Hierarchy_Layer(class CLayer* pLayer);
#pragma endregion

private:
	list<pair<string, class CGameObject*>> m_HierarchyObjects = {};


public:
	static CImgui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
	
public:
	static CImgui_Manager* Get_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	static CImgui_Manager* m_pInstance;
	
};
NS_END

