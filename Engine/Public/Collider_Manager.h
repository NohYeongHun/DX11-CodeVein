#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CCollider_Manager final : public CBase
{
private:
	explicit CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

#pragma region ENGINE에 제공
public:
	HRESULT Add_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider);
	HRESULT Remove_Collider_To_Layer(COLLIDERLAYER eColliderLayer, CCollider* pCollider);

public:
	void Priority_Update(); // list에서 소멸.
	void Update();			// N
	void Late_Update();
#pragma endregion



private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = { };
	// 1 Phase 최적화용 list에 담아두기.
	list<pair<CCollider*, CCollider*>> m_BroadList = {};

	typedef list<CCollider::COLLIDER_INFO> COLLIDERTYPES;

	unordered_map<COLLIDERLAYER, COLLIDERTYPES> m_Colliders = {};
	//map<, COLLIDERTYPES> m_Colliders = { nullptr };




public:
	static CCollider_Manager* Create(_uint iNumLevels);
	virtual void Free();

private:
};
NS_END

