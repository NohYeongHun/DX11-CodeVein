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
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);

public:
	const BoundingSphere* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

#ifdef _DEBUG
public:
	CBounding_Sphere::BOUNDING_SPHERE_DESC* Get_DebugDesc() { return &m_DebugDesc; }

private:
	CBounding_Sphere::BOUNDING_SPHERE_DESC m_DebugDesc = {};

#endif // DEBUG


public:
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor = DirectX::Colors::White) override;
#endif

#pragma region 충돌 감시
public:
	virtual _bool Intersect(COLLIDER eColliderType, CBounding* pBounding) override;
	virtual _float Calculate_PenetrationDepthSpehre(CBounding* pBounding) override;
#pragma endregion





private:
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();

};
NS_END

