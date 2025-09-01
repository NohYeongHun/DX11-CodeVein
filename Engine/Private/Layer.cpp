#include "Layer.h"

CLayer::CLayer()
{

}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;
	
	return (*iter)->Get_Component(strComponentTag);
}

RAYHIT_DESC CLayer::Get_PickingLocalObject(_float* pOutDist)
{
	RAYHIT_DESC Desc{};
	_float fBestDist = FLT_MAX;
	_float fHitDist = 0.f;

	_float3 pOutLocalPos = {};
	_float3 pOutLocalNormal = {};
	static int iGameObjectID = 0;

	for (auto& pGameObject : m_GameObjects)
	{
		
		// 가장 먼저 Ray에 맞은 객체를 반환. => 이렇게 하면 안됨..
		// => 가장 가깝게 Ray Picking된 객체를 반환해야됨.

		iGameObjectID++;
		if (pGameObject->Is_Ray_LocalHit(&pOutLocalPos, &pOutLocalNormal, pOutDist))
		{
			fHitDist = *pOutDist;

			if (fHitDist < fBestDist)
			{
				fBestDist = fHitDist;
				CTransform* pTransform = static_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
				Desc.fDistance = *pOutDist;
				Desc.pHitObject = pGameObject;
				Desc.vHitLocal = pOutLocalPos;
				Desc.vHitNormal = pOutLocalNormal;
				XMStoreFloat3(&Desc.vHitPoint, pTransform->Get_State(STATE::POSITION));
				//Desc.vHitNormal = { 0.f, 1.f, 0.f }; // 그냥 무조건 위로 1.f 띄워서 반환.
			}
		}
			
	}

	iGameObjectID = 0;

	return Desc;
}

MODEL_PICKING_INFO CLayer::Get_PickingLocalObject()
{
	MODEL_PICKING_INFO Info{};

	_float fBestDist = FLT_MAX;
	_float fHitDist = 0.f;

	_float3 pOutLocalPos = {};
	_float3 pOutLocalNormal = {};
	static int iGameObjectID = 0;

	for (auto& pGameObject : m_GameObjects)
	{

		// 가장 먼저 Ray에 맞은 객체를 반환. => 이렇게 하면 안됨..
		// => 가장 가깝게 Ray Picking된 객체를 반환해야됨.

		iGameObjectID++;
		if (pGameObject->Is_Ray_LocalHit(&Info, &fHitDist))
		{
			if (fHitDist < fBestDist)
			{
				fBestDist = fHitDist;

				CTransform* pTransform = pGameObject->Get_Transform();
				Info.bHit = true;
				Info.fDistance = fHitDist;
				Info.pHitObject = pGameObject;

				// World 좌표로 지점 저장.
				XMStoreFloat3(&Info.vHitWorldPoint
					, XMVector3TransformCoord(
						XMLoadFloat3(&Info.vHitPoint), pTransform->Get_WorldMatrix()));
			}
		}

	}

	iGameObjectID = 0;
	return Info;
}


void CLayer::Priority_Update(_float fTimeDelta)
{
	/* Destroy 객체들을 오브젝트 매니저 풀에 넣을라면? */
	auto iter = m_GameObjects.begin();
	while (iter != m_GameObjects.end())
	{
		if ((*iter)->Is_Destroy())
		{
			(*iter)->Destroy();
			m_Destroyed.emplace_back(*iter);

			//Safe_Release(*iter); // 이걸 안하는 건데..
			iter = m_GameObjects.erase(iter);
		}
		else if ((*iter)->Is_DeActivate())
		{
			// 풀로 뺍니다.
			(*iter)->OnDeActivate();
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}

	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Priority_Update(fTimeDelta);
		}
	}

}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Update(fTimeDelta);
	}
}

/* 추가된 size만 반환. */
size_t CLayer::PopDestroyedTo(std::list<CGameObject*>& out)
{
	size_t n = m_Destroyed.size();
	if (n) {
		out.splice(out.end(), m_Destroyed); // m_Destroyed -> out (move)
	}
	return n;
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	CBase::Free();

	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->Destroy(); // 해제 전에 해결해야할 제거 작업을 미리 정의?
		Safe_Release(pGameObject);
	}
		

	m_GameObjects.clear();
	
}
