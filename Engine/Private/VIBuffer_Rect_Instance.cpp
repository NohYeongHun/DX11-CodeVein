CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype)
	: CVIBuffer_Instance( Prototype )
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];	

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);	

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	VTXINSTANCE_PARTICLE* pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		/*pInstanceVertices[i].vRight = ;*/

	}

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	CVIBuffer_Instance::Free();
}
