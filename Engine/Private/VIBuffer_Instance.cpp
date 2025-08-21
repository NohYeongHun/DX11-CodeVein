CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{pDevice, pContext}
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer(Prototype)
	, m_pVBInstance{ Prototype.m_pVBInstance }
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize_Clone(void* pArg)
{
	return S_OK;
}


void CVIBuffer_Instance::Free()
{
	CVIBuffer::Free();
	Safe_Release(m_pVBInstance);
}
