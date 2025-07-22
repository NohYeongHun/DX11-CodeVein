#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& Prototype);
	virtual ~CModel() = default;


public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg);
	HRESULT Render(_uint iNumMesh);


public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
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
	vector<class CMesh*> m_Meshes;

private:
	/* Materials */
	_uint m_iNumMaterials = {};
	vector<class CMeshMaterial*> m_Materials;

private:
	/* Bones */
	_uint m_iNumBones = {};
	vector<class CBone*> m_Bones;

private:
	/* Animations */
	_uint m_iCurrentAnimIndex = { 0 };
	_uint m_iNumAnimations = { 0 };
	vector<class CAnimation*> m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAiNode, _int iParentBoneIndex);
	HRESULT Ready_Animations();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

