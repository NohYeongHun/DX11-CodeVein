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
	
public:
	void ImGui_Render();
	


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };

private:
	HRESULT Ready_Imgui();
	

private:
	void ImGui_MenuBar_Render();


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};
NS_END

