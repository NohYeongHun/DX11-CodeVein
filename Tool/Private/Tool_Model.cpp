#include "Tool_Model.h"

CTool_Model::CTool_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CTool_Model::CTool_Model(const CTool_Model& Prototype)
    : CComponent ( Prototype )
	, m_pAIScene { Prototype.m_pAIScene }
	, m_Meshes { Prototype.m_Meshes }
	, m_ModelType(Prototype.m_ModelType)
	, m_Materials { Prototype.m_Materials}
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_Bones{ Prototype.m_Bones }
	, m_ModelDir{ Prototype.m_ModelDir }
{
	for (auto& mesh : m_Meshes)
		Safe_AddRef(mesh);

	for (auto& material : m_Materials)
		Safe_AddRef(material);

	for (auto& bone : m_Bones)
		Safe_AddRef(bone);
}

const MAP_PART_INFO& CTool_Model::Save_ModelInfo(_fmatrix PreTransformMatrix, _wstring strModelTag)
{
	m_ModelInfo.strModelTag = strModelTag;
	m_ModelInfo.meshVectorSize = m_pAIScene->mNumMeshes;
	m_ModelInfo.meshVector.resize(m_pAIScene->mNumMeshes);
	m_ModelInfo.materialVectorSize = m_pAIScene->mNumMaterials;
	m_ModelInfo.materialVector.resize(m_pAIScene->mNumMaterials);
	

	// 공간 할당.
	if (FAILED(Save_Meshes(PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Mesh Save Failed"));
		return m_ModelInfo;
	}

	if (FAILED(Save_Marterials()))
	{
		MSG_BOX(TEXT("Material Save Failed"));
		return m_ModelInfo;
	}

	return m_ModelInfo;
}

HRESULT CTool_Model::Save_Meshes(_fmatrix PreTransformMatrix)
{
	for (_uint i = 0; i < m_pAIScene->mNumMeshes; i++)
	{
		/* 1. Mesh 정보 넣어주기. */
		aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
		MESH_INFO& dst = m_ModelInfo.meshVector[i];

		// (1) 기본 메타
		dst.iMarterialIndex = pAIMesh->mMaterialIndex;
		dst.iVertexCount = pAIMesh->mNumVertices;
		dst.iIndicesCount = pAIMesh->mNumFaces * 3;

		// (2) 정점 벡터 크기 확보
		dst.vertices.resize(dst.iVertexCount);

		// (3) 정점에 값 채워넣기.
		for (uint32_t v = 0; v < dst.iVertexCount; ++v)
		{
			// 모델 생성시 Transform 설정.
			memcpy(&dst.vertices[v].vPosition, &pAIMesh->mVertices[v], sizeof(_float3));
			XMStoreFloat3(&dst.vertices[v].vPosition, XMVector3TransformCoord(XMLoadFloat3(&dst.vertices[v].vPosition), PreTransformMatrix));
			memcpy(&dst.vertices[v].vNormal, &pAIMesh->mNormals[v], sizeof(_float3));
			XMStoreFloat3(&dst.vertices[v].vNormal, XMVector3TransformNormal(XMLoadFloat3(&dst.vertices[v].vNormal), PreTransformMatrix));

			memcpy(&dst.vertices[v].vTangent, &pAIMesh->mTangents[v], sizeof(_float3));
			memcpy(&dst.vertices[v].vBinormal, &pAIMesh->mBitangents[v], sizeof(_float3));
			memcpy(&dst.vertices[v].vTexcoord, &pAIMesh->mTextureCoords[0][v], sizeof(_float2));
		}

		// (4) 인덱스 벡터
		dst.indices.resize(dst.iIndicesCount);
		uint32_t idx = 0;
		for (uint32_t f = 0; f < pAIMesh->mNumFaces; ++f)
		{
			const aiFace& face = pAIMesh->mFaces[f]; // always 3개
			dst.indices[idx++] = face.mIndices[0];
			dst.indices[idx++] = face.mIndices[1];
			dst.indices[idx++] = face.mIndices[2];
		}
	}

	return S_OK;
}

HRESULT CTool_Model::Save_Marterials()
{
	_uint iNumMaterials = m_pAIScene->mNumMaterials;

	_char			szModelDir[MAX_PATH] = {};
	_splitpath_s(m_ModelDir.c_str(), nullptr, 0, szModelDir, MAX_PATH, nullptr, 0, nullptr, 0);

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		// Material 정보를 저장할 구조체.
		MATERIAL_INFO& dst = m_ModelInfo.materialVector[i];
		dst.materialPathVectorSize = AI_TEXTURE_TYPE_MAX;
		
		// AIScene에서 만든 Material
		const aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		// Material 정보를 담을 벡터.
		vector<_wstring>& pathVector = m_ModelInfo.materialVector[i].materialPathVector;
		
		pathVector.resize(AI_TEXTURE_TYPE_MAX);

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiTextureType textureType = static_cast<aiTextureType>(j);
			_uint iNumtextures = pAIMaterial->GetTextureCount(textureType);

			aiString strTexturePath;

			if (iNumtextures == 0)
				continue;
			
			// TextureType DIFFUSE, AMBIENT 등등..
			if (FAILED(pAIMaterial->GetTexture(textureType, 0, &strTexturePath)))
				break;

			_char			szFullPath[MAX_PATH] = {};
			_char			szDrive[MAX_PATH] = {};
			_char			szDir[MAX_PATH] = {};
			_char			szFileName[MAX_PATH] = {};
			_char			szExt[MAX_PATH] = {};

			_splitpath_s(strTexturePath.data, nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
			// 총 필요한 경로. ModelPath도 필요함.

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szModelDir);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);
			
			_tchar			szTextureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFilePath, MAX_PATH);

			pathVector[j] = szTextureFilePath;
		}
	}

	return S_OK;
}

const _bool CTool_Model::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist)
{
	_bool IsHit = false;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		IsHit = m_Meshes[i]->Is_Ray_Hit(rayOrigin, rayDir, pOutDist);
		if (IsHit)
			return true;
	}

	return false;
}

HRESULT CTool_Model::Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath, const _char* pTextureFolderPath)
{
	
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_ModelType = eModelType;

	if (MODELTYPE::NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices; // Animation 사용하지 않는 경우.

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	m_ModelDir = pModelFilePath;
	std::replace(m_ModelDir.begin(), m_ModelDir.end(), '/', '\\');

	if (FAILED(Ready_Meshes(PreTransformMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath, pTextureFolderPath)))
		return E_FAIL;

	

    return S_OK;
}

HRESULT CTool_Model::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CTool_Model::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iTextureIndex);
}

HRESULT CTool_Model::Render(_uint iNumMesh)
{
	if (FAILED(m_Meshes[iNumMesh]->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Meshes[iNumMesh]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Model::Ready_Meshes(_fmatrix PreTransformMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		//string szPath = m_pAIScene->mMeshes[i]->mName.data;
		//_wstring MeshName(szPath.begin(), szPath.end());
		//MeshName += L" : " + to_wstring(i);
		//MSG_BOX(MeshName.c_str());

		CTool_Mesh* pMesh = CTool_Mesh::Create(m_pDevice, m_pContext, m_ModelType, m_pAIScene->mMeshes[i], PreTransformMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CTool_Model::Ready_Materials(const _char* pModelFilePath, const _char* pTextureFolderPath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CTool_MeshMaterial* pMaterial = CTool_MeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, pTextureFolderPath, m_pAIScene->mMaterials[i], m_pAIScene);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CTool_Model::Ready_Bones(const aiNode* pAiNode, _int iParentBoneIndex)
{
	CTool_Bone* pBone = CTool_Bone::Create(pAiNode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iOwnBoneIndex = m_Bones.size() - 1; // 자식 객체에 부여할 부모 인덱스는 자신의 인덱스.

	for (_uint i = 0; i < pAiNode->mNumChildren; i++)
	{
		if (FAILED(Ready_Bones(pAiNode->mChildren[i], iOwnBoneIndex)))
			return E_FAIL;
	}

	return S_OK;
}



CTool_Model* CTool_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath, const _char* pTextureFolderPath)
{

	CTool_Model* pInstance = new CTool_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PreTransformMatrix, pModelFilePath, pTextureFolderPath)))
	{
		MSG_BOX(TEXT("Failed to Created : CTool_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTool_Model::Clone(void* pArg)
{
	CTool_Model* pInstance = new CTool_Model(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTool_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Model::Free()
{
	__super::Free();
	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);

	m_Materials.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();
	
	//Safe_Delete(m_pAIScene);
	m_Importer.FreeScene();
}
