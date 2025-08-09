#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;

private:
	explicit CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;


#pragma region DEBUG 용도

#ifdef _DEBUG
public:
	CBounding_AABB::BOUNDING_AABB_DESC* Get_DebugDesc() { return &m_DebugDesc; }

private:
	CBounding_AABB::BOUNDING_AABB_DESC m_DebugDesc = {};
#endif // DEBUG
	
#pragma endregion
public:
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;

public:
	const BoundingBox* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor = DirectX::Colors::White) override;
#endif

#pragma region 충돌 감시
public:
	virtual _bool Intersect(COLLIDER eColliderType, CBounding* pBounding) override;
	virtual _float Calculate_PenetrationDepthSpehre(CBounding* pBounding) override { return 0.f; };
#pragma endregion



private:
	BoundingBox* m_pOriginalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };



public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();


};
NS_END

