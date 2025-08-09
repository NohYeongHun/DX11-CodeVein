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
	HRESULT Add_Collider_To_Manager(CCollider* pCollider);

public:
	void Update();
	void Collision_CleanUp(); // 충돌 이후에 죽은 객체들 솎아내기.
#pragma endregion

	

#pragma region 1. 충돌 작업 세분화
private:
	void Build_BroadPhase();
	void Narrow_Phase();

private:
	HRESULT Remove_Collider();

private:
	void Handle_Collision_Exit(CCollider* pLeft, CCollider* pRight, CGameObject* pLeftOwner, CGameObject* pRightOwner);
	void Handle_Collision_By_Type(CCollider* pLeft, CCollider* pRight, CGameObject* pLeftOwner, CGameObject* pRightOwner);
#pragma endregion


#pragma region 0. 충돌 관리용 멤버 변수

private:
	// 전체 등록될 콜라이더
	vector<CCollider*>  m_RegisterPool;
	// 삭제할 인덱스를 담는 Pool
	vector<_uint>		m_freePool;

private:
	_uint m_iFree_StartIdx = {}; // Free가 시작되는 인덱스.
#pragma endregion

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = { };

	// 최적화는 각 객체들이 업데이트나 Render에서 조건에 따라 Collider를 안넣으면 되려나?
	
private:
	unordered_set<pair<CCollider*, CCollider*>
		, ColliderPairHash, ColliderPairEqual> m_ActiveColliders;

	list<CCollider*> m_Narrow_ColliderList = {};

	list<CCollider*> m_ColliderList = {};


public:
	static CCollider_Manager* Create(_uint iNumLevels);
	virtual void Free();

private:
};
NS_END

