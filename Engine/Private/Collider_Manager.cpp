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

	m_RegisterPool.emplace_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

// 매 프레임 추가하고 => 충돌 검사 후에 => 매 프레임 제거합니다.
void CCollider_Manager::Update()
{

	//OutputDebugWstring(TEXT("콜라이더 등록 개수 : "));
	//OutPutDebugInt(m_ColliderList.size());
	

	// 1. 충돌 가능성이 있는 후보 쌍들을 추출.
	Build_BroadPhase();

	// 2. 실제 충돌 처리.
	Narrow_Phase();


	for (auto& pLeft : m_ColliderList)
	{
		for (auto& pRight : m_ColliderList)
		{
			// 1. 예외처리. => 같은 경우 무시하기.
			if (pLeft == pRight || nullptr == pLeft || nullptr == pRight)
				continue;

			CGameObject* pLeftOwner = pLeft->Get_Owner();
			CGameObject* pRightOwner = pRight->Get_Owner();

			// 2. Owner가 없는 경우
			if (nullptr == pLeftOwner || nullptr == pRightOwner)
				continue;

			// 3. 타겟 Layer가 아닌 경우
			if (!pLeft->Has_TargetLayer(pRight))
				continue;


			// 1. 만약 Active 되어 있지 않은 Collider 라면?
			if (!pLeft->Is_Active() || !pRight->Is_Active())
			{
				Handle_Collision_Exit(pLeft, pRight, pLeftOwner, pRightOwner);
				continue;
			}

			// 2. 둘다 충돌이 성공했다면?
			if (pLeft->Intersect(pRight) && pRight->Intersect(pLeft))
			{
				Handle_Collision_By_Type(pLeft, pRight, pLeftOwner, pRightOwner);
			}
			// 3. 둘중 하나라도 충돌 실패했다면?
			else
			{
				Handle_Collision_Exit(pLeft, pRight, pLeftOwner, pRightOwner);
			}
			
		}
	}

	m_ActiveColliders.clear();
	/*for (auto& pCollider : m_ColliderList)
		Safe_Release(pCollider);
	m_ColliderList.clear();*/
}

/* 죽은 것들을 솎아낸다. */
void CCollider_Manager::Collision_CleanUp()
{
	m_freePool.clear();

	for (_uint i = 0; i < m_RegisterPool.size(); ++i)
	{
		// Collider Owner가 삭제되어야 하는 상황이라면?
		if (m_RegisterPool[i]->Get_Owner()->Is_Destroy())
			m_freePool.emplace_back(i);
	}

	// 인덱스 정리: 중복 제거 + 내림차순 정렬
	sort(m_freePool.begin(), m_freePool.end());
	m_freePool.erase(std::unique(m_freePool.begin(), m_freePool.end()), m_freePool.end()); // 4) 방어적
	sort(m_freePool.begin(), m_freePool.end(), std::greater<_uint>());

	Remove_Collider();
	m_freePool.clear();
	
}


#pragma region 1. 충돌 작업 세분화

// 1. 충돌 가능성 있는 것 선별
void CCollider_Manager::Build_BroadPhase()
{
	if (m_RegisterPool.size() == 0)
		return;

	CCollider* pLeft = { nullptr };
	CCollider* pRight = { nullptr };
	CGameObject* pLeftOwner = { nullptr };
	CGameObject* pRightOwner = { nullptr };

	for (_uint i = 0; i < m_RegisterPool.size() - 1; ++i)
	{
		for (_uint j = i + 1; j < m_RegisterPool.size(); ++j)
		{
			pLeft = m_RegisterPool[i];
			pRight = m_RegisterPool[j];
			 
			// 1. null pointer인 객체가 등록되었으므로 Crash
			if (nullptr == pLeft || nullptr == pRight)
			{
				CRASH("Failed Search Collider");
				return;
			}

			pLeftOwner = pLeft->Get_Owner();
			pRightOwner = pRight->Get_Owner();

			// 2. Target Layer가 아닌 경우 무시.
			if (!pLeft->Has_TargetLayer(pRight))
				continue;

			// 3. World AABB Bounding과 Collider를 충돌 비교해서 담을 객체인지 확인. 
			if (pLeft->BroadIntersect(pRight))
			{
				m_ActiveColliders.emplace(pLeft, pRight);
			}
		}
	}
}

void CCollider_Manager::Narrow_Phase()
{
	CCollider* pLeft = { nullptr };
	CCollider* pRight = { nullptr };
	CGameObject* pLeftOwner = { nullptr };
	CGameObject* pRightOwner = { nullptr };
	// 1. Active Collider가 있는 경우 충돌 비교합니다.
	for (auto& pair : m_ActiveColliders)
	{
		pLeft = pair.first;
		pRight = pair.second;

		pLeftOwner = pLeft->Get_Owner();
		pRightOwner = pLeft->Get_Owner();
		if (!pLeft->Is_Active() || !pRight->Is_Active())
		{
			Handle_Collision_Exit(pLeft, pRight, pLeftOwner, pRightOwner);
			continue;
		}

		if (pLeft->Intersect(pRight) && pRight->Intersect(pLeft))
			Handle_Collision_By_Type(pLeft, pRight, pLeftOwner, pRightOwner);
		else
			Handle_Collision_Exit(pLeft, pRight, pLeftOwner, pRightOwner);
	}
}

/* 제거 하면 인덱스에 변화가 생기잖아? */
HRESULT CCollider_Manager::Remove_Collider()
{
	if (m_freePool.size() == 0)
		return S_OK;
		
	for (_uint idxToRemove : m_freePool)
	{
		_uint last = (_uint)m_RegisterPool.size() - 1;
		if (idxToRemove > last) continue; 

		// (마지막이면 스왑 불필요)
		if (idxToRemove == last)
		{
			CCollider* dead = m_RegisterPool[last];
			Safe_Release(dead);
			m_RegisterPool.pop_back();
			continue;
		}

		// swap-and-pop
		CCollider* dead = m_RegisterPool[idxToRemove];
		CCollider* tail = m_RegisterPool[last];

		swap(m_RegisterPool[idxToRemove], m_RegisterPool[last]);
		Safe_Release(dead);
		m_RegisterPool.pop_back();
	}

	m_freePool.clear();
	return S_OK;
}

void CCollider_Manager::Handle_Collision_Exit(CCollider* pLeft, CCollider* pRight, CGameObject* pLeftOwner, CGameObject* pRightOwner)
{
	// pLeft가 RightOwner를 소유하고 있다면? => 충돌을 기억하고 있다면
	if (pLeft->Find_ColliderObject(pRightOwner))
	{
		pLeftOwner->On_Collision_Exit(pRightOwner);
		pRightOwner->On_Collision_Exit(pLeftOwner);
		pLeft->Erase_ColliderObject(pRightOwner);
		pRight->Erase_ColliderObject(pLeftOwner);
	}
}

/* 충돌 타입에 따른 분기. => Trigger냐 */
void CCollider_Manager::Handle_Collision_By_Type(CCollider* pLeft, CCollider* pRight, CGameObject* pLeftOwner, CGameObject* pRightOwner)
{
	// 2-1. 충돌했는데 둘 다 list에 충돌 목록이 없다면?
	if (!pLeft->Find_ColliderObject(pRightOwner) && !pRight->Find_ColliderObject(pLeftOwner))
	{
		pLeftOwner->On_Collision_Enter(pRightOwner);
		pRightOwner->On_Collision_Enter(pLeftOwner);
		pLeft->Insert_ColliderObject(pRightOwner);
		pRight->Insert_ColliderObject(pLeftOwner);
	}
	// 2-2. 충돌 했는데 둘다 list에 충돌 목록이 있다면?
	else if (pLeft->Find_ColliderObject(pRightOwner) && pRight->Find_ColliderObject(pLeftOwner))
	{
		pLeftOwner->On_Collision_Stay(pRightOwner);
		pRightOwner->On_Collision_Stay(pLeftOwner);
	}
	
	// 3. 충돌 했는데 타입이 둘다 BODY라면? Sliding Vector 처리. 
	// 같은 충돌 쌍은 없으므로 Left -> Right 기준으로 처리.
	if (pLeft->Get_Collision_Type() == CCollider::COLLISION_BODY &&
		pRight->Get_Collision_Type() == CCollider::COLLISION_BODY)
	{

	}

}
#pragma endregion



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

	for (auto& pCollider : m_RegisterPool)
	{
		Safe_Release(pCollider);
	}
		

	m_RegisterPool.clear();

}
