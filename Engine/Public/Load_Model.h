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
#pragma region GET_SET
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	void Set_Animation(_uint iAnimIndex, _bool isLoop = false) {
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

	const _bool Is_Blending()
	{
		return m_isBlending;
	}

	_uint Get_CurrentAnimationIndex()
	{
		return m_iCurrentAnimIndex;
	}

	const _float Get_CurrentTrackPosition() { return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition(); }
	void Set_CurrentTrackPosition(_float fTrackPosition) { m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fTrackPosition); }
	void Set_AnimationTrackPosition(_uint iAnimIndex, _float fTrackPosition) { m_Animations[iAnimIndex]->Set_CurrentTrackPosition(fTrackPosition); }

	_matrix Get_RootBoneMatrix()
	{
		return m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
	}

	_float4x4* Get_BoneMatrix(const _char* pBoneName);

	_uint Get_CurrentFrame();
#pragma endregion

	

public:
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);

	
public:
	HRESULT Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iTextureIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

public:
	/* 보간*/
	_bool Play_Animation(_float fTimeDelta);
	void Set_BlendInfo(uint32_t iNextAnimIndex, _float fBlendTime, _bool bScale, _bool bRotation, _bool bTranslation);
	_vector QuaternionSlerpShortest(_vector q1, _vector q2, _float t);


public:
	void Animation_Reset();
	
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

	_bool m_isBlending = { false };
	BLEND_DESC m_BlendDesc = {};

private:
	/* Root Bone */
	_uint m_iRoot_BoneIndex = { };
	_bool m_isTrackEnd = { }; // 애니메이션 한 프레임이 종료된 상태를 저장합니다.

	_bool m_bEnableRootMotion = true;           // 루트 모션 활성화 여부
	_float m_fRootMotionScale = 1.0f;           // 루트 모션 스케일
	_bool m_bEnableRootRotation = true;         // 루트 회전 활성화 여부

	_matrix m_oldMatrix = {};
	_float4 m_vOldPos = {};
	_float4 m_vSameOldPos = {};
	

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

