#include "VIBuffer_SwordTrail.h"

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& Prototype)
	: CVIBuffer( Prototype )
{
}

HRESULT CVIBuffer_SwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Initialize_Clone(void* pArg)
{

	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = m_iLimitPointCount * 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버텍스 버퍼 바인드용
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);

	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
	{
		CRASH("Failed Create Buffer");
		return E_FAIL;
	}
		

	Safe_Delete_Array(pVertices);

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = m_iLimitPointCount * 2 - 2;
	m_iNumIndicesPerPrimitive = 3;

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DYNAMIC; // 동적 버퍼 생성.
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	FACEINDICES* pIndices = new FACEINDICES[m_iNumIndices];
	_uint quadIndex = 0;

	for (_uint iIndex = 0; iIndex < m_iNumIndices; iIndex += 2)
	{
		_uint baseVertex = quadIndex * 2;  // 각 Quad의 시작 정점

		// 첫 번째 삼각형: (0, 2, 1)
		pIndices[iIndex]._0 = baseVertex;         // 왼쪽 아래
		pIndices[iIndex]._1 = baseVertex + 2;     // 오른쪽 아래  
		pIndices[iIndex]._2 = baseVertex + 1;     // 왼쪽 위

		// 두 번째 삼각형: (1, 2, 3)
		pIndices[iIndex + 1]._0 = baseVertex + 1; // 왼쪽 위
		pIndices[iIndex + 1]._1 = baseVertex + 2; // 오른쪽 아래
		pIndices[iIndex + 1]._2 = baseVertex + 3; // 오른쪽 위

		quadIndex++;
	}

	/*FACEINDICES32* pIndices = new FACEINDICES32[m_iNumIndices];

	for (_uint iIndex = 0; iIndex < m_iNumIndices; iIndex += 2)
	{
		pIndices[iIndex]._0 = iIndex + 3;
		pIndices[iIndex]._1 = iIndex + 1;
		pIndices[iIndex]._2 = iIndex;

		pIndices[iIndex + 1]._0 = iIndex + 2;
		pIndices[iIndex + 1]._1 = iIndex + 3;
		pIndices[iIndex + 1]._2 = iIndex;
	}*/

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Render()
{
	
	m_pContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint		iOffsets[] = {
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Update(TRAILPOINT TrailPoint)
{
	_float3 PointDown;
	_float3 PointUp;

	XMStoreFloat3(&PointDown, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vPointDown), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));
	XMStoreFloat3(&PointUp, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vPointUp), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));

	if (!isnan(PointDown.x))
	{
		m_TrailPoint.push_back(PointDown);
		m_TrailPoint.push_back(PointUp);
	}

	if (m_TrailPoint.size() > m_iLimitPointCount)
	{
		m_TrailPoint.pop_front();
		m_TrailPoint.pop_front();
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = (VTXPOSTEX*)SubResource.pData;

	auto TrailPointIter = m_TrailPoint.begin();

	_vector		vPos[50];

	for (_uint iIndex = 0; iIndex < m_TrailPoint.size(); iIndex += 2)
	{
		vPos[iIndex] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		_float fUCoord = (_float)(iIndex / 2) / (_float)((m_TrailPoint.size() / 2) - 1);
		_float2 vTexCoord = _float2(fUCoord, 1.f);
		XMStoreFloat3(&pVertices[iIndex].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		XMStoreFloat2(&pVertices[iIndex].vTexcoord, XMLoadFloat2(&vTexCoord));

		++TrailPointIter;

		vPos[iIndex + 1] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);

		XMStoreFloat3(&pVertices[iIndex + 1].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		vTexCoord = _float2(fUCoord, 0.f);
		XMStoreFloat2(&pVertices[iIndex + 1].vTexcoord, XMLoadFloat2(&vTexCoord));

		++TrailPointIter;
	}


#pragma region 보간

	m_iCatmullRomCount = m_TrailPoint.size();
	if (m_iCatmullRomCount < 2)
		goto Unmaping;

	XMVECTOR vec;
	_float3	 vCat[80];

	if (m_TrailPoint.size() >= m_iLimitPointCount) //트레일갯수 다 채워졌을때
	{
		_float4 vCatPos = { 0.f,0.f ,0.f ,0.f };
		_float3 vTrailPos = {};
		_float3 vTempPos = {};

		//소드 트레일의 윗부분 보간
		for (_int i = 19; i >= 10; i--) //보간 구간은 10으로 잡는다
		{
			vec = XMVectorCatmullRom(vPos[10], vPos[11], vPos[19], vPos[18], _float((i - 10) / 20.f));
			XMStoreFloat4(&vCatPos, vec);
			
			vTrailPos = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
			XMStoreFloat3(&pVertices[(m_iLimitPointCount - 1 - i) * 2 + 1].vPosition, XMLoadFloat3(&vTrailPos));
			XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2 + 1], XMLoadFloat3(&vTrailPos));
		}

		//소드 트레일의 아랫부분 보간
		for (_int i = 19; i >= 10; i--) //보간 구간은 10으로 잡는다
		{
			

			vec = XMVectorCatmullRom(vPos[11], vPos[10], vPos[18], vPos[19], _float((i - 10) / 20.f));
			XMStoreFloat4(&vCatPos, vec);
			vTempPos = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
			XMStoreFloat3(&pVertices[(m_iLimitPointCount - 1 - i) * 2].vPosition, XMLoadFloat3(&vTempPos));
			XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2], XMLoadFloat3(&vTempPos));
		}
	}


#pragma endregion 전체 구간 보간


	Unmaping :
	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

CVIBuffer_SwordTrail* CVIBuffer_SwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_SwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_SwordTrail::Clone(void* pArg)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_SwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SwordTrail::Free()
{
	CVIBuffer::Free();
}
