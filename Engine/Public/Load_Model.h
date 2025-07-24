#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLoad_Model final : public CComponent
{
public:
	typedef struct tagLoadModelDesc
	{
		class CGameObject* pGameObject = { nullptr };
	}LOADMODEL_DESC;

private:
	explicit CLoad_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLoad_Model(const CLoad_Model& Prototype);
	virtual ~CLoad_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filePath, _wstring textureFolderPath);
	virtual HRESULT Initialize_Clone(void* pArg);
	HRESULT Render(_uint iNumMesh);

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	void Set_Animation(_uint iAnimIndex, _bool isLoop = false) {
		m_bFirstFrame = true;
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

	void Set_Loop(_bool isLoop)
	{
		m_isLoop = isLoop;
	}

	const _bool Is_Finished()
	{
		return m_isFinished;
	}
	
public:
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);

	
public:
	HRESULT Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);
	
private:
	MODELTYPE m_ModelType = {};
	_float4x4 m_PreTransformMatrix = {};

private:
	class CGameObject* m_pOwner = { nullptr };

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
	/* Animations */
	_bool m_isFinished = { false };
	_uint m_iCurrentAnimIndex = { 0 };
	_bool m_isLoop = { false };
	_uint m_iNumAnimations = { 0 };
	vector<class CLoad_Animation*> m_Animations;

private:
	// RootNode Index;
	_bool m_IsTrackEnd = { false };
	_bool m_bFirstFrame = { true };
	_float4x4 m_gPrevRootTM = {};

	_uint m_iRoot_BoneIndex = {};
	_float3 m_vPrevRootPos = { 0.f, 0.f, 0.f};
	

private:
	string m_ModelDir = {};


private:
	HRESULT Load_Meshes(_fmatrix PreTransformMatrix, std::ifstream& ifs);
	HRESULT Load_Materials( std::ifstream& ifs, _wstring textureFolderPath);
	HRESULT Load_Bones(std::ifstream& ifs);
	HRESULT Load_Animations(std::ifstream& ifs);




public:
	static CLoad_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, string filepath, _wstring textureFolderPath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

