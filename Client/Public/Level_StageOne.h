#pragma once
#include "Level.h"

NS_BEGIN(Client)
class CLevel_StageOne final : public CLevel
{
private:
	CLevel_StageOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_StageOne() = default;

public:
	virtual HRESULT Initialize_Clone() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


#pragma region 0. 먼저 생성해야하는 객체들 생성.
private:
	HRESULT Ready_Layer_FadeOut(const _wstring& strLayerTag);
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
	HRESULT Ready_Layer_SlaveVampire(const _wstring& strLayerTag);
	HRESULT Ready_Layer_QueenKnight(const _wstring& strLayerTag);
#pragma endregion

private:
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);

#pragma region 2. 다음 레벨 전환
private:
	void Open_Level();
	void Start_FadeOut();
	void Start_FadeIn();

private:
	HRESULT Ready_Events();
#pragma endregion


private:
	LEVEL m_eCurLevel = { LEVEL::STAGEONE };
	vector<pair<EventType, uint32_t>> m_Events = {};
	_bool m_IsFadeOutStarted = { false };

public:
	static CLevel_StageOne* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};
NS_END

