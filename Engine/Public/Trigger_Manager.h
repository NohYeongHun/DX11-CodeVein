#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CTrigger_Manager final : public CBase
{
public:
	
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

#pragma region ENGINE에 제공
public:
	HRESULT Add_GameObject_ToTrigger(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_GameObject_ToObjectLayer(_uint iLayerLevelIndex, const _wstring& strSourTag, const _wstring& strDestTag, _uint iCount);
	HRESULT Add_Trigger(_uint iLayerLevelIndex, const TRIGGER_MONSTER_DESC& triggerDesc);
	HRESULT Trigger_Check(_uint iLayerLevelIndex, class CGameObject* pTarget);
	void Update(_float fTimeDelta);
	void Clear(_uint iLayerLevelIndex);

	void Set_TargetPlayer(class CGameObject* pPlayer) { 
		if (nullptr == pPlayer)
			CRASH("Failed Set TargetPlayer");
		m_pPlayer = pPlayer; 
	};

private:
	class CGameObject* m_pPlayer = { nullptr };
#pragma endregion
	

private:
	class CGameInstance* m_pGameInstance = { nullptr };


#pragma region 게임 시작 시 해당 레벨에 생성한 객체들을 모두 담아둡니다.
private:
	/* Ready_Layer ... 담아둘 객체. */
	map<const _wstring, class CLayer*>* m_pLayers = {};
	// 가장 마지막 객체 부터 조건에 부합하면 실행.
	vector<TRIGGER_MONSTER_DESC>* m_pPhases = {};

	_uint m_iNumLevels = {};

	

private:
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);

#pragma endregion

#pragma region 트리거 완료 추적
private:
	list<class CGameObject*> m_Current_TrackObjects;
	_uint m_iCurrentPhase = {};

private:
	/* 객체들이 삭제되었는지 확인합니다. */
	_bool Check_Destroyed_TrackObjects();
	 void Clear_TrackedObjects();
	_bool Check_Player_ReachedZone(const _float3& vTargetPos, _float fRadius);
#pragma endregion


public:
	static CTrigger_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};
NS_END

