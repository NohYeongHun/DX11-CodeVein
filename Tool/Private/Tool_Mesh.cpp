#include "Tool_Mesh.h"

CTool_Mesh::CTool_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CTool_Mesh::CTool_Mesh(const CTool_Mesh& Prototype)
	: CVIBuffer( Prototype )
{
}

const _bool CTool_Mesh::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist)
{
	_bool bHit = false;
	_float fClosestDist = 1000.f;

	_fvector vRayOrigin = XMLoadFloat3(&rayOrigin);
	_fvector vRayDir = XMLoadFloat3(&rayDir);

	for (_uint i = 0; i < m_vecIndices.size(); i+= 3)
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

HRESULT CTool_Mesh::Initialize_Prototype(MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	// Material Index 설정.
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = (eModelType == MODELTYPE::NONANIM || eModelType == MODELTYPE::STATIC )
		? Initialize_Vertex_For_NonAnim(pAIMesh, PreTransformMatrix)
		: Initialize_Vertex_For_Anim(pAIMesh);




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

HRESULT CTool_Mesh::Initialize_Vertex_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

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

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CTool_Mesh::Initialize_Vertex_For_Anim(const aiMesh* pAIMesh)
{
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		// 모델 생성시 Transform 설정.
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pAIMesh->mNumBones;

	// 애니메이션용 본 인덱스와 가중치 설정.
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		const aiBone* pAiBone = pAIMesh->mBones[i];

		for (_uint j = 0; j < pAiBone->mNumWeights; j++)
		{
			const aiVertexWeight& AIVertexWeight = pAiBone->mWeights[j];

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

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
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



CTool_Mesh* CTool_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const aiMesh* pMesh, _fmatrix PreTransformMatrix)
{
	CTool_Mesh* pInstance = new CTool_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pMesh, PreTransformMatrix)))
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
