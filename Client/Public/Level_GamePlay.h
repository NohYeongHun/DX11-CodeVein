#pragma once

#include "Level.h"

NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize_Clone() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


#pragma region 0. 먼저 생성해야하는 객체들 생성.
private:
	HRESULT Ready_Lights();
	HRESULT Ready_HUD();
	HRESULT Ready_Layer_Map(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_SkyBox(const _wstring& strLayerTag);
#pragma endregion

	
#pragma region 1. Trigger 준비. => 보통 몬스터들을 Trigger에 담아둡니다.
private:
	HRESULT Ready_Monster_Trigger();
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_WolfDevil(const _wstring& strLayerTag);
	HRESULT Ready_Layer_QueenKnight(const _wstring& strLayerTag);
#pragma endregion



private:
	
	HRESULT Ready_Layer_SkyBoss(const _wstring& strLayerTag);
	
	
	
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	

private:
	LEVEL m_eCurLevel = { LEVEL::GAMEPLAY };
	vector<pair<EventType, uint32_t>> m_Events = {};

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END