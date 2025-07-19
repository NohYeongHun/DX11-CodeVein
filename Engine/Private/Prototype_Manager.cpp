#include "Prototype_Manager.h"

CPrototype_Manager::CPrototype_Manager()	
{
	
}

/* PrototyeName 들을 List에 Add한다. */
void CPrototype_Manager::Add_Prototype_To_List(list<_wstring>& outList, _uint iLevelIndex, list<_wstring>& nameList)
{
	if (nullptr == m_pPrototypes || iLevelIndex >= m_iNumLevels)
		return;

	for (auto& name : nameList)
	{
		for (auto& Pair : m_pPrototypes[iLevelIndex])
		{
			_wstring strPrototypeTag = Pair.first;

			if (strPrototypeTag == name)
				outList.push_back(Pair.first);
		}
	}
	
}

void CPrototype_Manager::Get_PrototypeName_List(list<_wstring>& outList, _uint iLevelIndex, const _tchar* pPrefix)
{
	if (nullptr == m_pPrototypes || iLevelIndex >= m_iNumLevels)
		return;

	_wstring prefix = pPrefix;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
	{
		_wstring strPrototypeTag = Pair.first;
		if (strPrototypeTag.compare(0, prefix.length(), prefix) == 0)
			outList.push_back(Pair.first);
			
	}

}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == m_pPrototypes || 
		m_iNumLevels <= iPrototpyeLevelIndex ||
		nullptr != Find_Prototype(iPrototpyeLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iPrototpyeLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CBase* pPrototype = Find_Prototype(iPrototypeLevelIndex, strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CBase* pGameObject = { nullptr };

	if (PROTOTYPE::GAMEOBJECT == ePrototype)
		pGameObject = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	else
		pGameObject = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}


CBase* CPrototype_Manager::Find_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag)
{
	/* 이진탐색 */
	auto	iter = m_pPrototypes[iPrototypeLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iPrototypeLevelIndex].end())
		return nullptr;

	return iter->second;	
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CPrototype_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);	
}
