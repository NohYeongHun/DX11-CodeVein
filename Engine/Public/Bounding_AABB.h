#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)
class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;

private:
	explicit CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;


public:
	const BoundingBox* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);
	//virtual HRESULT Render(PrimitiveBatch)

public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) override;
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;

private:
	BoundingBox* m_pLocalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();


};
NS_END

