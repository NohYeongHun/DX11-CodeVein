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
	, m_ModelDir{ Prototype.m_ModelDir }
	, m_iCurrentAnimIndex { Prototype.m_iCurrentAnimIndex }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations { Prototype.m_Animations}
{
	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& mesh : m_Meshes)
		Safe_AddRef(mesh);

	for (auto& material : m_Materials)
		Safe_AddRef(material);

	
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

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		CRASH("READY BONE FAILED");

	if (FAILED(Ready_Meshes(PreTransformMatrix)))
		CRASH("READY MESH FAILED");

	if (FAILED(Ready_Materials(pModelFilePath, pTextureFolderPath)))
		CRASH("READY MARTERIAL FAILED");

	if (FAILED(Ready_Animations()))
		CRASH("READY ANIMATION FAILED");



	return S_OK;
}

HRESULT CTool_Model::Initialize_Clone(void* pArg)
{
	return S_OK;
}


//#pragma region NONANIM MODEL
//const MODEL_INFO& CTool_Model::Save_NonAminModel(_fmatrix PreTransformMatrix, _wstring strModelTag)
//{
//	m_NonAnimModelInfo.strModelTag = strModelTag;
//	m_NonAnimModelInfo.meshVectorSize = m_pAIScene->mNumMeshes;
//	m_NonAnimModelInfo.meshVector.resize(m_pAIScene->mNumMeshes);
//	m_NonAnimModelInfo.materialVectorSize = m_pAIScene->mNumMaterials;
//	m_NonAnimModelInfo.materialVector.resize(m_pAIScene->mNumMaterials);
//
//
//	// 공간 할당.
//	if (FAILED(Save_NonAnimMeshes(PreTransformMatrix)))
//	{
//		MSG_BOX(TEXT("Mesh Save Failed"));
//		return m_NonAnimModelInfo;
//	}
//
//	if (FAILED(Save_NonAnimMarterials()))
//	{
//		MSG_BOX(TEXT("Material Save Failed"));
//		return m_NonAnimModelInfo;
//	}
//
//	return m_NonAnimModelInfo;
//}
//
//
//HRESULT CTool_Model::Save_NonAnimMeshes(_fmatrix PreTransformMatrix)
//{
//	for (_uint i = 0; i < m_pAIScene->mNumMeshes; i++)
//	{
//		/* 1. Mesh 정보 넣어주기. */
//		aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
//		MESH_INFO& dst = m_NonAnimModelInfo.meshVector[i];
//
//		// (1) 기본 메타
//		dst.iMarterialIndex = pAIMesh->mMaterialIndex;
//		dst.iVertexCount = pAIMesh->mNumVertices;
//		dst.iIndicesCount = pAIMesh->mNumFaces * 3;
//
//		// (2) 정점 벡터 크기 확보
//		dst.vertices.resize(dst.iVertexCount);
//
//		// (3) 정점에 값 채워넣기.
//		for (uint32_t v = 0; v < dst.iVertexCount; ++v)
//		{
//			// 모델 생성시 Transform 설정.
//			memcpy(&dst.vertices[v].vPosition, &pAIMesh->mVertices[v], sizeof(_float3));
//			XMStoreFloat3(&dst.vertices[v].vPosition, XMVector3TransformCoord(XMLoadFloat3(&dst.vertices[v].vPosition), PreTransformMatrix));
//			memcpy(&dst.vertices[v].vNormal, &pAIMesh->mNormals[v], sizeof(_float3));
//			XMStoreFloat3(&dst.vertices[v].vNormal, XMVector3TransformNormal(XMLoadFloat3(&dst.vertices[v].vNormal), PreTransformMatrix));
//
//			memcpy(&dst.vertices[v].vTangent, &pAIMesh->mTangents[v], sizeof(_float3));
//			memcpy(&dst.vertices[v].vBinormal, &pAIMesh->mBitangents[v], sizeof(_float3));
//			memcpy(&dst.vertices[v].vTexcoord, &pAIMesh->mTextureCoords[0][v], sizeof(_float2));
//		}
//
//		// (4) 인덱스 벡터
//		dst.indices.resize(dst.iIndicesCount);
//		uint32_t idx = 0;
//		for (uint32_t f = 0; f < pAIMesh->mNumFaces; ++f)
//		{
//			const aiFace& face = pAIMesh->mFaces[f]; // always 3개
//			dst.indices[idx++] = face.mIndices[0];
//			dst.indices[idx++] = face.mIndices[1];
//			dst.indices[idx++] = face.mIndices[2];
//		}
//	}
//
//	return S_OK;
//}
//
//#pragma endregion

//
//HRESULT CTool_Model::Save_NonAnimMarterials()
//{
//	_uint iNumMaterials = m_pAIScene->mNumMaterials;
//
//	_char			szModelDir[MAX_PATH] = {};
//	_splitpath_s(m_ModelDir.c_str(), nullptr, 0, szModelDir, MAX_PATH, nullptr, 0, nullptr, 0);
//
//	for (_uint i = 0; i < m_iNumMaterials; i++)
//	{
//		// Material 정보를 저장할 구조체.
//		MATERIAL_INFO& dst = m_NonAnimModelInfo.materialVector[i];
//		dst.materialPathVectorSize = AI_TEXTURE_TYPE_MAX;
//		
//		// AIScene에서 만든 Material
//		const aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
//
//		// Material 정보를 담을 벡터.
//		vector<_wstring>& pathVector = m_NonAnimModelInfo.materialVector[i].materialPathVector;
//		
//		pathVector.resize(AI_TEXTURE_TYPE_MAX);
//
//		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
//		{
//			aiTextureType textureType = static_cast<aiTextureType>(j);
//			_uint iNumtextures = pAIMaterial->GetTextureCount(textureType);
//
//			aiString strTexturePath;
//
//			if (iNumtextures == 0)
//				continue;
//			
//			// TextureType DIFFUSE, AMBIENT 등등..
//			if (FAILED(pAIMaterial->GetTexture(textureType, 0, &strTexturePath)))
//				break;
//
//			_char			szFullPath[MAX_PATH] = {};
//			_char			szDrive[MAX_PATH] = {};
//			_char			szDir[MAX_PATH] = {};
//			_char			szFileName[MAX_PATH] = {};
//			_char			szExt[MAX_PATH] = {};
//
//			_splitpath_s(strTexturePath.data, nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
//			// 총 필요한 경로. ModelPath도 필요함.
//
//			strcpy_s(szFullPath, szDrive);
//			strcat_s(szFullPath, szModelDir);
//			strcat_s(szFullPath, szDir);
//			strcat_s(szFullPath, szFileName);
//			strcat_s(szFullPath, szExt);
//			
//			_tchar			szTextureFilePath[MAX_PATH] = {};
//			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFilePath, MAX_PATH);
//
//			pathVector[j] = szTextureFilePath;
//		}
//	}
//
//	return S_OK;
//}

#pragma region ANIM_MODEL

void CTool_Model::Save_AnimModel(ANIMMODEL_INFO& AnimModelInfo, _fmatrix PreTransformMatrix)
{
	
	if (FAILED(Save_Bones(AnimModelInfo)))
		CRASH("Save Bones Failed");
	
	if (FAILED(Save_AnimMeshes(AnimModelInfo, PreTransformMatrix)))
		CRASH("Save AnimMeshes Failed");
		
	if (FAILED(Save_AnimMaterials(AnimModelInfo)))
		CRASH("Save AnimMaterials Failed");

	if (FAILED(Save_AnimationInfo(AnimModelInfo)))
		CRASH("Save AnimationInfo Failed")

	return;
}

HRESULT CTool_Model::Save_AnimMeshes(ANIMMODEL_INFO& AnimModelInfo, _fmatrix PreTransformMatrix)
{
	// 1. Mesh vector size 설정.
	AnimModelInfo.meshVectorSize = m_iNumMeshes;
	AnimModelInfo.meshVector.reserve(m_iNumMeshes);
	
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		vector<ANIMMESH_INFO>& meshVector = AnimModelInfo.meshVector;

		ANIMMESH_INFO dst{};
		// 2. 채울 수 있는 정보를 Mesh에서 채워오기
		m_Meshes[i]->Save_AnimMeshes(m_pAIScene->mMeshes[i], dst);

		// 8. 최종 결과 저장.
		meshVector.emplace_back(dst);
	}
	
	return S_OK;
}

HRESULT CTool_Model::Save_AnimMaterials(ANIMMODEL_INFO& AnimModelInfo)
{
	// 1. Material 정보 채우기.
	AnimModelInfo.materialVectorSize = m_iNumMaterials;
	AnimModelInfo.materialVector.reserve(m_iNumMaterials);

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		vector<MATERIAL_INFO>& materialVector = AnimModelInfo.materialVector;

		MATERIAL_INFO dst{};
		// 2. 채울 수 있는 정보를 material에서 채워오기.
		m_Materials[i]->Save_Materials(m_ModelDir.c_str(), m_pAIScene->mMaterials[i], dst);

		materialVector.emplace_back(dst);
	}

	return S_OK;
}

HRESULT CTool_Model::Save_Bones(ANIMMODEL_INFO& AnimModelInfo)
{
	// 1. size 채워주기.
	AnimModelInfo.boneVectorSize = m_Bones.size();
	AnimModelInfo.boneVector.reserve(m_Bones.size());

	// 2. 본정보 추가.
	for (auto& pBone : m_Bones)
	{
		BONE_INFO boneInfo = {};

		// Bone 벡터를 순회하면서 필요한 정보를 넣어줍니다.
		pBone->Save_Bones(boneInfo);
		// 채워진 정보를 Vector에 추가해줍니다. => 새로운 객체 생성되서 추가됨.
		AnimModelInfo.boneVector.emplace_back(boneInfo);
	}

	return S_OK;
}
/* 
	uint32_t iCurrentAnimIndex;
    uint32_t iNumAnimations;
    vector<ANIMATION_INFO> animationVector;
*/
// 1. Animation 정보 저장.
HRESULT CTool_Model::Save_AnimationInfo(ANIMMODEL_INFO& AnimModelInfo)
{
	// 2. index, animation 개수 저장.
	AnimModelInfo.iCurrentAnimIndex = m_iCurrentAnimIndex;
	AnimModelInfo.iNumAnimations = m_iNumAnimations;

	// 3. 벡터 크기 설정.
	AnimModelInfo.animationVector.reserve(m_iNumAnimations);

	vector<ANIMATION_INFO>& animVector = AnimModelInfo.animationVector;

	// 4. vector에 Animation 정보 구조체 단위로 저장.
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		ANIMATION_INFO animInfo = {};
		m_Animations[i]->Save_Animation(animInfo);
		animVector.emplace_back(animInfo);
	}

	return S_OK;
}
#pragma endregion



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


HRESULT CTool_Model::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iTextureIndex);
}

HRESULT CTool_Model::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}


_bool CTool_Model::Play_Animation(_float fTimeDelta)
{
	m_isFinished = false;

	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	}

	return m_isFinished;
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
		CTool_Mesh* pMesh = CTool_Mesh::Create(m_pDevice, m_pContext, m_ModelType, m_pAIScene->mMeshes[i],m_Bones, PreTransformMatrix);
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

	//_wstring strBoneDebug = L"============= Current Bone Index : " + to_wstring(iOwnBoneIndex) + L" ========================\n";
	//OutputDebugString(strBoneDebug.c_str());

	for (_uint i = 0; i < pAiNode->mNumChildren; i++)
	{
		if (FAILED(Ready_Bones(pAiNode->mChildren[i], iOwnBoneIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTool_Model::Ready_Animations()
{
	//_wstring wstr = L"Animation : \n";
	//OutputDebugString(wstr.c_str());

	// 애니메이션 개수 저장
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		_wstring wAnimIndex = L"";
		wAnimIndex += to_wstring(i) + L". ";
		OutputDebugString(wAnimIndex.c_str());
		CTool_Animation* pAnimation = CTool_Animation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
		{
			CRASH("Animation Create Failed");
			return E_FAIL;
		}
		m_Animations.emplace_back(pAnimation);
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
	
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	//Safe_Delete(m_pAIScene);
	m_Importer.FreeScene();
}
