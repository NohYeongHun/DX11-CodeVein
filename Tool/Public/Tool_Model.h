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
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath, const _char* pTextureFolderPath);
	virtual HRESULT Initialize_Clone(void* pArg);
	HRESULT Render(_uint iNumMesh);

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	MODELTYPE Get_ModelType() const {
		return m_ModelType;
	}

	
#pragma region 저장용 함수.
public:
	const MODEL_INFO& Save_NonAminModel(_fmatrix PreTransformMatrix, _wstring pModelTag);
	
private:
	HRESULT Save_NonAnimMeshes(_fmatrix PreTransformMatrix);
	HRESULT Save_NonAnimMarterials();


/* 애니메이션 */
public:
	void Save_AnimModel(ANIMMODEL_INFO& AnimModelInfo, _fmatrix PreTransformMatrix);


private:
	HRESULT Save_AnimMeshes(ANIMMODEL_INFO& AnimModelInfo, _fmatrix PreTransformMatrix);
	HRESULT Save_AnimMaterials(ANIMMODEL_INFO& AnimModelInfo);
	HRESULT Save_Bones(ANIMMODEL_INFO& AnimModelInfo);

#pragma endregion

public:
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);




public:
	HRESULT Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	
	void Play_Animation(_float fTimeDelta);



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
	MODEL_INFO m_NonAnimModelInfo = {};

	string m_ModelDir = {};


private:
	HRESULT Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT Ready_Materials(const _char* pModelFilePath, const _char* pTextureFolderPath);
	HRESULT Ready_Bones(const aiNode* pAiNode, _int iParentBoneIndex);


public:
	
	static CTool_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath, const _char* pTextureFolderPath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

