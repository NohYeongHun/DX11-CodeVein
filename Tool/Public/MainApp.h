#pragma once
#include "Tool_Defines.h"
#include "Base.h"

NS_BEGIN(Tool)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CImgui_Manager* m_pImGui_Manager = { nullptr };
	
	class CMap_Tool* m_pMapTool = { nullptr };

private:
	HRESULT Ready_Prototype_ForStatic();
	HRESULT Ready_Fonts(); // Font_Manager에 Font를 추가한다.
	HRESULT Ready_Console();

	//HRESULT Start_Level(LEVEL eStartLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;

};
NS_END

