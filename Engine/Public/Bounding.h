#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc : public CCollider::COLLIDER_DESC
	{
		_float3		vCenter;
	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

#pragma region 디버깅 용도

#ifdef _DEBUG
	
#endif // _DEBUG


#pragma endregion



public:
	virtual const _float3 Get_WorldCenter() PURE;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor = DirectX::Colors::White) { return S_OK; }
#endif

#pragma region 충돌 감시
public:
	virtual _bool Intersect(COLLIDER eColliderType, CBounding* pBounding) PURE;
#pragma endregion

public:
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) PURE;
	virtual void Reset_Bounding() PURE;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free();

};
NS_END

