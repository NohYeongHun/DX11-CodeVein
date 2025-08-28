#pragma once

NS_BEGIN(Client)

using PoolTable = map<const _wstring, vector<class CGameObject*>>;
class CLevel_Debug final : public CLevel
{
private:
	typedef struct tagEffectActivateInfo
	{
		_float3 vPosition;
		_float3 vRotation;
		_float3 vScale;
		_float fDisplayTime;
	}EFFECT_ACTIVEINFO;

private:
	CLevel_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Debug() = default;

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
	HRESULT Ready_Layer_SlaveVampire(const _wstring& strLayerTag);
	HRESULT Ready_Layer_GiantWhiteDevil(const _wstring& strLayerTag);
#pragma endregion

#pragma region 풀링 준비.
private:
	HRESULT Ready_Layer_Pooling();
#pragma endregion


private:
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);

#pragma region 2. 다음 레벨 전환
private:
	void Start_FadeOut();
	void Start_FadeIn();

private:
	HRESULT Ready_Events();
#pragma endregion


#pragma region 3. Debug시 필요한 정보
private:
	class CPlayer* m_pPlayer = {};
	class CPlayerWeapon* m_pPlayerWeapon = {};
	class CCamera_Free* m_pCamera = {};
	//class CCamera_Player* m_pCamera = {};
#pragma endregion


#pragma region 4. Effect Debug


#pragma region Effect Texture
private:
	// Texture Tab
	void Render_Effect_TextureTab();
	void Render_Effect_TextureInspector();

private:
	void Create_SlashEffect(); // 플레이어 정면에 생성하기.
#pragma endregion


#pragma region Effect Particle
private:
	void Render_Effect_ParticleTab();
	void Render_Effect_ParticleInspector();

#pragma endregion








private:
	_float m_fEditorAlpha = { 1.f };
	list<pair<string, string>> m_EffectNames = {};
	PoolTable* m_pPoolTable = {};

	// 선택된 이펙트 관련 변수들
	_wstring m_wSelected_EffectTag = {};
	string m_Selected_EffectTag = {};

#pragma endregion


#pragma region 5. PARTICLE
private:
	void Render_ParticleTab(EFFECTTYPE eEffectType);
#pragma endregion


#pragma region 5.헬퍼 함수
private:
	string WString_ToString(const wstring& ws);
	void Create_SelectedEffect(_vector vPosition, const EFFECT_ACTIVEINFO& effectInfo);
#pragma endregion


private:
	LEVEL m_eCurLevel = { LEVEL::DEBUG };
	vector<pair<EventType, uint32_t>> m_Events = {};
	_bool m_IsFadeOutStarted = { false };

	

public:
	static CLevel_Debug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};
NS_END

