#pragma once


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
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CImgui_Manager* m_pImGui_Manager = { nullptr };


	_float m_fThreshold = { 0.8f };
	_float m_fSoft = { 0.2f };
private:
	// 로더용 클래스 하나 가져오기.
	CLoader_Static m_cLoader_Static;


#pragma region STATIC OBJECT
	HRESULT Ready_Clone_ForStatic();
	HRESULT Ready_Clone_HUD(const _wstring& strLayerTag);
	HRESULT Ready_Clone_Inventory(const _wstring& strLayerTag);
	HRESULT Ready_Clone_SkillUI(const _wstring& strLayerTag);
	
#pragma endregion


#pragma region POOLING
	HRESULT Ready_Pooling();
#pragma endregion


	
	HRESULT Start_Level(LEVEL eStartLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;		
};

NS_END
