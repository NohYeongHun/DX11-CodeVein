#pragma once

NS_BEGIN(Engine)
class CEffect_Manager final : public CBase
{
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

#pragma region ENGINE에 제공
public:
	// Pool -> ObjectManager
	HRESULT Move_GameObject_ToObjectLayer(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount, void* pArg = nullptr);
	// Object Manager -> Pool
	//HRESULT Move_GameObject_ToPools(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount, void* pArg = nullptr);

	// Pool에 추가
	HRESULT Add_GameObject_ToPools(const _wstring& strDestTag, class CGameObject* pGameObject);
	//void	Clear(_uint iLayerLevelIndex);
	void	Update();
#pragma endregion


private:
	class CGameInstance* m_pGameInstance = { nullptr };


#pragma region 게임 시작 시 해당 레벨에 생성한 객체들을 모두 담아둡니다.
private:
	map<const _wstring, vector<class CGameObject*>> m_Pools = {};
	_uint m_iNumLevels = {};

#pragma endregion


#pragma region 헬퍼 함수
private:
	

#pragma endregion


public:
	static CEffect_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};
NS_END

