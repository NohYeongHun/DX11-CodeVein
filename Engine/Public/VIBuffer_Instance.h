#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint			iNumInstance;
		_float3			vCenter;
		_float3			vRange;
		_float2			vSize;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg);
	/* 준비된 자원을 이용하여 어떠한 기능(렌더링)을 수행하기위하여 자원을 장치에 바인딩한다. */
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint		  m_iNumInstance = {};
	_uint		  m_iNumIndexPerInstance = {};
	_uint		  m_iInstanceVertexStride = {};

protected:
	D3D11_BUFFER_DESC		m_VBInstanceDesc{};
	void* m_pInstanceVertices = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};
NS_END

