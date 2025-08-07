#pragma once
#include "Component.h"

/*
* 밀려냄을 어떻게 구현할 것인가?..
*/
NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct tagColliderDesc
	{
		_bool IsTrigger = { false };
	}COLLIDER_DESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	class CGameObject* Get_Owner();
	const _bool Is_Active();
	void Set_Active(_bool IsActive);
	const _bool Find_ColliderObject(CGameObject* pColliderObject);
	void Erase_ColliderObject(CGameObject* pColliderObject);
	void Insert_ColliderObject(CGameObject* pColliderObject);

public:
	void Change_BoundingDesc(void* pBoundingDesc);
	void Reset_Bounding();

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	void Update(_fmatrix WorldMatrix); // 현재 콜라이더의 주인 객체로부터 Update를 받는다.?

public:
	_bool Intersect(const CCollider* pTargetCollider); // 충돌 확인.

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
	class CGameObject* m_pOwner = { nullptr };
	_bool	m_IsActive = { true }; // 활성화 상태
	_bool	m_IsTrigger = { false }; // 충돌 Trigger냐? 아니냐?
	COLLIDER m_eType = {};
	unordered_set<CGameObject*> m_ColliderObjects = {}; // 콜라이더 저장 용도
	DirectX::XMVECTORF32 m_vColor = { DirectX::Colors::Black };



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

