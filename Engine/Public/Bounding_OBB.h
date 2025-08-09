#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;

	typedef struct tagOBBDesc
	{
		_float3		vCenterDir[3];
		_float3		vAlignDir[3];
	}OBB_DESC;


private:
	explicit CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;


public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

#pragma region DEBUG 용도



#ifdef _DEBUG
public:
	CBounding_OBB::BOUNDING_OBB_DESC* Get_DebugDesc() { return &m_DebugDesc; }

private:
	CBounding_OBB::BOUNDING_OBB_DESC m_DebugDesc = {};

#endif // DEBUG

#pragma endregion


public:
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor = DirectX::Colors::White) override;
#endif

#pragma region 충돌 감시
public:
	virtual _bool Intersect(COLLIDER eColliderType, CBounding* pBounding) override;
	virtual _float Calculate_PenetrationDepthSpehre(CBounding* pBounding) override { return 0.f; };
#pragma endregion



private:
	BoundingOrientedBox* m_pOriginalDesc = { nullptr };
	BoundingOrientedBox* m_pDesc = { nullptr };
	_float3 m_vRotation = {};

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();
};
NS_END

