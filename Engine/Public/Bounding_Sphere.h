#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	}BOUNDING_SPHERE_DESC;

private:
	explicit CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	const BoundingSphere* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor = DirectX::Colors::White) override;

public:
	virtual _bool Intersect(COLLIDER eColliderType, CBounding* pBounding) override;
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;



private:
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();

};
NS_END

