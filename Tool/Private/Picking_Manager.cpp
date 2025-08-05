#include "Picking_Manager.h"

CPicking_Manager::CPicking_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CPicking_Manager::Clear_PickingMgr()
{
    m_GameObjects.clear();
}

// 1. Layer Tag에 해당하는 피킹 그룹들을 담습니다.
void CPicking_Manager::Load_PickingGroup(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		return;
	}

	// Layer 객체를 담습니다.
	m_GameObjects = pLayer->Get_GameObjects();
}

void CPicking_Manager::Add_PickingGroup(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return;
    m_GameObjects.push_back(pGameObject);
}

void CPicking_Manager::Out_PickingGroup(CGameObject* pGameObject)
{
	auto iter = m_GameObjects.begin();
	while (iter != m_GameObjects.end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects.erase(iter);
		else
			++iter;
	}
}


// 내코드로 고치자.
_bool CPicking_Manager::Picking()
{
	list<CGameObject*>  vecPicked;
	vector<_float3>		vecPos;
	_float3 vPosition;

	
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Picking(&vPosition) == true)
		{
			vecPicked.push_back(pGameObject);
			vecPos.push_back(vPosition);

			vPosition.y += 0.2f;
			m_vPickingPos = vPosition; // 여기가 메시 피킹된 위치?
			
		}
	}

	// 객체 여러개면 => 카메라 기준 가까운 놈으로.
	if (!vecPicked.empty())
	{
		vecPicked.sort([](CGameObject* pSour, CGameObject* pDest)
			{
				return pSour->Get_CamDistance() < pDest->Get_CamDistance();
			});

		// 가장 가까운 객체로 선택.
		m_pPickedObj = vecPicked.front();
		return true;
	}
	return false;
}

CPicking_Manager* CPicking_Manager::Create()
{
    return new CPicking_Manager();
}

void CPicking_Manager::Free()
{
	__super::Free();
	m_GameObjects.clear();
	Safe_Release(m_pGameInstance);
}
