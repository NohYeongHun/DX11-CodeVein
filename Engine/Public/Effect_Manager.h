#pragma once

NS_BEGIN(Engine)

using PoolTable = map<const _wstring, vector<class CGameObject*>>;

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
	HRESULT Move_Effect_ToObjectLayer(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount, _uint iEffectType, void* pArg = nullptr);
	// Pool에 추가
	HRESULT Add_GameObject_ToPools(const _wstring& strDestTag, _uint iEffectType, class CGameObject* pGameObject);
	void	Update();


#pragma region DEBUG 용도 제공 (Effect Tool을 위한..)
public:
	const PoolTable& Export_EditPool(_uint iEffectType);


private:
	class CGameInstance* m_pGameInstance = { nullptr };


#pragma region 게임 시작 시 해당 레벨에 생성한 객체들을 모두 담아둡니다.
private:
	PoolTable* m_pPools = {nullptr }; // 관리하는 주체들은. 
	_uint m_iNumLevels = {}; // 아직 미사용 중
#pragma endregion


#pragma region 헬퍼 함수
private:
	

#pragma endregion


public:
	static CEffect_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};
NS_END

