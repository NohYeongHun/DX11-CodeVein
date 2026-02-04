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
	m_iNumVertices = MAX_TRAIL_POINTS * 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
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
	m_iNumIndices = (MAX_TRAIL_POINTS - 1) * 2 * 3; // (쿼드 수) * (삼각형 2개) * (인덱스 3개)
	m_NumIndicesPerPrimitive = 3;

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DYNAMIC; // 동적 버퍼 생성.
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	FACEINDICES* pIndices = new FACEINDICES[m_iNumIndices / 3];
	_uint triangleIndex = 0;

	for (_uint quadIndex = 0; quadIndex < MAX_TRAIL_POINTS - 1; quadIndex++)
	{
		_uint baseVertex = quadIndex * 2;  // 각 Quad의 시작 정점

		// 첫 번째 삼각형: (0, 2, 1)
		pIndices[triangleIndex]._0 = baseVertex;         // 왼쪽 아래
		pIndices[triangleIndex]._1 = baseVertex + 2;     // 오른쪽 아래  
		pIndices[triangleIndex]._2 = baseVertex + 1;     // 왼쪽 위
		triangleIndex++;

		// 두 번째 삼각형: (1, 2, 3)
		pIndices[triangleIndex]._0 = baseVertex + 1; // 왼쪽 위
		pIndices[triangleIndex]._1 = baseVertex + 2; // 오른쪽 아래
		pIndices[triangleIndex]._2 = baseVertex + 3; // 오른쪽 위
		triangleIndex++;
	}


	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Render()
{
	if (m_CurrentPointCount < 2) return S_OK; // 최소 1개 쿼드 필요 (2개 정점)
	
	_uint actualQuadCount = (m_CurrentPointCount / 2) - 1; // 실제 쿼드 개수
	if (actualQuadCount == 0) return S_OK; // 쿼드가 없으면 렌더링 안함
	
	_uint actualIndexCount = actualQuadCount * 6; // 쿼드당 인덱스 6개
	

	m_pContext->DrawIndexed(actualIndexCount, 0, 0);

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
		m_TrailPoints.push_back(PointDown);
		m_TrailPoints.push_back(PointUp);
	}

	if (m_TrailPoints.size() > MAX_TRAIL_POINTS)
	{
		m_TrailPoints.pop_front();
		m_TrailPoints.pop_front();
	}

	if (m_TrailPoints.size() < 8) return S_OK;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	//m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);

	/*auto TrailPointIter = m_TrailPoints.begin();

	_vector		vPos[50];

	for (_uint iIndex = 0; iIndex < m_TrailPoints.size(); iIndex += 2)
	{
		vPos[iIndex] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		_float fUCoord = (_float)(iIndex / 2) / (_float)((m_TrailPoints.size() / 2) - 1);
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

	m_CurrentPointCount = m_TrailPoints.size();
	if (m_CurrentPointCount >= 2 && m_TrailPoints.size() >= MAX_TRAIL_POINTS)
	{
		ApplyInterpolation(pVertices, vector<_vector>(vPos, vPos + m_CurrentPointCount));
	}*/

	ApplyInterpolation(pVertices);

#pragma endregion

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

HRESULT CVIBuffer_SwordTrail::ApplyInterpolation(VTXPOSTEX* pVertices, const vector<_vector>& positions)
{
	if (positions.size() < 4) return S_OK;

	const _uint startIndex = INTERPOLATION_SEGMENTS;
	const _uint endIndex = startIndex + INTERPOLATION_SEGMENTS;
	const _float invRange = 1.0f / INTERPOLATION_RANGE;

	// 상단 트레일 보간
	for (_uint i = 0; i < INTERPOLATION_SEGMENTS; ++i)
	{
		_float t = static_cast<_float>(i) * invRange;
		_vector interpolatedPos = XMVectorCatmullRom(
			positions[startIndex], positions[startIndex + 1],
			positions[endIndex - 1], positions[endIndex - 2], t
		);


		_uint vertexIndex = (MAX_TRAIL_POINTS - 1 - (endIndex - 1 - i)) * 2 + 1;
		XMStoreFloat3(&pVertices[vertexIndex].vPosition, interpolatedPos);
	}

	// 하단 트레일 보간
	for (_uint i = 0; i < INTERPOLATION_SEGMENTS; ++i)
	{
		_float t = static_cast<_float>(i) * invRange;
		_vector interpolatedPos = XMVectorCatmullRom(
			positions[startIndex + 1], positions[startIndex],
			positions[endIndex - 2], positions[endIndex - 1], t
		);

		_uint vertexIndex = (MAX_TRAIL_POINTS - 1 - (endIndex - 1 - i)) * 2;
		XMStoreFloat3(&pVertices[vertexIndex].vPosition, interpolatedPos);
	}

	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::ApplyInterpolation(VTXPOSTEX* pVertices)
{
	const int SAMPLES_PER_SEGMENT = 20;
	_uint vertexIdx = 0;

	// 리스트 내의 '세트(Down/Up)' 개수 계산
	int numPointSets = static_cast<int>(m_TrailPoints.size() / 2);
	int numSegments = numPointSets - 3; // 4세트가 있어야 1개 구간 보간 가능

	// 전체 생성될 정점 수 계산 (UV용)
	float totalExpectedPoints = static_cast<float>(numSegments * SAMPLES_PER_SEGMENT);

	for (int i = 0; i < numSegments; ++i)
	{
		// 반복자를 사용하여 현재 보간에 필요한 4개 세트의 시작점 찾기
		auto it0 = next(m_TrailPoints.begin(), i * 2);       // P(i-1)
		auto it1 = next(it0, 2);                             // P(i)   <- 여기서부터
		auto it2 = next(it1, 2);                             // P(i+1) <- 여기까지 보간
		auto it3 = next(it2, 2);                             // P(i+2)

		for (int j = 0; j < SAMPLES_PER_SEGMENT; ++j)
		{
			if (vertexIdx >= (MAX_TRAIL_POINTS * 2) - 2) break;

			float t = static_cast<float>(j) / static_cast<float>(SAMPLES_PER_SEGMENT);

			// Down 라인 보간 (짝수 번째 인덱스들)
			_vector vD0 = XMLoadFloat3(&(*it0));
			_vector vD1 = XMLoadFloat3(&(*it1));
			_vector vD2 = XMLoadFloat3(&(*it2));
			_vector vD3 = XMLoadFloat3(&(*it3));
			_vector interDown = XMVectorCatmullRom(vD0, vD1, vD2, vD3, t);

			// Up 라인 보간 (홀수 번째 인덱스들 - 바로 다음 칸)
			_vector vU0 = XMLoadFloat3(&(*std::next(it0)));
			_vector vU1 = XMLoadFloat3(&(*std::next(it1)));
			_vector vU2 = XMLoadFloat3(&(*std::next(it2)));
			_vector vU3 = XMLoadFloat3(&(*std::next(it3)));
			_vector interUp = XMVectorCatmullRom(vU0, vU1, vU2, vU3, t);

			float u = static_cast<float>(vertexIdx / 2) / totalExpectedPoints;

			// 정점 기록
			XMStoreFloat3(&pVertices[vertexIdx].vPosition, interDown);
			pVertices[vertexIdx].vTexcoord = _float2(u, 1.f);
			vertexIdx++;

			XMStoreFloat3(&pVertices[vertexIdx].vPosition, interUp);
			pVertices[vertexIdx].vTexcoord = _float2(u, 0.f);
			vertexIdx++;
		}
	}

	m_CurrentPointCount = vertexIdx;
	return S_OK;

	return S_OK;
}

void CVIBuffer_SwordTrail::Free()
{
	CVIBuffer::Free();
}
