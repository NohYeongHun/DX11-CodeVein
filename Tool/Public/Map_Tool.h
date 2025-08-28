#pragma once
#include "Base.h"

NS_BEGIN(Tool)

using LayerTable = map<const _wstring, class CLayer*>;

class CMap_Tool final : public CBase
{
public:
	enum TOOLMODE 
	{
		// 1. 프로토타입 하이어라키를 선택했을 때 메시를 원하는 위치에 배치하도록.
		// 메시에 클릭 했을 때 해당 메시 옆에 배치되도록.
		CREATE = 0, 
		EDIT = 1,
		NAV_MODE = 2,
		RENDER_END
	};

	enum SAVEMODE
	{
		MAP_OBJECT = 0,
		MODEL_COMPONENT = 1,
		SAVE_END
	};

	enum class SAVE_TYPE
	{
		MODEL = 0,
		NAVIGATION,
		MAP_OBJECT,
		END
	};

	enum TEXTURE_SHADERPATH
	{
		TEXTURE_SHADER_DEFAULT = 0,
		TEXTURE_SHADER_LINESLASH = 1,
		TEXTURE_SHADER_END
	};

	enum PARTICLE_SHADERPATH
	{
		PARTICLE_SHADER_DEFAULT = 0,
		PARTICLE_SHADER_END
	};

private:
	explicit CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(LEVEL eLevel);

public:
	void Change_SelectObject(class CGameObject* pSelectedObject);
	void Update(_float fTimeDelta);
	

#pragma region 기본 렌더
public:
	void Render();
	void Render_CheckBox();
	void SaveLoadMenu();

	/* Create Child */
	void Render_CreateModelChild();
	void Render_Prototype_Inspector();


	/* Edit Child */
	void Redner_EditModelChild();
	void Load_Layer();
	void Render_Edit_Inspector();
	
	
#pragma endregion

#pragma region SAVE_LOAD 시 사용

private:
	_bool m_IsEditModel = { false };
	_bool m_IsEditNavigation = { false };
	_bool m_IsEditMap = { false };

	SAVE_TYPE m_eSaveType = { SAVE_TYPE::MODEL };

public:
	void Open_FileDialog();
	void Save_FileDialog();
	void Handle_FileDialogs(); // 열기 다이얼로그 처리
#pragma endregion

#pragma region PICKING Manager 관리

private:
	class CPicking_Manager* m_pPicking_Manager = { nullptr };
#pragma endregion

#pragma region Navigation 사용
public:
	void Render_NavigationChild();
	void Save_Navigation(string filePath);
	void Load_Navigation(string filePath);

private:
	class CNavigationManager* m_pNavigation_Manager = { nullptr };
	_bool m_bNaviPicking = { false }; // Navigation Picking 여부.
	_float3 m_fClickPoint = {};

private:
	_int m_iCellIndex = { 0 };
#pragma endregion

	void Render_SaveLoad();
	void Render_Debug_Window();
#pragma region Create Mode
public:
	void Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix);
#pragma endregion



#pragma region EFFECT TOOL

#pragma region Effect Texture
private:
	void Render_Effect_TextureTab();
	void Render_Effect_TextureInspector();

	void Create_Effect_Texture(const _vector& vHitDirection, const _vector& vHitPosition, 
		const _float3& vScale, _float fDisplayTime, _uint iShaderPath);

private:
	CTool_EffectTexture::TOOLEFFECT_TEXTURE_DESC m_CurrentEffectTexture_Desc = {};
#pragma endregion

#pragma region Effect Particle
private:
	void Render_Effect_ParticleTab();
	void Render_Effect_ParticleInspector();


private:
	void Create_Effect_Particle();

private:	
	CTool_EffectParticle::TOOLEFFECT_PARTICLE_DESC m_CurrentEffectParticle_Desc = {};
#pragma endregion


 

#pragma endregion


#pragma region Map Tool의 기능들
public:
	void Update_Picking(_uint iLayerLevelIndex, const _wstring& strLevelLayerTag);
	void Load_EditObject(); // 현재 레벨에 생성된 객체들을 모두 불러옵니다.

	string WString_ToString(const wstring& ws);
	void SelectObject(class CGameObject* pObj);

	// 텍스처 선택을 위한 변수들
	struct SelectedTextureInfo
	{
		_bool bSelected = false;
		_uint iSelectedIndex = 0;  // 선택된 텍스처 인덱스
	};

	SelectedTextureInfo m_SelectedTextures[TEXTURE_END] = {};


#pragma endregion



#pragma region CREATE 시 사용 변수
private:
	/* 현재 선택된 SelectedObjTag */
	string m_Selected_PrototypeObjTag = {};
	string m_Selected_PrototypeModelTag = {};
	_wstring m_wSelected_PrototypeObjTag = {};
	_wstring m_wSelected_PrototypeModelTag = {};

	list<pair<string, string>> m_PrototypeNames = {};

	/* Prototype Hierarchy Pos 저장 */
	//ImVec2 m_PrototypeinspectorPos = {};
	_bool m_IsPicking_Create = {};
#pragma endregion

#pragma region EDIT 시 사용 변수.

private:
	string m_Selected_EditObjTag = {};
	uint32_t m_Selected_EditObjID = {};
	_wstring m_Selected_EditLayerTag = {};

	/* Edit Hierarchy Pos 저장 */
	//ImVec2 m_EditinspectorPos = {};
	int m_iSelectedIndex = { -1 };
#pragma endregion









#pragma region 공통
private:
	MODEL_PICKING_INFO m_ModelPickingDesc = {};
	RAYHIT_DESC m_RayHitDesc = {};
	_bool m_IsPossible_Picking = { false };
	_bool m_IsPossible_SaveLoad = { false };
	
	class CGameObject* m_pSelectedObject = { nullptr }; // 선택된 객체
	class CToolMap_Part* m_pSelectedMapPart = { nullptr };
	class CTool_Model* m_pSelectedModel = { nullptr };

	class CLayer* m_pSelectedLayer = { nullptr };
	TOOLMODE m_eToolMode = { TOOLMODE::CREATE }; // 상태 저장.

	LEVEL m_eCurLevel = { };
	_float m_Interval = 1.f;
	_float3 m_vInterval = {};

	SAVEMODE m_eSaveMode = {};

	_float m_fEditorAlpha = { 1.f};
	_bool m_bSave = {};
	_bool m_bLoad = {};
	_bool m_bShowSimpleMousePos = {};
	_bool m_bShowPickedObject = {};

	_bool m_bShowOnlyNavi = {};


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CCamera_Free* m_pCamera = { nullptr };
	class CTransform* m_pCameraTransformCom = { nullptr };
	class CSaveFile_Loader* m_pSaveFile_Loader = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };

	LayerTable m_LayerTable = {};
	vector<EventType> m_Events = {};
#pragma endregion


private:
	HRESULT Ready_Imgui();
	HRESULT Ready_Events();
	


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual void Free();
};
NS_END

