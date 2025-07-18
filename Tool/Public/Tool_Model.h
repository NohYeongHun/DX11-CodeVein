#pragma once
#include "Component.h"

NS_BEGIN(Tool)
class CTool_Model final : public CComponent
{
private:
	explicit CTool_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTool_Model(const CTool_Model& Prototype);
	virtual ~CTool_Model() = default;


public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg);



public:
	HRESULT Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
	HRESULT Render(_uint iNumMesh);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};

private:
	MODELTYPE m_ModelType = {};
	_float4x4 m_PreTransformMatrix = {};

private:
	/* Meshes */
	_uint m_iNumMeshes = {};
	vector<class CTool_Mesh*> m_Meshes;

private:
	/* Materials */
	_uint m_iNumMaterials = {};
	vector<class CTool_MeshMaterial*> m_Materials;

private:
	/* Bones */
	_uint m_iNumBones = {};
	vector<class CTool_Bone*> m_Bones;


private:
	HRESULT Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAiNode, _int iParentBoneIndex);


public:
	static CTool_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

