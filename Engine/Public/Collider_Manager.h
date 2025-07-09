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
	HRESULT Add_Collider_To_Manager(CCollider* pCollider);

public:
	void Update();
#pragma endregion



private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = { };

	// 최적화는 각 객체들이 업데이트나 Render에서 조건에 따라 Collider를 안넣으면 되려나?
	list<CCollider*> m_ColliderList = {};


public:
	static CCollider_Manager* Create(_uint iNumLevels);
	virtual void Free();

private:
};
NS_END

