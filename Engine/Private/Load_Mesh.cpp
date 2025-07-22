#include "Load_Mesh.h"

CLoad_Mesh::CLoad_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CLoad_Mesh::CLoad_Mesh(const CLoad_Mesh& Prototype)
	: CVIBuffer( Prototype )
{
}



HRESULT CLoad_Mesh::Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, std::ifstream& ifs)
{
	
	HRESULT hr = eModelType == MODELTYPE::ANIM ? Initialize_Vertex_For_Anim(ifs)
		: Initialize_Vertex_For_NonAnim(ifs, PreTransformMatrix);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoad_Mesh::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CLoad_Mesh::Initialize_Vertex_For_NonAnim(std::ifstream& ifs, _fmatrix PreTransformMatrix)
{
	/* ---------- Mesh ---------- */

	ANIMMESH_INFO info = {};
	/* 1. 읽어 오기 */
	// uint32_t iMarterialIndex;
	// uint32_t iVertexCount;
	// uint32_t iIndicesCount;
	// uint32_t iBoneCount; 

	ifs.read(reinterpret_cast<char*>(&info.iMarterialIndex), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iVertexCount), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iIndicesCount), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iBoneCount), sizeof(uint32_t));

	/* 2. Mesh 이름 가져오기 .*/
	info.strName = ReadString(ifs);
	strcpy_s(m_szName, info.strName.c_str());

	/* 3. Vertex Vecetor 가져오기 */
	info.vertices.resize(info.iVertexCount);

	// 4. 버텍스 데이터에 정점 데이터 채워넣기.
	if (!ReadBytes(ifs, info.vertices.data(), sizeof(VTXANIMMESH) * info.iVertexCount))
		CRASH("READ VERTICES FAILED");

	// 5. 채워넣은 데이터로 값 채워넣기.	
	m_iVertexStride = sizeof(VTXANIMMESH);
	m_iNumVertices = info.iVertexCount;
	m_iMaterialIndex = info.iMarterialIndex;
	m_iNumIndices = info.iIndicesCount;
	m_iNumBones = info.iBoneCount;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	/* 6. Vertices 정의 */
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	// 7. 정보 채워넣기.
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		// 모델 생성시 Transform 설정.
		memcpy(&pVertices[i].vPosition, &info.vertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &info.vertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &info.vertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &info.vertices[i].vBinormal, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndex, &info.vertices[i].vBlendIndex, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeight, &info.vertices[i].vBlendWeight, sizeof(_float4));
		memcpy(&pVertices[i].vTexcoord, &info.vertices[i].vTexcoord, sizeof(_float2));
	}


	//m_iNumVertices = meshVectorSize;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;


	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		CRASH("Create Buffer FAILED");

	Safe_Delete_Array(pVertices);


	// 8. 인덱스 데이터 읽어오기.
	info.indices.resize(info.iIndicesCount);
	if (!ReadBytes(ifs, info.indices.data(), sizeof(uint32_t) * info.iIndicesCount))
		CRASH("READ INDICIES FAILED");

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	/* 9. 인덱스 데이터 채워넣기. */
	_uint* pIndices = new _uint[m_iNumIndices];

	for (_uint i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = info.indices[i];

		/* 충돌 확인용 벡터 인덱스 생성. */
		m_vecIndices.push_back(pIndices[i]);
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		CRASH("CREATE INDEX BUFFER FAILED");

	Safe_Delete_Array(pIndices);

	/* 10. Bone 정보 채워넣기.*/
	ifs.read(reinterpret_cast<char*>(&info.BoneIndexVectorSize), sizeof(uint32_t));

	info.Boneindices.resize(info.BoneIndexVectorSize);

	// 11. Bone Vector 채우기.
	if (!ReadBytes(ifs, info.Boneindices.data(), sizeof(_int) * info.BoneIndexVectorSize))
		CRASH("BONE VECTOR READ FAILED");

	// 12. Offset 정보 채워넣기.
	ifs.read(reinterpret_cast<char*>(&info.OffSetVectorSize), sizeof(uint32_t));
	info.OffsetMatrices.resize(info.OffSetVectorSize);

	// 13. Offset matrix 정보 채워넣기.
	if (!ReadBytes(ifs, info.OffsetMatrices.data(), sizeof(_float4x4) * info.OffSetVectorSize))
		CRASH("OFFSET MATRIX FAILED");

	// 14. BoneIndices, OffSetMatrices Vector에 값 복사하기. 
	m_BoneIndices = info.Boneindices;
	m_OffsetMatrices = info.OffsetMatrices;

	return S_OK;
}

HRESULT CLoad_Mesh::Initialize_Vertex_For_Anim(std::ifstream& ifs)
{
	/* ---------- Mesh ---------- */

	ANIMMESH_INFO info = {};
	/* 1. 읽어 오기 */
	// uint32_t iMarterialIndex;
	// uint32_t iVertexCount;
	// uint32_t iIndicesCount;
	// uint32_t iBoneCount; 

	ifs.read(reinterpret_cast<char*>(&info.iMarterialIndex), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iVertexCount), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iIndicesCount), sizeof(uint32_t));
	ifs.read(reinterpret_cast<char*>(&info.iBoneCount), sizeof(uint32_t));

	/* 2. Mesh 이름 가져오기 .*/
	info.strName = ReadString(ifs);
	strcpy_s(m_szName, info.strName.c_str());

	/* 3. Vertex Vecetor 가져오기 */
	info.vertices.resize(info.iVertexCount);

	// 4. 버텍스 데이터에 정점 데이터 채워넣기.
	if (!ReadBytes(ifs, info.vertices.data(), sizeof(VTXANIMMESH) * info.iVertexCount))
		CRASH("READ VERTICES FAILED");

	// 5. 채워넣은 데이터로 값 채워넣기.	
	m_iVertexStride = sizeof(VTXANIMMESH);
	m_iNumVertices = info.iVertexCount;
	m_iMaterialIndex = info.iMarterialIndex;
	m_iNumIndices = info.iIndicesCount;
	m_iNumBones = info.iBoneCount;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	/* 6. Vertices 정의 */
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	// 7. 정보 채워넣기.
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		// 모델 생성시 Transform 설정.
		memcpy(&pVertices[i].vPosition, &info.vertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &info.vertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &info.vertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &info.vertices[i].vBinormal, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndex, &info.vertices[i].vBlendIndex, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeight, &info.vertices[i].vBlendWeight, sizeof(_float4));
		memcpy(&pVertices[i].vTexcoord, &info.vertices[i].vTexcoord, sizeof(_float2));
	}


	//m_iNumVertices = meshVectorSize;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;


	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		CRASH("Create Buffer FAILED");

	Safe_Delete_Array(pVertices);


	// 8. 인덱스 데이터 읽어오기.
	info.indices.resize(info.iIndicesCount);
	if (!ReadBytes(ifs, info.indices.data(), sizeof(uint32_t) * info.iIndicesCount))
		CRASH("READ INDICIES FAILED");

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	/* 9. 인덱스 데이터 채워넣기. */
	_uint* pIndices = new _uint[m_iNumIndices];

	for (_uint i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = info.indices[i];

		/* 충돌 확인용 벡터 인덱스 생성. */
		m_vecIndices.push_back(pIndices[i]);
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		CRASH("CREATE INDEX BUFFER FAILED");

	Safe_Delete_Array(pIndices);

	/* 10. Bone 정보 채워넣기.*/
	ifs.read(reinterpret_cast<char*>(&info.BoneIndexVectorSize), sizeof(uint32_t));

	info.Boneindices.resize(info.BoneIndexVectorSize);

	// 11. Bone Vector 채우기.
	if (!ReadBytes(ifs, info.Boneindices.data(), sizeof(_int) * info.BoneIndexVectorSize))
		CRASH("BONE VECTOR READ FAILED");

	// 12. Offset 정보 채워넣기.
	ifs.read(reinterpret_cast<char*>(&info.OffSetVectorSize), sizeof(uint32_t));
	info.OffsetMatrices.resize(info.OffSetVectorSize);

	// 13. Offset matrix 정보 채워넣기.
	if (!ReadBytes(ifs, info.OffsetMatrices.data(), sizeof(_float4x4) * info.OffSetVectorSize))
		CRASH("OFFSET MATRIX FAILED");

	// 14. BoneIndices, OffSetMatrices Vector에 값 이동하기. 
	m_BoneIndices = move(info.Boneindices);
	m_OffsetMatrices = move(info.OffsetMatrices);
	

	return S_OK;
}

const _bool CLoad_Mesh::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist)
{
	_bool bHit = false;
	_float fClosestDist = 1000.f;

	_fvector vRayOrigin = XMLoadFloat3(&rayOrigin);
	_fvector vRayDir = XMLoadFloat3(&rayDir);

	for (_uint i = 0; i < m_vecIndices.size(); i += 3)
	{
		_fvector v0 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i]]);
		_gvector v1 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 1]]);
		_hvector v2 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 2]]);

		_float fDist = 0.f;
		if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v1, v2, fDist))
		{
			if (fDist < fClosestDist)
			{
				fClosestDist = fDist;
				bHit = true;
			}
		}
	}

	if (bHit && pOutDist)
		*pOutDist = fClosestDist;

	return bHit;
}




HRESULT CLoad_Mesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CLoad_Bone*>& Bones)
{
	// 매 프레임 Bone Matrix 행렬을 초기화합니다.
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CLoad_Mesh::Bind_Resources()
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

HRESULT CLoad_Mesh::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}


CLoad_Mesh* CLoad_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, std::ifstream& ifs)
{
	CLoad_Mesh* pInstance = new CLoad_Mesh(pDevice, pContext);
	// 4. Prototype 생성.
	if (FAILED(pInstance->Initialize_Prototype(eModelType, PreTransformMatrix, ifs)))
	{
		MSG_BOX(TEXT("Create Failed : Load_Mesh"));
		Safe_Release(pInstance);
		CRASH("LOAD MESH FAILED");
	}
	
	return pInstance;
}

CComponent* CLoad_Mesh::Clone(void* pArg)
{
	CLoad_Mesh* pInstance = new CLoad_Mesh(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoad_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoad_Mesh::Free()
{
	__super::Free();


}
