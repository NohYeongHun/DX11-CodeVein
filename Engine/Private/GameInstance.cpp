#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

#pragma region ENGINE

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;


	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;	

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	
	m_pCollider_Manager = CCollider_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	// Texture 자원 공유용
	m_pTexture_Manager = CTexture_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pTexture_Manager)
		return E_FAIL;

	// Level과 관련 없이 보관하고. 
	// 이벤트 객체를 소유한 객체가 지워질 때 이벤트 매니저에서도 소멸되어야 한다.
	m_pEvent_Manager = CEvent_Manager::Create();
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	// PipeLine
	m_pPipleLine = CPipeLine::Create();
	if (nullptr == m_pPipleLine)
		return E_FAIL;

	// Input Device
	m_pInput_Device = CInput_Device::Create(EngineDesc.hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	// Light Manager
	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	// Camera Manager
	m_pCamera_Manager = CCamera_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	// Task Queue 우선 처리
	if (FAILED(Task()))
		return;

	// 1. 입력 처리
	m_pInput_Device->Update();

	// 2. 게임 오브젝트 우선 업데이트
	m_pObject_Manager->Priority_Update(fTimeDelta);

	// 3. 게임 오브젝트 일반 업데이트 (플레이어 움직임 완료)
	m_pObject_Manager->Update(fTimeDelta);

	// 4. ⭐ 카메라 업데이트 (최신 플레이어 위치로 View 행렬 계산)
	// => 플레이어 움직임 이후에 바로 이동.
	m_pCamera_Manager->Update(fTimeDelta);

	// 5. 게임 오브젝트 Late 업데이트
	m_pObject_Manager->Late_Update(fTimeDelta);

	// 6. ⭐ 파이프라인 업데이트 (카메라에서 설정한 View 행렬 적용)
	m_pPipleLine->Update();

	// 7. ⭐ 피킹 업데이트 (최신 View/Projection 행렬 사용)
	m_pPicking->Update();

	// 8. 충돌 처리
	m_pCollider_Manager->Update();

	// 9. 레벨 업데이트
	m_pLevel_Manager->Update(fTimeDelta);

}



HRESULT CGameInstance::Clear_Resources(_uint iClearLevelID)
{
	/* 기존레벨용 자원들을 날린다. */
	m_pObject_Manager->Clear(iClearLevelID);
	// 오브젝트 매니저에서 관리하지 않는다.
	m_pCamera_Manager->Clear(iClearLevelID); 

	m_pPrototype_Manager->Clear(iClearLevelID);

	
	

	return S_OK;
}


HRESULT CGameInstance::Task()
{
	while (!m_Tasks.empty())
	{
		INSTANCE_TASK task = m_Tasks.front();
		HRESULT hr = task.fn();

		if (FAILED(hr))
		{
			MSG_BOX(TEXT("Game Instance Task Queue Failed"));
			return E_FAIL;
		}
			
		m_Tasks.pop();
	}

	return S_OK;
}

void CGameInstance::Render_Begin(const _float4* pClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return;
	
	m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
	
	m_pGraphic_Device->Clear_DepthStencil_View();		
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pRenderer)
		return E_FAIL;

	/* 백버퍼에 그릴것들을 그린다. */
	m_pRenderer->Draw();

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;



	return S_OK;
}

void CGameInstance::Render_End(HWND hWnd)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Present();
}

_float CGameInstance::Rand_Normal()
{
	return static_cast<_float>(rand()) / RAND_MAX;	
}

_float CGameInstance::Rand(_float fMin, _float fMax)
{	
	return fMin + Rand_Normal() * (fMax - fMin);	
}

// 현재 창의 마우스 좌표로 가져와야함.
POINT CGameInstance::Get_Mouse_Cursor(HWND hWnd)
{
	POINT pt{};

	// pt에 현재 마우스 포스 가져오기.
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	return pt;
}

// 윈도우 좌표 기준의 (2D 좌표를 제공해야합니다. )
_bool CGameInstance::Mouse_InRect2D(HWND hWnd, _float2 vPosition, _float fSizeX, _float fSizeY)
{
	POINT pt = Get_Mouse_Cursor(hWnd);
	RECT rcWindow = { (LONG)(vPosition.x - fSizeX * 0.5f)
		, (LONG)(vPosition.y - fSizeY * 0.5f), 
		  (LONG)(vPosition.x + fSizeX * 0.5f), 
		  (LONG)(vPosition.y + fSizeY * 0.5f) };

	return PtInRect(&rcWindow, pt);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Open_Level(_uint iLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	CLevel_Manager::CHANGE_LEVEL_DESC Desc{};
	Desc.iLevelID = iLevelID;
	Desc.pNewLevel = pNewLevel;
	
	// Task 넣어주기.
	m_Tasks.emplace(INSTANCE_TASK{ [desc = Desc, this]() {
			return m_pLevel_Manager->Open_Level(desc.iLevelID, desc.pNewLevel);
		}});

	return S_OK;
}

/* 현재 레벨 확인. */
_uint CGameInstance::Get_CurrentLevelID()
{
	if (nullptr == m_pLevel_Manager)
		return 0;

	return m_pLevel_Manager->Get_CurrentLevelID();
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iPrototypeLevelIndex, strPrototypeTag, pArg);	
}

CBase* CGameInstance::Get_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Get_Prototype(ePrototype, iPrototpyeLevelIndex, strPrototypeTag);
}

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLayerLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CLayer* CGameInstance::Get_Layer(_uint iLayerIndex, const _wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr; 

	return m_pObject_Manager->Get_Layer(iLayerIndex, strLayerTag);
}

/* 읽기 전용 Export EditLayer*/
const LayerTable& CGameInstance::Export_EditLayer(_uint iLayerLevelIndex)
{
	return m_pObject_Manager->Export_EditLayer(iLayerLevelIndex);
}

void CGameInstance::Request_EditObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID, const EditPayload& payload)
{
	m_pObject_Manager->Request_EditObject(iLayerLevelIndex, strLayerTag, objID, payload);
}

void CGameInstance::Request_DeleteObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID)
{
	m_pObject_Manager->Request_DeleteObject(iLayerLevelIndex, strLayerTag, objID);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

/* Layer 객체들이 Picking 되었는지 확인하기. */
RAYHIT_DESC CGameInstance::Get_PickingLocalObject(_uint iLayerLevelIndex, const _wstring strLayerTag, _float* pOutDist)
{
	return m_pObject_Manager->Get_PickingLocalObject(iLayerLevelIndex, strLayerTag, pOutDist);
}

///* Layer 객체들이 Picking 되었는지 확인하기. */
//CGameObject* CGameInstance::Get_PickingObject(_uint iLayerLevelIndex, const _wstring strLayerTag, const _float3& vRayOrigin, const _float3& vRayDir, _float* pOutDist)
//{
//	if (nullptr == m_pObject_Manager)
//		return nullptr;
//
//	return m_pObject_Manager->Get_PickingObject(iLayerLevelIndex, strLayerTag, vRayOrigin, vRayDir, pOutDist);
//}

#pragma endregion

#pragma region RENDERER


HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Apply_BlendeState()
{
	return m_pRenderer->Apply_BlendeState();
}

HRESULT CGameInstance::Apply_DepthStencilOff()
{
	return m_pRenderer->Apply_DepthStencilOff();
}

HRESULT CGameInstance::Apply_DefaultStates()
{
	return m_pRenderer->Apply_DefaultStates();
}


#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Load_Font(const _wstring& strFontID, const _tchar* pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Load_Font(strFontID, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(strFontTag, pText, vPosition, vColor, fRotation, vOrigin, fScale);
}

#pragma endregion

#pragma region COLLIDER_MANAGER
HRESULT CGameInstance::Add_Collider_To_Manager(CCollider* pCollider)
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return m_pCollider_Manager->Add_Collider_To_Manager(pCollider);
}

#pragma endregion


#pragma region TEXTURE_MANAGER
HRESULT CGameInstance::Add_Texture(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strTextureTag)
{
	return m_pTexture_Manager->Add_Texture(iLevelIndex, strPrototypeTag, strTextureTag);
}

void CGameInstance::Change_Texture_ToGameObject(class CGameObject* pGameObject, const _wstring& strComponentTag, class CComponent** ppOut, _uint iLevelIndex, const _wstring& strTextureTag)
{
	m_pTexture_Manager->Change_Texture_ToGameObject(pGameObject, strComponentTag, ppOut, iLevelIndex, strTextureTag);
}
#pragma endregion

#pragma region EVENT_MANAGER
void CGameInstance::Subscribe(EventType id, uint32_t iID, FCallback&& fn)
{
	m_pEvent_Manager->Subscribe(id, iID, std::move(fn));
}

void CGameInstance::UnSubscribe(EventType id, uint32_t iID)
{
	m_pEvent_Manager->UnSubscribe(id, iID);
}
_matrix CGameInstance::Get_Transform_Matrix(D3DTS eTransformState) const
{
	return m_pPipleLine->Get_Transform_Matrix(eTransformState);
}
const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eTransformState) const
{
	return m_pPipleLine->Get_Transform_Float4x4(eTransformState);
}
_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eTransformState) const
{
	return m_pPipleLine->Get_Transform_Matrix_Inverse(eTransformState);
}
const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const
{
	return m_pPipleLine->Get_Transform_Float4x4_Inverse(eTransformState);
}
const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipleLine->Get_CamPosition();
}
void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix Matrix)
{
	m_pPipleLine->Set_Transform(eTransformState, Matrix);
}
void CGameInstance::Set_Transform(D3DTS eTransformState, const _float4x4& Matrix)
{
	m_pPipleLine->Set_Transform(eTransformState, Matrix);
}

#pragma endregion

#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}
_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}
_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
_bool CGameInstance::Get_KeyPress(_ubyte byKeyID)
{
	return m_pInput_Device->Get_KeyPress(byKeyID);
}
_bool CGameInstance::Get_KeyUp(_ubyte byKeyID)
{
	return m_pInput_Device->Get_KeyUp(byKeyID);
}
_bool CGameInstance::Get_MouseKeyPress(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_MouseKeyPress(eMouse);
}
_bool CGameInstance::Get_MouseKeyUp(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_MouseKeyUp(eMouse);
}

#pragma endregion

#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
const _float3& CGameInstance::Get_RayOrigin()
{
	return m_pPicking->Get_RayOrigin();
}
const _float3& CGameInstance::Get_RayDir()
{
	return m_pPicking->Get_RayDir();
}
const _float3& CGameInstance::Get_Local_RayOrigin()
{
	return m_pPicking->Get_Local_RayOrigin();
}
const _float3& CGameInstance::Get_Local_RayDir()
{
	return m_pPicking->Get_Local_RayDir();
}
void CGameInstance::Transform_To_LocalSpace(_matrix WorldInverseMatrix)
{
	m_pPicking->Transform_To_LocalSpace(WorldInverseMatrix);
}


#pragma endregion

#pragma region CAMERA_MANAGER
HRESULT CGameInstance::Add_Camera(const _wstring& strCameraTag, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pCamera_Manager->Add_Camera(strCameraTag, iLevelIndex, strPrototypeTag, pArg);
}
CCamera* CGameInstance::Find_Camera(const _wstring& strCameraTag, _uint iLevelIndex)
{
	return m_pCamera_Manager->Find_Camera(strCameraTag, iLevelIndex);
}
CCamera* CGameInstance::Get_MainCamera()
{
	return m_pCamera_Manager->Get_MainCamera();
}
HRESULT CGameInstance::Change_Camera(const _wstring& strCameraTag, _uint iLevelIndex)
{
	return m_pCamera_Manager->Change_Camera(strCameraTag, iLevelIndex);
}
#pragma endregion



void CGameInstance::Release_Engine()
{
	Release();

	
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTexture_Manager);

	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pPipleLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pCamera_Manager);


	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	
	
}

void CGameInstance::Free()
{
	__super::Free();
}
