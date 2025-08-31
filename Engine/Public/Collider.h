#pragma once
#include "Component.h"

/*
* 밀려냄을 어떻게 구현할 것인가?..
*/
NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{

#pragma region 0. Collision Type과 Layer 관리
public:
	/* 충돌 타입. */
	enum COLLISION_TYPE
	{
		COLLISION_BODY, // 물체 충돌.
		COLLISION_TRIGGER, // 트리거
		COLLISION_SENSOR, // 감지만.
		COLLISION_END
	};

	/* 충돌 레이어*/
	enum LAYER
	{
		PLAYER = 1 << 0,
		PLAYER_WEAPON = 1 << 1,
		PLAYER_SKILL = 1 << 2,
		MONSTER = 1 << 3,
		MONSTER_WEAPON = 1 << 4,
		MONSTER_SKILL = 1 << 5,
		STATIC_OBJECT = 1 << 30,
		LAYER_END = 1 << 31
	};

public:
	/*
	* 1. 소유자
	* 2. 어떤 Type의 Collider인지.
	* 3. 어떤 Layer에 속하는지
	* 4. 어떤 Layer와 충돌 처리 할 것인지?
	*/
	typedef struct tagColliderDesc
	{
		class CGameObject* pOwner = { nullptr };
		COLLISION_TYPE eCollisionType = { COLLISION_BODY };
		LAYER eMyLayer = {};
		uint32_t eTargetLayer = {};
	}COLLIDER_DESC;
#pragma endregion



protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

#pragma region 0. 초기화 함수.
public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize_Clone(void* pArg);
#pragma endregion

#pragma region DEBUG용도 함수.

public:
	void* Get_BoundingDesc();

#ifdef _DEBUG




	
#endif // _DEBUG

public:
	COLLIDER Get_ColliderShape() { return m_eColliderShape; }
#pragma endregion


#pragma region Collider 크기 변경
public:
	void Change_BoundingDesc(void* pBoundingDesc);
	void Reset_Bounding();
#pragma endregion



#pragma region 0. 충돌체가 가져야 하는 식별 정보와 관련 함수들

public:
	// 1. 주인 체크
	class CGameObject* Get_Owner();

public:
	// 2. 활성화 상태 체크.
	const _bool Is_Active();
	void Set_Active(_bool IsActive);

public:
	// 3. Layer 체크 => 해당 안되면 바로 무시.
	_bool Has_TargetLayer(CCollider* pRight);
	COLLISION_TYPE Get_Collision_Type() { return m_eCollisionType; }
	_float3 Get_Center();

	
private:
	/* Prototype 생성시 정해지는 변수. */
	// 0. 콜라이더는 어떤 형태인가? => 생성과 동시에 정해짐.
	COLLIDER m_eColliderShape = {};

	/* Clone 시 정해지는 변수 */
	// 1. 활성화 상태
	_bool	m_IsActive = { true };
	// 2. Collision이 어떤 타입인지. => 물리충돌만 지원하나?, Trigger인가? 감지만하는가?
	COLLISION_TYPE m_eCollisionType = {};
	// 3. Collider는 어떤 Layer에 속하고 어떤 Layer와 충돌을 지원하는가?
	LAYER m_eMyLayer = {};
	uint32_t m_TargetLayers = { 0 };

	// 4. 콜라이더의 주인 객체는 누구인가?
	class CGameObject* m_pOwner = { nullptr };
#pragma endregion


#pragma region 3. Collider Manager에서 호출하는 충돌 관리용 함수.
public:
	_bool Has_TargetLayerCheck(CCollider* pRight);

	const _bool Find_ColliderObject(CGameObject* pColliderObject);
	void Erase_ColliderObject(CGameObject* pColliderObject);
	void Insert_ColliderObject(CGameObject* pColliderObject);
#pragma endregion




#pragma region 5. 매프레임 업데이트 되는 함수
public:
	// 현재 콜라이더의 주인 객체로부터 Update를 받아 위치 및 회전 갱신
	void Update(_fmatrix WorldMatrix); 

public:
	// 실제 충돌체와의 충돌 확인.
	_bool Intersect(const CCollider* pTargetCollider);

	// 충돌 전 후보 쌍 추려내기
	_bool BroadIntersect(const CCollider* pTargetCollider);

	// 구체간 침투 깊이 구하기.
	_float Calculate_PenetrationDepthSpehre(const CCollider* pTargetCollider);
#pragma endregion



#ifdef _DEBUG
	HRESULT Render();
#endif

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

private:
	class CBounding* m_pBounding = { nullptr };
	class CBounding* m_pWorldBounding = { nullptr };

	unordered_set<CGameObject*> m_ColliderObjects = {}; // 콜라이더 저장 용도
	DirectX::XMVECTORF32 m_vColor = { DirectX::Colors::Black };



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

