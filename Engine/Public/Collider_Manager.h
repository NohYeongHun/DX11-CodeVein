#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CCollider_Manager final : public CBase
{
public:

	/* pair함수를 set에 넣기 위한 Hash 함수 */
	struct ColliderPairHash {
		size_t operator()(const std::pair<CCollider*, CCollider*>& p) const noexcept {
			return hash<CCollider*>{}(p.first) ^ 
			(hash<CCollider*>{}(p.second) << 1); // << 1은 비트 셔플. => 충돌확률 감소.
		}
	};

	/* pair에는 같다 라는 의미가 없으므로 해당 의미를 정의해주어야함. */
	struct ColliderPairEqual {
		bool operator()(const pair<CCollider*, CCollider*>& a,
			const pair<CCollider*, CCollider*>& b) const noexcept {
			return a.first == b.first && a.second == b.second;
		}
	};

private:
	explicit CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

#pragma region ENGINE에 제공
public:
	HRESULT Add_Collider_To_Manager(CCollider* pCollider, _uint iLevelIndex);
	HRESULT Clear(_uint iLevelIndex);

public:
	void Update();
	void Collision_CleanUp(); // 충돌 이후에 죽은 객체들 솎아내기.
#pragma endregion

	

#pragma region 1. 충돌 작업 세분화
private:
	void Build_BroadPhase();
	void Narrow_Phase();

private:
	HRESULT Remove_Collider(_uint iLevelIndex);

private:
	void Handle_Collision_Exit(class CCollider* pLeft, class CCollider* pRight, class CGameObject* pLeftOwner, class CGameObject* pRightOwner);
	void Handle_Collision_By_Type(class CCollider* pLeft, class CCollider* pRight, class CGameObject* pLeftOwner, class CGameObject* pRightOwner);

private:
	void Handle_SlidingVector(class CCollider* pLeft, class CCollider* pRight, class CGameObject* pLeftOwner, class CGameObject* pRightOwner);

private:
	_vector Calculate_ColliderNormal(class CCollider* pLeft, class CCollider* pRight);
	_vector Calculate_SlidingVector(_vector vInputDirection, _vector vCollisionNormal);
	_float Calculate_PenetrationDepthSpehre(class CCollider* pLeft, class CCollider* pRight);
#pragma endregion


#pragma region 0. 충돌 관리용 멤버 변수

private:
	// 레벨별 등록될 콜라이더
	vector<CCollider*>* m_RegisterPools;
	// 레벨별 삭제할 인덱스를 담는 Pool
	vector<vector<_uint>> m_freePools;

	// 현재 프레임 충돌 검사하는 Collider들
	unordered_set<pair<CCollider*, CCollider*>
		, ColliderPairHash, ColliderPairEqual> m_ActiveColliders;


#pragma endregion

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = { };
private:
	list<CCollider*> m_ColliderList = {};


public:
	static CCollider_Manager* Create(_uint iNumLevels);
	virtual void Free();

private:
};
NS_END

