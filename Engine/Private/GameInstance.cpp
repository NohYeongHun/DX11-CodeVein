﻿#include "GameInstance.h"

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

	//m_pPicking = CPicking::Create(*ppOut, EngineDesc.hWnd);
	//if (nullptr == m_pPicking)
	//	return E_FAIL;


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

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 내 게임내에서 반복적인 갱신이 필요한 객체들이 있다라면 갱신을 여기에서 모아서 수행하낟. */
	m_pObject_Manager->Priority_Update(fTimeDelta);

	/*m_pPicking->Update();*/

	m_pObject_Manager->Update(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Clear_Resources(_uint iClearLevelID)
{
	/* 기존레벨용 자원들을 날린다. */
	m_pPrototype_Manager->Clear(iClearLevelID);

	m_pObject_Manager->Clear(iClearLevelID);

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

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Open_Level(_uint iLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelID, pNewLevel);
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

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLayerLevelIndex, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

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

HRESULT CGameInstance::Add_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider)
{
	return m_pCollider_Manager->Add_Collider_To_Layer(eColliderLayer, pCollider);
}

HRESULT CGameInstance::Remove_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider)
{
	return m_pCollider_Manager->Remove_Collider_To_Layer(eColliderLayer, pCollider);
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


//
//void CGameInstance::Transform_Picking_ToLocalSpace(CTransform* pTransformCom)
//{
//	m_pPicking->Transform_ToLocalSpace(pTransformCom);
//}
//
//_bool CGameInstance::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
//{
//	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
//}



void CGameInstance::Release_Engine()
{
	Release();

	//Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTexture_Manager);

	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pFont_Manager);
	
	Safe_Release(m_pGraphic_Device);
	
	
}

void CGameInstance::Free()
{
	__super::Free();
}
