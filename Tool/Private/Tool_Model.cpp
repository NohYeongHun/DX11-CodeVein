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
{
	for (auto& mesh : m_Meshes)
		Safe_AddRef(mesh);

	for (auto& material : m_Materials)
		Safe_AddRef(material);
}

HRESULT CTool_Model::Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath)
{
	
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_ModelType = eModelType;

	if (MODELTYPE::NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices; // Animation 사용하지 않는 경우.

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	

	if (FAILED(Ready_Meshes(PreTransformMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

    return S_OK;
}

HRESULT CTool_Model::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CTool_Model::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex)
{
	// 1. Model에서 Material Index를 가져오고
	// 이거를 m_Materials MaterialIndex
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_Meshes.size()) 
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_Shader_Resource(pShader, pConstantName, eTextureType, iTextureIndex);
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

		CTool_Mesh* pMesh = CTool_Mesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], PreTransformMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CTool_Model::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		CTool_MeshMaterial* pMaterial = CTool_MeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i], m_pAIScene);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

CTool_Model* CTool_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath)
{

	CTool_Model* pInstance = new CTool_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PreTransformMatrix, pModelFilePath)))
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
	
	//Safe_Delete(m_pAIScene);

}
