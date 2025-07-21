#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CLoad_Model final : public CComponent
{
private:
	explicit CLoad_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLoad_Model(const CLoad_Model& Prototype);
	virtual ~CLoad_Model() = default;



public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	
public:
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filePath);
	virtual HRESULT Initialize_Clone(void* pArg);
	
public:
	HRESULT Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
	HRESULT Render(_uint iNumMesh);


private:
	MODELTYPE m_ModelType = {};
	_float4x4 m_PreTransformMatrix = {};

private:
	/* Meshes */
	_uint m_iNumMeshes = {};
	vector<class CLoad_Mesh*> m_Meshes;

private:
	/* Materials */
	_uint m_iNumMaterials = {};
	vector<class CLoad_MeshMaterial*> m_Materials;

private:
	/* Bones */
	_uint m_iNumBones = {};
	vector<class CLoad_Bone*> m_Bones;

private:
	string m_ModelDir = {};


private:
	HRESULT Load_Meshes(_fmatrix PreTransformMatrix, std::ifstream& ifs);
	HRESULT Load_Materials( std::ifstream& ifs);
	HRESULT Load_Bones(std::ifstream& ifs);




public:
	static CLoad_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filepath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

