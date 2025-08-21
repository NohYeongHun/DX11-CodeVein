#include "Collider_Manager.h"

CCollider_Manager::CCollider_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	m_RegisterPools = new vector<CCollider*>[iNumLevels];
	m_freePools.resize(iNumLevels);
	return S_OK;
}

#pragma region ENGINE에 제공
HRESULT CCollider_Manager::Add_Collider_To_Manager(CCollider* pCollider, _uint iLevelIndex)
{
	if (nullptr == pCollider || iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	m_RegisterPools[iLevelIndex].emplace_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

HRESULT CCollider_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& pCollider : m_RegisterPools[iLevelIndex])
	{
		Safe_Release(pCollider);
	}

	m_RegisterPools[iLevelIndex].clear();
	m_freePools[iLevelIndex].clear();

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

	m_ActiveColliders.clear();
}

/* 죽은 것들을 솎아낸다. */
void CCollider_Manager::Collision_CleanUp()
{
	for (_uint levelIndex = 0; levelIndex < m_iNumLevels; ++levelIndex)
	{
		m_freePools[levelIndex].clear();

		for (_uint i = 0; i < m_RegisterPools[levelIndex].size(); ++i)
		{
			// Collider Owner가 삭제되어야 하는 상황이라면?
			if (m_RegisterPools[levelIndex][i]->Get_Owner()->Is_Destroy())
				m_freePools[levelIndex].emplace_back(i);
		}

		// 인덱스 정리: 중복 제거 + 내림차순 정렬
		sort(m_freePools[levelIndex].begin(), m_freePools[levelIndex].end());
		m_freePools[levelIndex].erase(std::unique(m_freePools[levelIndex].begin(), m_freePools[levelIndex].end()), m_freePools[levelIndex].end());
		sort(m_freePools[levelIndex].begin(), m_freePools[levelIndex].end(), std::greater<_uint>());

		Remove_Collider(levelIndex);
		m_freePools[levelIndex].clear();
	}
}


#pragma region 1. 충돌 작업 세분화

// 1. 충돌 가능성 있는 것 선별
void CCollider_Manager::Build_BroadPhase()
{
	CCollider* pLeft = { nullptr };
	CCollider* pRight = { nullptr };
	CGameObject* pLeftOwner = { nullptr };
	CGameObject* pRightOwner = { nullptr };

	// 현재 레벨의 콜라이더들만 검사
	_uint currentLevel = m_pGameInstance->Get_CurrentLevelID();
	if (currentLevel >= m_iNumLevels || m_RegisterPools[currentLevel].size() == 0)
		return;

	for (_uint i = 0; i < m_RegisterPools[currentLevel].size() - 1; ++i)
	{
		for (_uint j = i + 1; j < m_RegisterPools[currentLevel].size(); ++j)
		{
			pLeft = m_RegisterPools[currentLevel][i];
			pRight = m_RegisterPools[currentLevel][j];
			 
			// 1. null pointer인 객체가 등록되었으므로 Crash
			if (nullptr == pLeft || nullptr == pRight)
			{
				CRASH("Failed Search Collider");
				return;
			}

			pLeftOwner = pLeft->Get_Owner();
			pRightOwner = pRight->Get_Owner();

			//2. 같은 객체인지 체크 (동일 GameObject의 Collider끼리 충돌 방지)
			if (pLeftOwner == pRightOwner)
				continue;

			// 3. 같은 Collider인지 체크 (혹시 모를 상황 대비)
			if (pLeft == pRight)
				continue;

			// 4. Target Layer가 아닌 경우 무시.
			if (!pLeft->Has_TargetLayer(pRight))
				continue;

			// 5. World AABB Bounding과 Collider를 충돌 비교해서 담을 객체인지 확인. 
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
		pRightOwner = pRight->Get_Owner();

		if (pLeftOwner == pRightOwner || pLeft == pRight)
			continue;

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
HRESULT CCollider_Manager::Remove_Collider(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels || m_freePools[iLevelIndex].size() == 0)
		return S_OK;
		
	for (_uint idxToRemove : m_freePools[iLevelIndex])
	{
		_uint last = (_uint)m_RegisterPools[iLevelIndex].size() - 1;
		if (idxToRemove > last) continue; 

		// (마지막이면 스왑 불필요)
		if (idxToRemove == last)
		{
			CCollider* dead = m_RegisterPools[iLevelIndex][last];
			Safe_Release(dead);
			m_RegisterPools[iLevelIndex].pop_back();
			continue;
		}

		// swap-and-pop
		CCollider* dead = m_RegisterPools[iLevelIndex][idxToRemove];
		CCollider* tail = m_RegisterPools[iLevelIndex][last];

		swap(m_RegisterPools[iLevelIndex][idxToRemove], m_RegisterPools[iLevelIndex][last]);
		Safe_Release(dead);
		m_RegisterPools[iLevelIndex].pop_back();
	}

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
		Handle_SlidingVector(pLeft, pRight, pLeftOwner, pRightOwner);
	}

}

// 충돌 했을때 BODY 타입인것들 끼리 Sliding Vector 계산
void CCollider_Manager::Handle_SlidingVector(CCollider* pLeft, CCollider* pRight, CGameObject* pLeftOwner, CGameObject* pRightOwner)
{
	// 1. 충돌 방향 벡터 계산
	_vector vCollisionNormal = Calculate_ColliderNormal(pLeft, pRight);

	// 2. 각 객체의 이동 벡터 계산 => 이전 프레임 위치를 Transform에서 계산해서 가지고 있습니다.
	_vector vRightVelocity = pRightOwner->Get_Transform()->Get_Velocity();
	_vector vLeftVelocity = pLeftOwner->Get_Transform()->Get_Velocity();

	// 3. 관통 깊이 계산 => 구 고정. Body 타입은 무조건 구로만 생성.
	_float fPenetrationValue = Calculate_PenetrationDepthSpehre(pLeft, pRight);

	// 4. 분리 벡터 계산
	_vector vSeparationVector = vCollisionNormal * fPenetrationValue;
	_vector vLeftSepartation = vSeparationVector * (- 1.f);
	_vector vRightSepartation = vSeparationVector * 0.f;

	// 5. 위치 보정 적용 => 적용시 Naviagtion 고려.
	pLeftOwner->Get_Transform()->Translate(vLeftSepartation
		, pLeftOwner->Get_Navigation());
	pRightOwner->Get_Transform()->Translate(vRightSepartation
		, pRightOwner->Get_Navigation());

	// 6. 슬라이딩 벡터 계산.
	_vector vSlidingVector = Calculate_SlidingVector(vLeftVelocity, vCollisionNormal);

	_float fSlidingSpeed = XMVectorGetX(XMVector3Length(vLeftVelocity)) * 0.4f; // 원래 속도의 40%
	_vector vSlidingMovement = vSlidingVector * fSlidingSpeed;

	// 7. 슬라이딩 이동 적용 (벽을 따라 미끄러지기)
	pLeftOwner->Get_Transform()->Translate(vSlidingMovement, pLeftOwner->Get_Navigation());
	
}

// 1. 충돌 방향 벡터 계산 => vLeft -> vRight 방향.
_vector CCollider_Manager::Calculate_ColliderNormal(CCollider* pLeft, CCollider* pRight)
{
	_float3 vLeftPos = pLeft->Get_Center();
	_float3 vRightPos = pRight->Get_Center();

	// 1. pLeft -> pRight 방향을 구합니다.
	_vector vDirection = XMLoadFloat3(&vRightPos) - XMLoadFloat3(&vLeftPos);

	return XMVector3Normalize(vDirection);
}

_vector CCollider_Manager::Calculate_SlidingVector(_vector vInputDirection, _vector vCollisionNormal)
{
	// 입력 방향에서 충돌 법선 성분을 제거 == 슬라이딩 벡터
	// 플레이어 입력 방향에서 벽에 박히는 벡터를 빼주면 그것이 벽을 타고 가는 Sliding Vector이다.
	_float fDotProduct = XMVectorGetX(XMVector3Dot(vInputDirection, vCollisionNormal));
	_vector vNormalComponent = vCollisionNormal * fDotProduct;
	_vector vSlidingVector = vInputDirection - vNormalComponent;

	return XMVector3Normalize(vSlidingVector);
}


// 3. 콜라이더 간 관통 깊이 계산.
_float CCollider_Manager::Calculate_PenetrationDepthSpehre(CCollider* pLeft, CCollider* pRight)
{
	return pLeft->Calculate_PenetrationDepthSpehre(pRight);
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
	CBase::Free();
	Safe_Release(m_pGameInstance);

	if (nullptr != m_RegisterPools)
	{
		for (_uint i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& pCollider : m_RegisterPools[i])
			{
				Safe_Release(pCollider);
			}
			m_RegisterPools[i].clear();
		}
		
		Safe_Delete_Array(m_RegisterPools);
	}

	m_freePools.clear();
}
