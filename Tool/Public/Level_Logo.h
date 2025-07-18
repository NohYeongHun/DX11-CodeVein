#pragma once

#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize_Clone() override;
	virtual void Update(_float fTimeDelta) override;
	
	virtual HRESULT Render() override;

public:
	void Open_Level();

private:
	vector<pair<EventType, uint32_t>> m_Events = {};
	
	CMap_Tool* m_pMapTool = { nullptr };

	_bool m_IsLogoEnd = { false };
	LEVEL m_eCurLevel = LEVEL::LOGO;

	

private:
	HRESULT Ready_Layer_Title(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map_Parts(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	
	HRESULT Ready_Events();

private:
	HRESULT Ready_Map_Tool();


public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END