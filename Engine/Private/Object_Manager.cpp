#include "Object_Manager.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


#pragma region ENGINE에 제공
CComponent* CObject_Manager::Get_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

CLayer* CObject_Manager::Get_Layer(_uint iLayerIndex, const _wstring& strLayerTag)
{
	return Find_Layer(iLayerIndex, strLayerTag);
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLayerIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLayerIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	list<CGameObject*>& gameObjects = pLayer->Get_GameObjects();

	auto iter = gameObjects.begin();

	for (_uint i = 0; i < iIndex; i++)
		iter++;

	return (*iter);
}

/* 읽기 전용 Layer를 내보냅니다. */
const LayerTable& CObject_Manager::Export_EditLayer(_uint iLayerLevelIndex)
{	
	return m_pLayers[iLayerLevelIndex];
}

/* 수정용 이벤트. */
void CObject_Manager::Request_EditObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID, const EditPayload& payload)
{
	switch (payload.type)
	{
	case EEditType::Transform:
		Edit_Transform(iLayerLevelIndex, strLayerTag, objID, payload);
		break;
	default:
		break;
	}
}

/* 삭제용 이벤트. */
void CObject_Manager::Request_DeleteObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		CRASH("Failed Find Layer");
		return;
	}
	
	std::list<CGameObject*>& objList = pLayer->Get_GameObjects();

	// Iter 방식 순회.
	for (auto it = objList.begin(); it != objList.end(); /* no ++ here */)
	{
		CGameObject* pObj = *it;
		if (pObj && pObj->Get_ID() == objID)
		{
			Safe_Release(pObj);        
			it = objList.erase(it); 
			return;                    
		}
		else
		{
			++it;                      // 다음 원소
		}
	}

	// 여기까지 왔다는 건 못 찾은 것
	CRASH("Object ID not found in Layer");

}

#pragma endregion




RAYHIT_DESC CObject_Manager::Get_PickingLocalObject(_uint iLayerLevelIndex, const _wstring strLayerTag, _float* pOutDist)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return {};

	return pLayer->Get_PickingLocalObject(pOutDist);
}

/* 모델 피킹 정보 반환. */
MODEL_PICKING_INFO CObject_Manager::Get_PickingLocalObject(_uint iLayerLevelIndex, const _wstring strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return {};
	return pLayer->Get_PickingLocalObject();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				Pair.second->Priority_Update(fTimeDelta);
				Pair.second->PopDestroyedTo(m_DestroyObjects);
			}
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
	{		
		Safe_Release(Pair.second);
	}

	/* 삭제 컨테이너에 넣어둔 GameObject들을 모두 제거. */
	for (auto& pGaemObject : m_DestroyObjects)
		Safe_Release(pGaemObject);
	m_DestroyObjects.clear();

	m_pLayers[iLevelIndex].clear();
}

CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}

/* 트랜스폼 전용 수정 함수. */
void CObject_Manager::Edit_Transform(_uint iLayerLevelIndex, const _wstring& strLayerTag, uint32_t objID, const EditPayload& payload)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		CRASH("Edit Find Layer Failed");
		return;
	}
	
	CTransform* pTransform = { nullptr };

	const TransformData& data = get<TransformData>(payload.data);

	for (auto& pGameObject : pLayer->Get_GameObjects())
	{
		// 찾았다면?
		if (pGameObject->Get_ID() == objID)
		{
			pTransform = static_cast<CTransform*>(pGameObject->Get_Component(TEXT("Com_Transform")));
			pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&data.pos));

			// 회전한다.
			if (data.rot.x > 0.f)
				pTransform->Add_Rotation(XMConvertToRadians(data.rot.x), 0.f, 0.f);
			else if(data.rot.y > 0.f)
				pTransform->Add_Rotation(0.f, XMConvertToRadians(data.rot.y), 0.f);
			else if (data.rot.z > 0.f)
				pTransform->Add_Rotation(0.f, 0.f, XMConvertToRadians(data.rot.z));
			
			pTransform->Set_Scale(data.scale);

			return;
		}
	}

	
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();	
	}

	for (auto& pGameObject : m_DestroyObjects)
		Safe_Release(pGameObject);

	m_DestroyObjects.clear();

	Safe_Release(m_pGameInstance);

	Safe_Delete_Array(m_pLayers);
	
}
