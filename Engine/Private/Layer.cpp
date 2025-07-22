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

	static int iGameObjectID = 0;

	for (auto& pGameObject : m_GameObjects)
	{
		
		// 가장 먼저 Ray에 맞은 객체를 반환. => 이렇게 하면 안됨..
		// => 가장 가깝게 Ray Picking된 객체를 반환해야됨.

		iGameObjectID++;
		if (pGameObject->Is_Ray_LocalHit(pOutDist))
		{
			fHitDist = *pOutDist;

			if (fHitDist < fBestDist)
			{
				fBestDist = fHitDist;
				CTransform* pTransform = static_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
				Desc.fDistance = *pOutDist;
				Desc.pHitObject = pGameObject;
				XMStoreFloat3(&Desc.vHitPoint, pTransform->Get_State(STATE::POSITION));
				Desc.vHitNormal = { 0.f, 1.f, 0.f }; // 그냥 무조건 위로 1.f 띄워서 반환.
			}
		}
			
	}

	iGameObjectID = 0;

	return Desc;
}


void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
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

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->Destroy(); // 해제 전에 해결해야할 제거 작업을 미리 정의?
		Safe_Release(pGameObject);
	}
		

	m_GameObjects.clear();
	
}
