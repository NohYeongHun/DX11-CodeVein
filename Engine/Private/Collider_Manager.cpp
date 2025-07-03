#include "Collider_Manager.h"

CCollider_Manager::CCollider_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize(_uint iNumLevels)
{

	m_iNumLevels = iNumLevels;

	_uint rows = iNumLevels;

	/*for
	for (size_t i = 0; i < rows; i++)
	{
		m_Colliders[i] = new COLLIDERTYPES[COLLIDERLAYER];
	}*/

	return S_OK;
}

#pragma region ENGINE에 제공
HRESULT CCollider_Manager::Add_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider)
{
	return S_OK;
}

HRESULT CCollider_Manager::Remove_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider)
{
	return S_OK;
}
#pragma endregion




void CCollider_Manager::Priority_Update()
{
}

void CCollider_Manager::Update()
{
}

void CCollider_Manager::Late_Update()
{
}


CCollider_Manager* CCollider_Manager::Create(_uint iNumLevels)
{
	CCollider_Manager* pInstance = new CCollider_Manager();
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Create Failed : CCollider_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

}
