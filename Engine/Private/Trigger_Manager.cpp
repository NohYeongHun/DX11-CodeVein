#include "Trigger_Manager.h"

CTrigger_Manager::CTrigger_Manager()
    : m_pGameInstance {CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTrigger_Manager::Initialize(_uint iNumLevels)
{
	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];
	m_pPhases = new vector<TRIGGER_MONSTER_DESC>[iNumLevels];
	m_iNumLevels = iNumLevels;

	return S_OK;
}

#pragma region ENGINE에 제공.
HRESULT CTrigger_Manager::Add_GameObject_ToTrigger(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
	{
		CRASH("Failed GameObject Add To Trigger");
		return E_FAIL;
	}

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

/* Trigger Manager Layer안에 있는 GameObject를 Layer에 추가. */
HRESULT CTrigger_Manager::Add_GameObject_ToObjectLayer(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount)
{
	// 0. Layer 탐색
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strSourTag);
	if (nullptr == pLayer)
	{
		CRASH("Failed Find Layer Trigger");
		return E_FAIL;
	}

	// 1. Layer로부터 객체 리스트 참조로 가져오기.
	list<CGameObject*>& gameObjects = pLayer->Get_GameObjects();
	
	
	// 2. Layer에서 객체들을 빼서. Object Manager에 전달
	auto iter = gameObjects.begin();
	for (_uint i = 0; i < iCount; ++i)
	{
		// 3. 전달할 객체들은 추적해야하므로 list에 추가.
		m_Current_TrackObjects.emplace_back(*iter);
		Safe_AddRef(*iter);


		m_pGameInstance->Add_GameObject_ToLayer(iLayerLevelIndex, strDestTag, *iter);
		iter = gameObjects.erase(iter);
	}

	return S_OK;
}


// 가장 먼저 실행할 것들을 나중에 추가합니다. => 레벨의 마지막 부분에 추가 해줍니다.
HRESULT CTrigger_Manager::Add_Trigger(_uint iLayerLevelIndex, const TRIGGER_MONSTER_DESC& triggerDesc)
{
	m_pPhases[iLayerLevelIndex].emplace_back(triggerDesc);
	return S_OK;
}

/* 매프레임 트리거 체크 .*/
HRESULT CTrigger_Manager::Trigger_Check(_uint iLayerLevelIndex, CGameObject* pTarget)
{
	if (nullptr == pTarget ||
		m_pPhases[iLayerLevelIndex].size() == 0 || 
		m_iCurrentPhase >= m_pPhases[iLayerLevelIndex].size())
		return E_FAIL;
	

	const TRIGGER_MONSTER_DESC& triggerDesc = m_pPhases[iLayerLevelIndex][m_iCurrentPhase];

	// 1. 완료 조건 체크 먼저 수행
	_bool IsTrackObjectDestroy = Check_Destroyed_TrackObjects();
	_bool IsPlayerReachedZone = Check_Player_ReachedZone(triggerDesc.vTriggerPos, triggerDesc.fRadius);
	
	if (IsTrackObjectDestroy && IsPlayerReachedZone)
	{
		// 2. 현재 추적중인 객체들 제거.
		Clear_TrackedObjects();

		// 3. 객체들을 Object Manager에 추가.
		Add_GameObject_ToObjectLayer(iLayerLevelIndex, triggerDesc.strTriggerLayer, triggerDesc.strObjectLayer, triggerDesc.iCount);
		++m_iCurrentPhase;
	}
		
	return S_OK;
}

/* 매 프레임 트리거 체크. */
void CTrigger_Manager::Update(_float fTimeDelta)
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelID();
	Trigger_Check(iLevelIndex,
		m_pPlayer);
}

void CTrigger_Manager::Clear(_uint iLayerLevelIndex)
{
	if (iLayerLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLayerLevelIndex])
		Safe_Release(Pair.second);

	Clear_TrackedObjects();
	m_pLayers[iLayerLevelIndex].clear();
	m_pPhases[iLayerLevelIndex].clear();
	m_pPlayer = nullptr;
	m_iCurrentPhase = 0;
}

#pragma endregion



#pragma region 객체를 찾는 함수.
CLayer* CTrigger_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	// TODO: 여기에 return 문을 삽입합니다.
	auto iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}
#pragma endregion




CTrigger_Manager* CTrigger_Manager::Create(_uint iNumLevels)
{
	CTrigger_Manager* pInstance = new CTrigger_Manager();
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Create Failed Trigger Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

#pragma region Trigger 완료 추적

/* 처음에는 list에 아무런 객체들이 없을 것이므로 true 반환됨. */
_bool CTrigger_Manager::Check_Destroyed_TrackObjects()
{
	_bool isDestroyed = true;
		
	/* 추적 중인 객체들이 제거되지 않았다면 */
	for (auto& pGameObject : m_Current_TrackObjects)
	{
		if (!pGameObject->Is_Destroy())
			isDestroyed = false;
	}

	return isDestroyed;
}

/* 객체들 추적 제거. */
void CTrigger_Manager::Clear_TrackedObjects()
{
	for (auto& pGameObject : m_Current_TrackObjects)
		Safe_Release(pGameObject);

	m_Current_TrackObjects.clear();
}


_bool CTrigger_Manager::Check_Player_ReachedZone(const _float3& vTriggerPos, _float fRadius)
{
	_vector vTargetPosition = XMLoadFloat3(&vTriggerPos);
	_vector vPlayerPosition = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

	_vector vDiff = vPlayerPosition - vTargetPosition;
	vDiff = XMVectorSetY(vDiff, 0.f); // Y값 무시

	_float fDistance = XMVectorGetX(XMVector3Length(vDiff));

	return fDistance <= fRadius;
}
#pragma endregion



#pragma region 트리거 완료 조건 체크 함수들

#pragma endregion

void CTrigger_Manager::Free()
{
    CBase::Free();

	for (_uint i = 0; i < m_iNumLevels; ++i)
		Clear(i);

	for (_uint i = 0; i < m_iNumLevels; ++i)
		m_pPhases[i].clear();

	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_pLayers);
	Safe_Delete_Array(m_pPhases);
}
