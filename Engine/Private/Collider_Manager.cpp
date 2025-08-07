#include "Collider_Manager.h"

CCollider_Manager::CCollider_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	return S_OK;
}

#pragma region ENGINE에 제공
HRESULT CCollider_Manager::Add_Collider_To_Manager(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_ColliderList.emplace_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

// 매 프레임 추가하고 => 충돌 검사 후에 => 매 프레임 제거합니다.
void CCollider_Manager::Update()
{

	//OutputDebugWstring(TEXT("콜라이더 등록 개수 : "));
	//OutPutDebugInt(m_ColliderList.size());

	for (auto& pLeft : m_ColliderList)
	{
		for (auto& pRight : m_ColliderList)
		{
			// 예외처리.
			if (pLeft == pRight || nullptr == pLeft || nullptr == pRight)
				continue;

			CGameObject* pLeftOwner = pLeft->Get_Owner();
			CGameObject* pRightOwner = pRight->Get_Owner();

			if (nullptr == pLeftOwner || nullptr == pRightOwner)
				continue;

			// 1. 만약 Active 되어 있지 않은 Collider 라면?
			if (!pLeft->Is_Active() || !pRight->Is_Active())
			{
				// Collider Left가 소유하고 있다면
				if (pLeft->Find_ColliderObject(pRightOwner))
				{
					pLeftOwner->On_Collision_Exit(pRightOwner);
					pRightOwner->On_Collision_Exit(pLeftOwner);
					pLeft->Erase_ColliderObject(pRightOwner);
					pRight->Erase_ColliderObject(pLeftOwner);
				}
				continue;
			}

			if (pLeft->Intersect(pRight) && pRight->Intersect(pLeft))
			{
				// 충돌 했는데 충돌 목록에 없다면? Enter
				if (!pLeft->Find_ColliderObject(pRightOwner) && !pRight->Find_ColliderObject(pLeftOwner))
				{
					pLeftOwner->On_Collision_Enter(pRightOwner);
					pRightOwner->On_Collision_Enter(pLeftOwner);
					pLeft->Insert_ColliderObject(pRightOwner);
					pRight->Insert_ColliderObject(pLeftOwner);
				}
				// 충돌 했는데 충돌 목록에 있다면? Stay
				else if (pLeft->Find_ColliderObject(pRightOwner) && pRight->Find_ColliderObject(pLeftOwner))
				{
					pLeftOwner->On_Collision_Stay(pRightOwner);
					pRightOwner->On_Collision_Stay(pLeftOwner);
				}
			}
			else
			{
				if (pLeft->Find_ColliderObject(pRightOwner) && pRight->Find_ColliderObject(pLeftOwner))
				{
					pLeftOwner->On_Collision_Exit(pRightOwner);
					pRightOwner->On_Collision_Exit(pLeftOwner);
					pLeft->Erase_ColliderObject(pRightOwner);
					pLeft->Erase_ColliderObject(pLeftOwner);
				}
			}


			
		}
	}

	for (auto& pCollider : m_ColliderList)
		Safe_Release(pCollider);
	m_ColliderList.clear();
}

#pragma endregion

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

	for (auto& pCollider : m_ColliderList)
	{
		Safe_Release(pCollider);
	}
	m_ColliderList.clear();

}
