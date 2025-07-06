#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize_Clone();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CImgui_Manager* m_pImGui_Manager = { nullptr };

private:
#pragma region PROTOTYPE
	HRESULT Ready_Prototype_ForStatic();
	HRESULT Ready_Prototype_HUD();
	HRESULT Ready_Prototype_Fonts();
	HRESULT Ready_Prototype_Loading();
#pragma endregion


#pragma region STATIC OBJECT
	HRESULT Ready_Clone_ForStatic();
	HRESULT Ready_Clone_HUD(const _wstring& strLayerTag);
	HRESULT Ready_Clone_Loading(const _wstring& strLayerTag);
#pragma endregion

	
	HRESULT Start_Level(LEVEL eStartLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;		
};

NS_END
