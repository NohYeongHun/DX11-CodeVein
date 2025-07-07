#pragma once
#include "Component.h"

/*
* 밀려냄을 어떻게 구현할 것인가?..
*/
NS_BEGIN(Engine)
class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum class STATUS { NONE, ENTER, STAY, EXIT, STATUS_END };

public:
	typedef struct tagColliderInfo
	{
		CCollider* pCollider; // Collider
		STATUS eStatus; // 상태
	}COLLIDER_INFO;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(COLLIDERSHAPE eColliderType);
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	void Update(_fmatrix WorldMatrix); // 현재 콜라이더의 주인 객체로부터 Update를 받는다.?

public:
	_bool Intersect(const CCollider* pTargetCollider); // 밀려남 처리.

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:


public:
	//static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDERSHAPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

