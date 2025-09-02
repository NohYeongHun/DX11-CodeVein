#pragma once

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_SwordTrail final : public CVIBuffer
{
public:
	typedef struct Trail_Point
	{
		_float3		vPointUp;
		_float3		vPointDown;

		_float4x4	vTrailMatrix;

	}TRAILPOINT;

private:
	CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& Prototype);
	virtual ~CVIBuffer_SwordTrail() = default;

private:
	static constexpr _uint MAX_TRAIL_POINTS = 30;
	static constexpr _uint INTERPOLATION_SEGMENTS = 10;
	static constexpr _float INTERPOLATION_RANGE = 20.f;
	
	list<_float3>	m_TrailPoints;
	_uint		m_CurrentPointCount = 0;
	_uint		m_NumIndicesPerPrimitive = {};

private:
	HRESULT UpdateVertexBuffer();
	HRESULT ApplyInterpolation(VTXPOSTEX* pVertices, const vector<_vector>& positions);
	void SetTextureCoordinates(VTXPOSTEX* pVertices, _uint pointCount);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Render();
	virtual HRESULT Bind_Resources();
	HRESULT Update(TRAILPOINT TrailPoint);

public:
	static CVIBuffer_SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
