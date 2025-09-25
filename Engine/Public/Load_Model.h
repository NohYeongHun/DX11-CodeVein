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


#pragma region GET_SET
public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	void Set_Animation(_uint iAnimIndex, _bool isLoop = false);

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

	const _float Get_TrackPosition(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_CurrentTrackPosition(); }
	const _float Get_Duration(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_Duration(); }
	const _float Get_CurrentTrackPosition() { return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition(); }
	const _float Get_CurrentDuration() { return m_Animations[m_iCurrentAnimIndex]->Get_Duration(); }
	const _float Get_AnimationDuration(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_Duration(); }
	const _float Get_AnimationTickPersecond(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_TickPerSecond(); }
	void Set_CurrentTrackPosition(_float fTrackPosition) { m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fTrackPosition); }
	void Set_AnimationTrackPosition(_uint iAnimIndex, _float fTrackPosition) { m_Animations[iAnimIndex]->Set_CurrentTrackPosition(fTrackPosition); }

	_matrix Get_RootBoneMatrix()
	{
		return m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
	}

	_float4x4* Get_BoneMatrix(const _char* pBoneName);
	const _float4x4* Get_LocalBoneMatrix(const _char* pBoneName);

	_uint Get_CurrentFrame();

	_float Get_Current_Ratio();
	
	// 모델의 위 아래 길이 계산해서 반환.

	_float Get_AnimSpeed(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_AnimSpeed(); }
	void Set_AnimSpeed(_uint iAnimIndex, _float fSpeed) { m_Animations[iAnimIndex]->Set_AnimSpeed(fSpeed); }

	

#pragma endregion

	
#pragma region 0. 모델의 바운딩박스 구하기.
	// (x, -x, y, -y, z, -z) 구하기.
	// 
public:
	// 바운딩 박스 관련 함수들
	BOUNDING_BOX Get_BoundingBox() const { return m_BoundingBox; }
	_float Get_ModelHeight() const { return m_BoundingBox.fHeight; }
	_float Get_ModelTopY() const { return m_BoundingBox.vMax.y; }
	_float Get_ModelBottomY() const { return m_BoundingBox.vMin.y; }
	_float3 Get_ModelCenter() const { return m_BoundingBox.vCenter; }

private:
	BOUNDING_BOX m_BoundingBox = {};



private:
	void Calculate_Bounding_Box(); // Initialize 시점에 한번만 구합니다.
	void Rotate_Bounding_Box();

public:
	void Set_AnimationActivate() { m_IsStopAnimation = false; }
	void Set_AnimationDeActivate() { m_IsStopAnimation = true; }
	

private:
	_bool m_IsStopAnimation = { false };
	
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

	void Set_RootMotionRotation(_bool bRootRotation = false) { m_IsRootMotionRotate = bRootRotation; }
	void Set_RootMotionTranslate(_bool bRootTranslate = true) { m_IsRootMotionTranslate = bRootTranslate; }

public:
	void Animation_Reset();
	void Set_CurrentTickPerSecond(_uint iAnimIndex, _float fTickPerSecond) {
		m_Animations[iAnimIndex]->Set_TickPerSecond(fTickPerSecond); 
	}
	_float Get_CurrentTickPerSecond(_uint iAnimIndex) { return m_Animations[iAnimIndex]->Get_TickPerSecond(); }

#pragma region ROOT MOTION
private:
	void Handle_RootMotion(_float fTimeDelta);
	
#pragma endregion



	
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

	_bool m_IsEnableRootMotion = true;           // 루트 모션 활성화 여부
	//_float m_fRootMotionScale = 1.0f;           // 루트 모션 스케일
	_bool m_IsEnableRootRotation = true;         // 루트 회전 활성화 여부

	_matrix m_oldMatrix = {};
	_float4 m_vOldPos = {};
	_float4 m_vSameOldPos = {};
	

	_bool m_IsRootMotionRotate = { true };
	_bool m_IsRootMotionTranslate = { true };
	_bool m_IsAnimationJustChanged = { false };

private:
	string m_ModelDir = {};

	
private:
	// 디버깅용 임시변수
	_float4 m_fCurDodgePos = {};
	_float4 m_fNextDodgePos = {};


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

