#include "Tool_Mesh.h"

CTool_Mesh::CTool_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CTool_Mesh::CTool_Mesh(const CTool_Mesh& Prototype)
	: CVIBuffer( Prototype )
{
}

HRESULT CTool_Mesh::Initialize_Prototype(MODELTYPE eModelType, const aiMesh* pAIMesh, const vector<CTool_Bone*>& Bones, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	// Material Index 설정.
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = (eModelType == MODELTYPE::NONANIM || eModelType == MODELTYPE::STATIC )
		? Ready_Vertices_For_NonAnim(pAIMesh, PreTransformMatrix)
		: Ready_Vertices_For_Anim(pAIMesh, Bones);


	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint iNumIndices = {};
	for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];

		/* 충돌 확인용 벡터 인덱스 생성. */
		m_vecIndices.push_back(AIFace.mIndices[0]);
		m_vecIndices.push_back(AIFace.mIndices[1]);
		m_vecIndices.push_back(AIFace.mIndices[2]);
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CTool_Mesh::Initialize_Clone(void* pArg)
{
	return S_OK;
}

const _bool CTool_Mesh::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist)
{
	_bool bHit = false;
	_float fClosestDist = 1000.f;

	_vector vRayOrigin = XMLoadFloat3(&rayOrigin);
	_vector vRayDir = XMLoadFloat3(&rayDir);
	vRayDir = XMVector3Normalize(vRayDir);

	_uint iHitIndex = -1;


	
	for (_uint i = 0; i < m_vecIndices.size(); i += 3)
	{
		
		//_fvector v0 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i]]);
		//_gvector v1 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 1]]);
		//_hvector v2 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 2]]);

		_fvector v0 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i]]);
		_gvector v1 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 1]]);
		_hvector v2 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 2]]);

		_float fDist = 0.f;
		if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v1, v2, fDist))
		{
			if (fDist < fClosestDist)
			{
				fClosestDist = fDist;
				iHitIndex = m_vecIndices[i];
				bHit = true;
			}
		}
	}

	if (bHit && pOutDist)
		*pOutDist = fClosestDist;

	if (bHit)
	{
		*pOutLocalPos = m_Vertices[iHitIndex].vPosition;
		*pOutLocalNormal = m_Vertices[iHitIndex].vNormal;
	}

	return bHit;
}

const _bool CTool_Mesh::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, MODEL_PICKING_INFO* pPickingInfo, _float* pOutDist)
{
	_bool bHit = false;
	_float fClosestDist = 1000.f;

	_vector vRayOrigin = XMLoadFloat3(&rayOrigin);
	_vector vRayDir = XMLoadFloat3(&rayDir);
	vRayDir = XMVector3Normalize(vRayDir);

	_uint iHitTriangleIndex = -1;
	_float3 vHitPoint = {};
	_float3 vTriangleVertices[3] = {};

	for (_uint i = 0; i < m_vecIndices.size(); i += 3)
	{
		_fvector v0 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i]]);
		_fvector v1 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 1]]);
		_fvector v2 = XMLoadFloat3(&m_vecPositions[m_vecIndices[i + 2]]);

		_float fDist = 0.f;
		if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v1, v2, fDist))
		{
			if (fDist < fClosestDist)
			{
				fClosestDist = fDist;
				iHitTriangleIndex = i / 3;  // 삼각형 인덱스
				bHit = true;

				// 히트 포인트 계산 (레이 원점 + 방향 * 거리)
				XMStoreFloat3(&vHitPoint, vRayOrigin + vRayDir * fDist);

				// 삼각형 정점들 저장 (로컬 좌표)
				vTriangleVertices[0] = m_vecPositions[m_vecIndices[i]];
				vTriangleVertices[1] = m_vecPositions[m_vecIndices[i + 1]];
				vTriangleVertices[2] = m_vecPositions[m_vecIndices[i + 2]];
			}
		}
	}

	if (bHit && pPickingInfo)
	{
		// 삼각형 세 정점 다가져오기.
		memcpy(&pPickingInfo->vTriangleVertices, vTriangleVertices, sizeof(_float3) * 3);
		pPickingInfo->vHitPoint = vHitPoint;
		
	}

	if (bHit && pOutDist)
		*pOutDist = fClosestDist;

	return bHit;
}

const _bool CTool_Mesh::Picking(const _float3& rayOrigin, const _float3& rayDir, _float3* pPickingPoint, _float3* pPickingNormal, _float* pOutDist)
{
	_bool bHit = false;
	_float fClosestDist = FLT_MAX;

	_vector vRayOrigin = XMLoadFloat3(&rayOrigin);
	_vector vRayDir = XMLoadFloat3(&rayDir);
	vRayDir = XMVector3Normalize(vRayDir);

	_float3 vClosestHitPoint = {};
	_float3 vClosestHitNormal = {};


	// 모든 삼각형을 순회하며 Ray-Triangle 교차 검사
	for (_uint i = 0; i < m_vecIndices.size(); i += 3)
	{
		// 삼각형의 세 정점 가져오기
		_fvector v0 = XMLoadFloat3(&m_Vertices[m_vecIndices[i]].vPosition);
		_fvector v1 = XMLoadFloat3(&m_Vertices[m_vecIndices[i + 1]].vPosition);
		_fvector v2 = XMLoadFloat3(&m_Vertices[m_vecIndices[i + 2]].vPosition);

		_float fDist = 0.f;

		// DirectX TriangleTests를 사용한 Ray-Triangle 교차 검사
		if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v1, v2, fDist))
		{
			// 뒤쪽 충돌 방지 및 가장 가까운 충돌점 찾기
			if (fDist > 0.f && fDist < fClosestDist)
			{
				fClosestDist = fDist;
				bHit = true;

				// 충돌 지점 계산: Ray Origin + Ray Direction * Distance
				_fvector vHitPoint = vRayOrigin + vRayDir * fDist;
				XMStoreFloat3(&vClosestHitPoint, vHitPoint);

				// 삼각형 법선 벡터 계산
				_fvector vEdge1 = v1 - v0;  // 첫 번째 모서리
				_fvector vEdge2 = v2 - v0;  // 두 번째 모서리

				// 외적을 사용하여 법선 벡터 계산 (오른손 좌표계 기준)
				_fvector vTriangleNormal = XMVector3Normalize(XMVector3Cross(vEdge1, vEdge2));
				XMStoreFloat3(&vClosestHitNormal, vTriangleNormal);
			}
		}
	}

	if (bHit)
	{
		if (pPickingPoint)
			*pPickingPoint = vClosestHitPoint;

		if (pPickingNormal)
			*pPickingNormal = vClosestHitNormal;

		if (pOutDist)
			*pOutDist = fClosestDist;
	}

	return bHit;
}



void CTool_Mesh::Set_BondIndexVector(vector<_int>& boneIndicies)
{
	for (auto& val : m_BoneIndices)
		boneIndicies.emplace_back(val);
}

/* Anim Mesh에 대한 정보를 Save합니다. */
void CTool_Mesh::Save_AnimMeshes(const aiMesh* pAIMesh, ANIMMESH_INFO& AnimMeshInfo)
{
	// 3. 가지고 있는 정보들을 채워주기.
	AnimMeshInfo.iMarterialIndex = m_iMaterialIndex;
	AnimMeshInfo.iVertexCount = m_iNumVertices;
	AnimMeshInfo.iIndicesCount = m_iNumIndices;
	AnimMeshInfo.iBoneCount = m_iNumBones;

	AnimMeshInfo.BoneIndexVectorSize = m_BoneIndices.size();
	AnimMeshInfo.Boneindices = m_BoneIndices;
	AnimMeshInfo.strName = m_szName;

	AnimMeshInfo.OffSetVectorSize = m_OffsetMatrices.size();
	AnimMeshInfo.OffsetMatrices = m_OffsetMatrices; // 깊은 복사.
	
	// 정점 벡터 복사.
	AnimMeshInfo.vertices = m_Vertices;
	// 정점 Incidies 복사.
	AnimMeshInfo.indices = m_vecIndices;
}


HRESULT CTool_Mesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CTool_Bone*>& Bones)
{
	// 매 프레임 Bone Matrix 행렬을 초기화합니다.
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}



HRESULT CTool_Mesh::Bind_Resources()
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

HRESULT CTool_Mesh::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CTool_Mesh::Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		// 모델 생성시 Transform 설정.
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		/* 충돌 확인용 Vertex Position 설정. */
		m_vecPositions.push_back(pVertices[i].vPosition);
	}

	m_iNumBones = 0;

	m_Vertices.reserve(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_Vertices.emplace_back(pVertices[i]);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	/* MeshInfo Header에 생성과 동시에 저장가능한 정보들을 채워줍니다. */
	Safe_Delete_Array(pVertices);

	return S_OK;
}

//HRESULT CTool_Mesh::Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
//{
//	m_iVertexStride = sizeof(VTXMESH);
//
//	D3D11_BUFFER_DESC		VBDesc{};
//	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
//	VBDesc.Usage = D3D11_USAGE_DEFAULT;
//	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	VBDesc.CPUAccessFlags = 0;
//	VBDesc.MiscFlags = 0;
//	VBDesc.StructureByteStride = m_iVertexStride;
//
//	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
//
//	for (_uint i = 0; i < m_iNumVertices; i++)
//	{
//		// 모델 생성시 Transform 설정.
//		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
//		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
//		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
//
//		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
//		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
//		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
//
//		/* 충돌 확인용 Vertex Position 설정. */
//		m_vecPositions.push_back(pVertices[i].vPosition);
//	}
//
//	/*m_Vertices.reserve(m_iNumVertices);
//	for (_uint i = 0; i < m_iNumVertices; ++i)
//		m_Vertices.emplace_back(pVertices[i]);*/
//
//	D3D11_SUBRESOURCE_DATA	VBInitialData{};
//	VBInitialData.pSysMem = pVertices;
//
//	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
//		return E_FAIL;
//
//	/* MeshInfo Header에 생성과 동시에 저장가능한 정보들을 채워줍니다. */
//	Safe_Delete_Array(pVertices);
//
//	return S_OK;
//}

HRESULT CTool_Mesh::Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<CTool_Bone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		// 모델 생성시 Transform 설정.
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		m_vecPositions.push_back(pVertices[i].vPosition);
	}

	m_iNumBones = pAIMesh->mNumBones;

	// 애니메이션용 본 인덱스와 가중치 설정.
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4 OffsetMatrix;

		/*
		* 같은 뼈를 가지고도 모델마다 다르게 적용하기 위한 OffsetMatrix
		*/
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		/*
		* Assimp에서 불러오는 행렬은 ColMajor(열에 정보가 저장되어 있는 행렬)
		* 현재 상태에서 사용하기 위해서는 RowMajor(행에 정보가 저장되어 있는 행렬)로 사용해야 하므로
		* 전치행렬을 이용해서 정보를 변경해줍니다.
		*/
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CTool_Bone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];

			if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendIndex.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendIndex.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendIndex.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendIndex.w)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}

	// 같이 Load된 모델에 Bone이 없는경우도 존재함.
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](CTool_Bone* pBone) {
			if (true == pBone->Compare_Name(m_szName))
				return true;

			iBoneIndex++;

			return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	/* 여기서 저장할 정보를 Emplace Back 한다. */
	m_Vertices.reserve(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_Vertices.emplace_back(pVertices[i]);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;


	Safe_Delete_Array(pVertices);

	return S_OK;
}


CTool_Mesh* CTool_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const aiMesh* pMesh, const vector<CTool_Bone*>& Bones, _fmatrix PreTransformMatrix)
{
	CTool_Mesh* pInstance = new CTool_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pMesh, Bones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CTool_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTool_Mesh::Clone(void* pArg)
{
	CTool_Mesh* pInstance = new CTool_Mesh(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTool_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Mesh::Free()
{
	__super::Free();
}
