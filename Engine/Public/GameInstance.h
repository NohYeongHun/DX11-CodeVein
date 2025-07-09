#pragma once

//#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	_float Get_TimeDelta() { return m_fTimeDelta; }

	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Clear_Resources(_uint iClearLevelID);
public:
	void Render_Begin(const _float4* pClearColor);
	HRESULT Draw();
	void Render_End(HWND hWnd = 0);
public:
	_float Rand_Normal();
	_float Rand(_float fMin, _float fMax);


	// 마우스 2D 충돌 관련 헬퍼 함수 모아두기.
public:
	// 마우스 가져오기.
	POINT Get_Mouse_Cursor(HWND hWnd);
	// 마우스 충돌 확인하기.
	_bool Mouse_InRect2D(HWND hWnd, _float2 vPosition, _float fSizeX, _float fSizeY);

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	class CComponent* Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Apply_BlendeState();
	HRESULT Apply_DepthStencilOff();
	HRESULT Apply_DefaultStates();
#pragma endregion
//
#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region FONT_MANAGER
public:
	/* 1. Load Font */
	HRESULT Load_Font(const _wstring& strFontID, const _tchar* pFontFilePath);

	/* 2. Drawing Font */
	HRESULT Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation = 0.f, const _float2& vOrigin = {}, _float fScale = 0);
#pragma endregion

#pragma region COLLIDER_MANAGER
public:
	HRESULT Add_Collider_To_Manager(class CCollider* pCollider);
#pragma endregion

#pragma region TEXTURE_MANAGER
public:
	HRESULT Add_Texture(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strTextureTag);
	void	Change_Texture_ToGameObject(class CGameObject* pGameObject, const _wstring& strComponentTag, class CComponent** ppOut, _uint iLevelIndex, const _wstring& strTextureTag);
#pragma endregion

#pragma region EVENT_MANAGER
	void Subscribe(EventType id, uint32_t iID, FCallback&& fn);

	template<typename T>
	void Publish(EventType id, T* msg);

	void UnSubscribe(EventType id, uint32_t iID);
#pragma endregion

#pragma region PIPE_LINE
	_matrix Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

#pragma region INPUT_DEVICE
	public:
		_byte	Get_DIKeyState(_ubyte byKeyID);
		_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
		_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion


//
//#pragma region PICKING 
//	void Transform_Picking_ToLocalSpace(class CTransform* pTransformCom);
//	_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
//#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CCollider_Manager*	m_pCollider_Manager = { nullptr };
	class CTexture_Manager*		m_pTexture_Manager = { nullptr };
	class CEvent_Manager*		m_pEvent_Manager = { nullptr };
	class CPipeLine*			m_pPipleLine = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	_float m_fTimeDelta = {};


public:
	void Release_Engine();
	virtual void Free() override;
};


#include "GameInstance.inl"


NS_END