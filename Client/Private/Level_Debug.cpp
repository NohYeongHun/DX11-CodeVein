#include "Level_Debug.h"
CLevel_Debug::CLevel_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Debug::Initialize_Clone()
{
	if (FAILED(Ready_Lights()))
	{
		CRASH("Failed Light");
		return E_FAIL;
	}

	if (FAILED(Ready_HUD()))
	{
		CRASH("Failed Ready_HUD");
		return E_FAIL;
	}

	//if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
	//{
	//	CRASH("Failed Ready_Layer_Map");
	//	return E_FAIL;
	//}

	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	//{
	//	CRASH("Failed Layer_Player");
	//	return E_FAIL;
	//}

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		CRASH("Failed Ready_Layer_Camera");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox"))))
	{
		CRASH("Failed Ready_Layer_SkyBox");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		CRASH("Failed Ready_Layer_Effect");
		return E_FAIL;
	}

	// 레벨 시작 시 FadeIn 효과 시작
	Start_FadeIn();

	for (_uint i = 0; i < ENUM_CLASS(EFFECTTYPE::END); ++i)
		m_pPoolTable[i] = m_pGameInstance->Export_EditPool(i);

	

	return S_OK;
}

void CLevel_Debug::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyUp(DIK_V))
	{
		m_pGameInstance->Change_Camera(TEXT("FreeCamera"), ENUM_CLASS(m_eCurLevel));
		return;
	}
	if (m_pGameInstance->Get_KeyUp(DIK_B))
	{
		m_pGameInstance->Change_Camera(TEXT("PlayerCamera"), ENUM_CLASS(m_eCurLevel));
	}

}

HRESULT CLevel_Debug::Render()
{
#ifdef _DEBUG
	ImGui::Begin(u8"Editor", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);

	// Editor Alpha
	ImGui::Text("Settings");
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 WindowColor = style.Colors[ImGuiCol_WindowBg];
	ImGui::SliderFloat("Editor Opacity", &m_fEditorAlpha, 0.0f, 1.0f);
	const ImVec4 NewColor = ImVec4(WindowColor.x, WindowColor.y, WindowColor.z, m_fEditorAlpha);
	style.Colors[ImGuiCol_WindowBg] = NewColor;
	ImGui::NewLine();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("TabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Effect Texture"))
		{
			Render_Effect_TextureTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Effect Particle"))
		{

		}


		/*if (ImGui::BeginTabItem("Effect Particle Tool"))
		{


			ImGui::EndTabBar();
		}*/
		ImGui::EndTabBar();
	}

	/*if (ImGui::Button("Apply SlashEffect"))
	{
		Create_SlashEffect();
	}
	
	if (ImGui::Button("Apply HitEffect"))
	{
		Create_SlashEffect();
	}*/

	

	ImGui::End();


#endif // _DEBUG

	return S_OK;
}

#pragma region 0. 무조건 만들어야하는 객체들.
HRESULT CLevel_Debug::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Debug::Ready_HUD()
{
	HUDEVENT_DESC Desc{};
	Desc.isVisibility = true;

	// 이벤트 실행이지 구독이아님.
	m_pGameInstance->Publish<HUDEVENT_DESC>(EventType::HUD_DISPLAY, &Desc);

	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC Desc = {};

	// X, Z 2배로 깔았음. => Prototype Tag에 해당하는 맵 모델 가져옴.
	Desc.PrototypeTag = L"Prototype_Component_Model_BossStage";
	Desc.vScale = { 2.f, 1.f, 2.f };
	Desc.eCurLevel = m_eCurLevel;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Map"), &Desc)))
	{
		CRASH("Failed Ready Layer Map");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC Desc{};
#pragma region 1. 플레이어에게 넣어줘야할 레벨 별 다른 값들.
	Desc.eCurLevel = m_eCurLevel;
	//Desc.vPos = { 270.f, 0.f, 0.f };
	Desc.vPos = { 100.f, 0.f, 0.f };
#pragma endregion

#pragma region 2. 게임에서 계속 들고있어야할 플레이어 값들.
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	// TODO: 이전 레벨에서 플레이어 데이터를 가져오는 로직 필요
	// 현재는 StageOne 완료 후 예상 스탯으로 설정
	Desc.fMaxHP = 1672;
	Desc.fHP = 1672; // StageOne 전투 후 감소된 HP
	Desc.fAttackPower = 90;
#pragma endregion

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"), &Desc)))
	{
		CRASH("Failed Ready GameObject Player");
		return E_FAIL;
	}


	m_pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Get_GameObjcet(
			ENUM_CLASS(m_eCurLevel)
			, TEXT("Layer_Player"), 0));
	if (nullptr == m_pPlayer)
	{
		CRASH("Failed Load Player");
		return E_FAIL;
	}

	m_pPlayerWeapon = dynamic_cast<CPlayerWeapon*>(m_pPlayer->Find_PartObject(TEXT("Com_Weapon")));
	if (nullptr == m_pPlayerWeapon)
	{
		CRASH("Failed Load Player Weapon");
		return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	//CCamera_Player::CAMERA_PLAYER_DESC CameraPlayerDesc{};
	//CameraPlayerDesc.vEye = _float4(0.f, 10.f, -20.f, 1.f);
	//CameraPlayerDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//CameraPlayerDesc.fFovy = XMConvertToRadians(60.0f);
	//CameraPlayerDesc.fNear = 0.1f;
	//CameraPlayerDesc.fFar = 500.f;
	//CameraPlayerDesc.fSpeedPerSec = 10.f;
	//CameraPlayerDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	//CameraPlayerDesc.fMouseSensor = 0.8f;
	//CameraPlayerDesc.vTargetOffset = { 0.f, 1.7f, -3.3f, 0.f };
	//CameraPlayerDesc.vLockOnOffset = { 0.f, 1.7f, -3.3f, 0.f };
	//CameraPlayerDesc.eCurLevel = m_eCurLevel;

	//list<CGameObject*> pGameObjects = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Player"))->Get_GameObjects();
	//auto iter = pGameObjects.begin();

	//CPlayer* pPlayer = dynamic_cast<CPlayer*>(*iter);
	//CameraPlayerDesc.pTarget = pPlayer;

	//if (nullptr == CameraPlayerDesc.pTarget)
	//	CRASH("Failed CameraPlayer Add");

	//if (FAILED(m_pGameInstance->Add_Camera(TEXT("PlayerCamera"), ENUM_CLASS(m_eCurLevel)
	//	, TEXT("Prototype_GameObject_Camera_Player"), &CameraPlayerDesc)))
	//{
	//	CRASH("Add Camera Player Failed");
	//	return E_FAIL;
	//}

	//// 메인 카메라 변경.
	//if (FAILED(m_pGameInstance->Change_Camera(TEXT("PlayerCamera"), ENUM_CLASS(m_eCurLevel))))
	//{
	//	CRASH("Change Camera Failed");
	//	return E_FAIL;
	//}

	//pPlayer->Set_Camera(dynamic_cast<CCamera_Player*>(m_pGameInstance->Get_MainCamera()));


	CCamera_Free::CAMERA_FREE_DESC CameraDesc{};
	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	CameraDesc.fMouseSensor = 0.3f;

	if (FAILED(m_pGameInstance->Add_Camera(TEXT("FreeCamera"), ENUM_CLASS(m_eCurLevel)
		, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	{
		CRASH("Add Free Camera Failed");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Change_Camera(TEXT("FreeCamera"), ENUM_CLASS(m_eCurLevel))))
	{
		CRASH("Change Camera Failed");
		return E_FAIL;
	}

	m_pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_MainCamera());
	if (nullptr == m_pCamera)
	{
		CRASH("Failed Create Player Camera");
		return E_FAIL;
	}

	


	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	CSky::SKY_DESC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc.PrototypeTag = TEXT("Prototype_Component_Texture_Sky");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel), strLayerTag,
		ENUM_CLASS(m_eCurLevel), TEXT("Prototype_GameObject_Sky"), &Desc)))
	{

		CRASH("Failed SkyBox");
		return E_FAIL;
	}

	return S_OK;
}

#pragma endregion



#pragma region 1. Trigger 준비. 
HRESULT CLevel_Debug::Ready_Monster_Trigger()
{
	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_WolfDevil(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_SlaveVampire(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_GiantWhiteDevil(const _wstring& strLayerTag)
{
	return S_OK;
}
#pragma endregion


#pragma region POOLING 준비.
HRESULT CLevel_Debug::Ready_Layer_Pooling()
{
	return S_OK;
}

HRESULT CLevel_Debug::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	return S_OK;
}
#pragma endregion




void CLevel_Debug::Start_FadeOut()
{
}

void CLevel_Debug::Start_FadeIn()
{
	// FadeIn 객체 찾기
	CGameObject* pFadeOut = m_pGameInstance->Get_GameObjcet(ENUM_CLASS(m_eCurLevel), TEXT("Layer_FadeOut"), 0);
	if (nullptr != pFadeOut)
	{
		CFade_Out* pFade = dynamic_cast<CFade_Out*>(pFadeOut);
		if (nullptr != pFade)
		{
			pFade->Start_FadeIn();
		}
	}
}

HRESULT CLevel_Debug::Ready_Events()
{
	return S_OK;
}

#pragma region Effect Texture
/* Effect Tab 만들기. */
void CLevel_Debug::Render_Effect_TextureTab()
{
	ImGui::BeginChild("left pane", ImVec2(500, 0), true);

	static int iSelectedIndex = -1;
	_uint id = 0;
	_uint iEffectType = ENUM_CLASS(EFFECTTYPE::TEXTURE);

	// m_PoolTable을 이용해서 선택 가능한 이펙트 리스트 표시
	for (auto& pair : m_pPoolTable[iEffectType])
	{
		const wstring& tagW = pair.first;
		string effectName = WString_ToString(tagW);

		if (ImGui::Selectable(effectName.c_str(), id == iSelectedIndex))
		{
			iSelectedIndex = id;
			m_wSelected_EffectTag = tagW;
			m_Selected_EffectTag = effectName;
		}
		id++;
	}

	// 선택된 이펙트가 있을 때 Inspector 창 표시
	if (iSelectedIndex >= 0 && iSelectedIndex < m_pPoolTable[iEffectType].size())
		Render_Effect_TextureInspector();

	ImGui::EndChild();
}

void CLevel_Debug::Render_Effect_TextureInspector()
{
	ImGuiIO& io = ImGui::GetIO();

	// 오른쪽 위 위치 계산 (창 크기 300x250 고려)
	ImVec2 vPos = ImVec2(io.DisplaySize.x - 310.f, 10.f); // 오른쪽에서 310픽셀, 위에서 10픽셀
	ImGui::SetNextWindowPos(vPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);

	ImGui::Begin("Texture_Inspector");
	ImGui::Text(m_Selected_EffectTag.c_str());

	static float fPosition[3] = { 0.f, 0.f, 0.f };
	ImGui::InputFloat3("Position", fPosition);

	static float fRotation[3] = { 0.f, 0.f, 0.f };
	ImGui::InputFloat3("Rotation", fRotation);

	static float fScale[3] = { 1.f, 1.f, 1.f };
	ImGui::InputFloat3("Scale", fScale);

	static float fDisplayTime = 5.f;
	ImGui::InputFloat("Display Time", &fDisplayTime);

	if (ImGui::Button("Create Effect"))
	{
		// 카메라 위치 기준으로 이펙트 생성
		_vector vPos = m_pCamera->Get_Transform()->Get_State(STATE::POSITION);
		_vector vLook = XMVector3Normalize(m_pCamera->Get_Transform()->Get_State(STATE::LOOK));

		// 생성할 위치 (카메라 앞쪽으로 조금 이동)
		_vector vEffectPosition = vPos + (vLook * 5.f);
		vEffectPosition = XMVectorAdd(vEffectPosition, XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 0.f));

		// 선택된 이펙트 타입에 따라 생성

		EFFECT_ACTIVEINFO effectInfo =
		{
			{fPosition[0], fPosition[1], fPosition[2]},
			{fRotation[0], fRotation[1], fRotation[2]},
			{fScale[0], fScale[1], fScale[2]},
			fDisplayTime
		};
		Create_SelectedEffect(vEffectPosition, effectInfo);
	}

	ImGui::End();
}

void CLevel_Debug::Create_SlashEffect()
{
	// 플레이어 기준 정면
	_vector vPos = m_pCamera->Get_Transform()->Get_State(STATE::POSITION);
	_vector vLook = XMVector3Normalize(m_pCamera->Get_Transform()->Get_State(STATE::LOOK));

	// 생성할 위치.
	_vector vSlashPosition = vPos += vLook * 7.f;
	_float3 vDirection = { 0.f, 1.f, 0.f };

	CSlash::SLASHACTIVATE_DESC Desc{};
	Desc.eCurLevel = m_eCurLevel;
	Desc.vHitPosition = vSlashPosition;
	Desc.vHitDirection = XMLoadFloat3(&vDirection);
	Desc.fDisPlayTime = 10.f;
	m_pGameInstance->Move_GameObject_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
		, TEXT("SLASH_EFFECT"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CSlash::EffectType), &Desc);

}

#pragma endregion




#pragma region Effect Particle
void CLevel_Debug::Render_Effect_ParticleTab()
{
}

void CLevel_Debug::Render_Effect_ParticleInspector()
{
}
#pragma endregion






#pragma region 헬퍼 함수
void CLevel_Debug::Render_ParticleTab(EFFECTTYPE eEffectType)
{
	ImGui::BeginChild("Middle pane", ImVec2(500, 0), true);

	_uint iEffectType = ENUM_CLASS(eEffectType);
	for (auto itLayer = m_pPoolTable[iEffectType].begin(); itLayer != m_pPoolTable[iEffectType].end(); ++itLayer)
	{
		// 1. PoolTable에서 현재 생성할 이름 가져오기. 
		const wstring& tagW = itLayer->first;


	}

	ImGui::EndChild();

}
string CLevel_Debug::WString_ToString(const wstring& ws)
{
	if (ws.empty()) return {};
	int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string s(len - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), len, nullptr, nullptr);
	return s;
}


void CLevel_Debug::Create_SelectedEffect(_vector vPosition, const EFFECT_ACTIVEINFO& effectInfo)
{
	if (m_wSelected_EffectTag.empty())
		return;

	// 선택된 이펙트 타입에 따라 다른 생성 로직 적용
	if (m_wSelected_EffectTag == TEXT("SLASH_EFFECT"))
	{
		_float3 vDirection = { 0.f, 1.f, 0.f };

		CSlash::SLASHACTIVATE_DESC Desc{};
		Desc.eCurLevel = m_eCurLevel;
		Desc.vHitPosition = vPosition;
		Desc.vHitDirection = XMLoadFloat3(&vDirection);
		Desc.fDisPlayTime = effectInfo.fDisplayTime;
		Desc.vScale = effectInfo.vScale;
		m_pGameInstance->Move_GameObject_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
			, TEXT("SLASH_EFFECT"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CSlash::EffectType), &Desc);
	}
	// 다른 이펙트 타입들도 여기에 추가 가능
	else if (m_wSelected_EffectTag == TEXT("HITFLASH_EFFECT"))
	{
		_float3 vDirection = { 0.f, 1.f, 0.f };

		CSlash::SLASHACTIVATE_DESC Desc{};
		Desc.eCurLevel = m_eCurLevel;
		Desc.vHitPosition = vPosition;
		Desc.vHitDirection = XMLoadFloat3(&vDirection);
		Desc.fDisPlayTime = effectInfo.fDisplayTime;
		Desc.vScale = effectInfo.vScale;
		m_pGameInstance->Move_GameObject_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
			, m_wSelected_EffectTag, TEXT("Layer_Effect"), 1, ENUM_CLASS(CHitFlashEffect::EffectType), &Desc);

	}
	// 추가 이펙트 타입들...
}
#pragma endregion



CLevel_Debug* CLevel_Debug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Debug* pInstance = new CLevel_Debug(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Debug"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Debug::Free()
{
	CLevel::Free();
}
