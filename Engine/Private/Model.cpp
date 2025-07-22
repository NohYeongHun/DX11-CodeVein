#include "Model.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent ( Prototype )
	, m_pAIScene { Prototype.m_pAIScene }
	, m_Meshes { Prototype.m_Meshes }
	, m_ModelType(Prototype.m_ModelType)
	, m_Materials { Prototype.m_Materials}
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_Bones{ Prototype.m_Bones }
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations { Prototype.m_Animations }
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_ModelType = eModelType;

	// PreTransform 저장. => 모델을 생성할 때. 지정해주어야할 PreTransformMatrix
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if (MODELTYPE::NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices; // Animation 사용하지 않는 경우.

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	
	/* 본 정보를 Mesh에서 활용해야 하므로 Bone 부터 정의한다. */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;
	

    return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CModel::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iTextureIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

void CModel::Play_Animation(_float fTimeDelta)
{
	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */


   /* 바꿔야할 뼈들의 Transforemation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombindTransformationMatrix를 만들어준다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	}
}

HRESULT CModel::Render(_uint iNumMesh)
{
	if (FAILED(m_Meshes[iNumMesh]->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Meshes[iNumMesh]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_ModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CMeshMaterial* pMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i], m_pAIScene);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAiNode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAiNode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iIndex = m_Bones.size() - 1; // 자식 객체에 부여할 부모 인덱스는 자신의 인덱스.

	for (_uint i = 0; i < pAiNode->mNumChildren; i++)
		Ready_Bones(pAiNode->mChildren[i], iIndex);

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	// 애니메이션 개수 저장
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
		{
			CRASH("Animation Create Failed");
			return E_FAIL;
		}
		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PreTransformMatrix, pModelFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
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
