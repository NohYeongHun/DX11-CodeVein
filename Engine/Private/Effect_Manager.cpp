CEffect_Manager::CEffect_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	m_pPools = new PoolTable[ENUM_CLASS(EFFECTTYPE::END)];

	return S_OK;
}

#pragma region ENGINE에 제공
/* SourTag => EffectManager에서 소유한 String 이름 */
HRESULT CEffect_Manager::Move_GameObject_ToObjectLayer(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount, _uint iEffectType, void* pArg)
{
	if (iCount > m_pPools[iEffectType][strSourTag].size())
	{
		CRASH("Failed Crash Enough Pool Size");
		return E_FAIL;
	}
		

	for (_uint i = 0; i < iCount; ++i)
	{
		// 1. 객체 포인터를 가져오고
		CGameObject* pGameObject = m_pPools[iEffectType][strSourTag].back();
		// 2. 제거합니다.
		m_pPools[iEffectType][strSourTag].pop_back();

		// 3. OnActivate를 통해 활성화 상태를 만들어 줍니다.
		pGameObject->OnActivate(pArg);

		// 4. Layer에 추가합니다.
		m_pGameInstance->Add_GameObject_ToLayer(iLayerLevelIndex, strDestTag, pGameObject);
	}
	

	return S_OK;
}


HRESULT CEffect_Manager::Add_GameObject_ToPools(const _wstring& strDestTag, _uint iEffectType, CGameObject* pGameObject)
{
	// 1. 객체를 Pool에 넣어줍니다.
	m_pPools[iEffectType][strDestTag].emplace_back(pGameObject);
	return S_OK;
}

void CEffect_Manager::Update()
{
}

const PoolTable& CEffect_Manager::Export_EditPool(_uint iEffectType)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_pPools[iEffectType];
}
#pragma endregion







CEffect_Manager* CEffect_Manager::Create(_uint iNumLevels)
{
	CEffect_Manager* pInstance = new CEffect_Manager();
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Create Failed CEffect_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Manager::Free()
{
	CBase::Free();

	// 2. Pool 객체 제거.
	for (_uint i = 0; i < ENUM_CLASS(EFFECTTYPE::END); ++i)
	{
		for (auto& Pair : m_pPools[i])
		{
			for (auto& pGameObject : Pair.second)
				Safe_Release(pGameObject);
		}

		m_pPools[i].clear();
	}

	Safe_Delete_Array(m_pPools);
	Safe_Release(m_pGameInstance);
	
	//Safe_Delete_Array(m_pPools);
}
