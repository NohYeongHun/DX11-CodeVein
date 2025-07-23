#include "Load_Model.h"

CLoad_Model::CLoad_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CLoad_Model::CLoad_Model(const CLoad_Model& Prototype)
    : CComponent ( Prototype )
	, m_Meshes { Prototype.m_Meshes }
	, m_ModelType(Prototype.m_ModelType)
	, m_Materials { Prototype.m_Materials}
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_Bones{ Prototype.m_Bones }
	, m_ModelDir{ Prototype.m_ModelDir }
	, m_Animations { Prototype.m_Animations }
	, m_iNumAnimations { Prototype.m_iNumAnimations }
	, m_iCurrentAnimIndex { Prototype.m_iCurrentAnimIndex }
{
	for (auto& mesh : m_Meshes)
		Safe_AddRef(mesh);

	for (auto& material : m_Materials)
		Safe_AddRef(material);

	for (auto& bone : m_Bones)
		Safe_AddRef(bone);

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
}


const _bool CLoad_Model::Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist)
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

HRESULT CLoad_Model::Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filePath, _wstring textureFolderPath)
{
	std::ifstream ifs(filePath, std::ios::binary);
	if (!ifs.is_open())
		return E_FAIL;

	// Model 타입 지정.
	m_ModelType = eModelType;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	/* ---------- ModelTag ---------- */

	// 1. Model Tag 읽어오기.
	_wstring strModelTag = ReadWString(ifs);

	if (strModelTag.empty())
		CRASH("MODEL EMPTY");

	ScopedTimer allTimer("Initialize_Prototype");   // 전체 시간

	{
		ScopedTimer LoadMesh("Load_Meshes");
		if (FAILED(Load_Meshes(PreTransformMatrix, ifs)))
			CRASH("LOAD MESH FAILED");
	}

	
	{
		ScopedTimer LoadMaterial("Load_Materials");
		if (FAILED(Load_Materials(ifs, textureFolderPath)))
			CRASH("LOAD MATERIALS FAILED");
	}

	{
		ScopedTimer LoadBone("Load_Bones");
		if (FAILED(Load_Bones(ifs)))
			CRASH("LOAD BONES FAILED");
	}

	{
		ScopedTimer LoadAnimation("Load_Animations");
		if (FAILED(Load_Animations(ifs)))
			CRASH("LOAD ANIMATION FAILED");
	}

	// Load가 끝났다면 삭제.
	ifs.close();
	

    return S_OK;
}

HRESULT CLoad_Model::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CLoad_Model::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iTextureIndex);
}

HRESULT CLoad_Model::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

void CLoad_Model::Play_Animation(_float fTimeDelta)
{
	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */


   /* 바꿔야할 뼈들의 Transforemation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombindTransformationMatrix를 만들어준다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	}
}

HRESULT CLoad_Model::Render(_uint iNumMesh)
{
	if (FAILED(m_Meshes[iNumMesh]->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Meshes[iNumMesh]->Render()))
		return E_FAIL;

	return S_OK;
}



HRESULT CLoad_Model::Load_Meshes(_fmatrix PreTransformMatrix, std::ifstream& ifs)
{

	/* ---------- Mesh ---------- */

	// 2. meshVectorsize 읽어오기.
	uint32_t meshVectorSize = {};
	ifs.read(reinterpret_cast<char*>(&meshVectorSize), sizeof(uint32_t));

	for (uint32_t i = 0; i < meshVectorSize; ++i)
	{
		// 3. Vector size만큼 순회 돌리면서 Mesh 생성.
		CLoad_Mesh* pLoadMesh = CLoad_Mesh::Create(m_pDevice, m_pContext, m_ModelType, PreTransformMatrix, ifs);
		if (nullptr == pLoadMesh)
			CRASH("LOAD MESH NULLPTR");

		m_Meshes.emplace_back(pLoadMesh);
	}

	m_iNumMeshes = m_Meshes.size();

	return S_OK;
}

/* 각 모델마다 텍스쳐 폴더를 다르게 저장할 것. => 구별하기 위해서. */
HRESULT CLoad_Model::Load_Materials(std::ifstream& ifs, _wstring textureFolderPath)
{
	/* ---------- Material ---------- */

	// 3. Material 값 읽어오기.
	uint32_t materialVectorSize = {};
	ifs.read(reinterpret_cast<char*>(&materialVectorSize), sizeof(uint32_t));

	for (uint32_t i = 0; i < materialVectorSize; ++i)
	{
		// 4. Vector size만큼 순회 돌리면서 Material 생성.
		CLoad_MeshMaterial* pLoadMesh = CLoad_MeshMaterial::Create(m_pDevice, m_pContext, ifs, textureFolderPath);
		if (nullptr == pLoadMesh)
			CRASH("LOAD MESH");

		m_Materials.emplace_back(pLoadMesh);
	}

	m_iNumMaterials = m_Materials.size();

	return S_OK;
}

HRESULT CLoad_Model::Load_Bones(std::ifstream& ifs)
{
	/* ------------ Bones -------------*/

	// 4. Bone 값 읽어오기.
	uint32_t boneVectorSize = {};
	ifs.read(reinterpret_cast<char*>(&boneVectorSize), sizeof(uint32_t));

	for (uint32_t i = 0; i < boneVectorSize; ++i)
	{
		// 5. 순회하면서 값채워넣기.
		CLoad_Bone* pLoadBone = CLoad_Bone::Create(ifs);
		if (nullptr == pLoadBone)
			CRASH("LOAD BONE FAILED");

		m_Bones.emplace_back(pLoadBone);
	}

	m_iNumBones = m_Bones.size();
	return S_OK;
}

/* 1. Animation Load */
HRESULT CLoad_Model::Load_Animations(std::ifstream& ifs)
{
	uint32_t iCurrentAnimIndex = {};
	ifs.read(reinterpret_cast<char*>(&iCurrentAnimIndex), sizeof(uint32_t));

	uint32_t iNumAnimations = {};
	ifs.read(reinterpret_cast<char*>(&iNumAnimations), sizeof(uint32_t));

	for (uint32_t i = 0; i < iNumAnimations; ++i)
	{
		CLoad_Animation* pAnimation = CLoad_Animation::Create(ifs);
		if (nullptr == pAnimation)
			CRASH("LOAD ANIMATION FAILED");

		m_Animations.emplace_back(pAnimation);
	}

	m_iNumAnimations = iNumAnimations;

	return S_OK;
}

CLoad_Model* CLoad_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filepath, _wstring textureFloderPath)
{
	CLoad_Model* pInstance = new CLoad_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PreTransformMatrix, filepath, textureFloderPath)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoad_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CLoad_Model::Clone(void* pArg)
{
	CLoad_Model* pInstance = new CLoad_Model(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoad_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoad_Model::Free()
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
}
