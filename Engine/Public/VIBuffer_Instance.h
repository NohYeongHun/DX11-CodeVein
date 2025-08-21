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

	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg);

protected:
	ID3D11Buffer*	m_pVBInstance = { nullptr };	
	_uint			m_iNumInstance = {};
	_uint			m_iNumIndexPerInstance = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};
NS_END

