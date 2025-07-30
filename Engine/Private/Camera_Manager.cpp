#include "Camera_Manager.h"

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize(_uint iNumLevels)
{
	m_Cameras = new CAMERAS[iNumLevels];
	m_iNumLevels = iNumLevels;

	return S_OK;
}

/* 업데이트 카메라 매니저*/
void CCamera_Manager::Update(_float fTimeDelta)
{
	if (nullptr != m_pCurrentCamera)
		m_pCurrentCamera->Update(fTimeDelta);
}

void CCamera_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_Cameras[iLevelIndex])
		Safe_Release(Pair.second);

	m_Cameras[iLevelIndex].clear();
}


#pragma region ENGINE에서 제공

CCamera* CCamera_Manager::Get_MainCamera()
{
	if (nullptr != m_pCurrentCamera)
	{
		return m_pCurrentCamera;
	}

	return nullptr;
}

/* 여기서 생성*/
HRESULT CCamera_Manager::Add_Camera(const _wstring& strCameraTag, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr != Find_Camera(strCameraTag, iLevelIndex))
	{
		CRASH("Find Camera Failed");
		return E_FAIL;
	}
		

	CCamera* pCamera = dynamic_cast<CCamera*>(
		m_pGameInstance->Clone_Prototype
		(PROTOTYPE::GAMEOBJECT, iLevelIndex, strPrototypeTag, pArg));

	if (nullptr == pCamera)
	{
		CRASH("Clone Camera Failed");
		return E_FAIL;
	}

	m_Cameras[iLevelIndex].emplace(strCameraTag, pCamera);

	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(const _wstring& strCameraTag, _uint iLevelIndex)
{
	auto    iter = m_Cameras[iLevelIndex].find(strCameraTag);
	if (iter == m_Cameras[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CCamera_Manager::Change_Camera(const _wstring& strCameraTag, _uint iLevelIndex)
{
	CCamera* pCamera = Find_Camera(strCameraTag, iLevelIndex);

	if (nullptr == pCamera)
	{
		CRASH("Change Camera Failed");
		return E_FAIL;
	}

	m_pCurrentCamera = pCamera;
		
	return S_OK;
}



#pragma endregion


CCamera_Manager* CCamera_Manager::Create(_uint iNumLevels)
{
	CCamera_Manager* pInstance = new CCamera_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_Cameras[i])
			Safe_Release(Pair.second);

		m_Cameras[i].clear();
	}
	
	Safe_Delete_Array(m_Cameras);
}
