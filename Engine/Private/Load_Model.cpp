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
	, m_iNumAnimations { Prototype.m_iNumAnimations }
	, m_iRoot_BoneIndex {Prototype.m_iRoot_BoneIndex }
	, m_ModelDir{ Prototype.m_ModelDir }
{

	for (auto& pPrototypeAnimation : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());


	for (auto& mesh : m_Meshes)
		Safe_AddRef(mesh);

	for (auto& material : m_Materials)
		Safe_AddRef(material);

	

	
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

	// 소멸자 호출되면서 시간 출력.
	ScopedTimer allTimer(filePath.c_str());   // 전체 시간

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
	LOADMODEL_DESC* pDesc = static_cast<LOADMODEL_DESC*>(pArg);
	m_pOwner = pDesc->pGameObject;


	return S_OK;
}

HRESULT CLoad_Model::Render(_uint iNumMesh)
{
	if (FAILED(m_Meshes[iNumMesh]->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Meshes[iNumMesh]->Render()))
		return E_FAIL;

	return S_OK;
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

// 1. Player에서 호출.


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


_bool CLoad_Model::Play_Animation(_float fTimeDelta)
{
	static _bool isLoop = m_isLoop;

	if (m_BlendDesc.isBlending)
	{
		Blend_Animation(fTimeDelta);
		return false;
	}
	
	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
	m_isFinished = false;

	/* 현재 트랙이 종료되었는지 확인 */
	m_isTrackEnd = false;
	
	// Loop가 무한히 반복되는 경우에. 해당 값을 이용하여 확인한다. 
	
	_vector vOldRootPos = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix().r[3];

	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(
		m_Bones, m_isLoop, &m_isFinished, &m_isTrackEnd, fTimeDelta
	);

	_vector vNewRootPos = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix().r[3];

	if (!m_isTrackEnd)
		Apply_RootMotion(vOldRootPos, vNewRootPos);

	//// 2. 행렬이 변한 이후에 루트본의 matix를 가져온다.
	//_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
	////
	//
	////// 3. 차이값을 구한다.
	//_vector vTranslate = vNewRootPos - vOldRootPos;
	////
	//vTranslate = XMVector3TransformCoord(vTranslate, XMLoadFloat4x4(&m_PreTransformMatrix));
	//// 4. 만약 Track이 종료되었다면? => 위치를 바꿔주지않습니다.

	//if (!m_isTrackEnd)
	//{
	//	_float fLength = XMVectorGetX(XMVector3Length(vTranslate));
	//	if (fLength > 0.01f)  // 민감도 조정 가능
	//		m_pOwner->Translate(vTranslate);
	//}

	//// 5. 루트본을 이동값이 지워진 위치로 재갱신합니다.
	//rootMatrix.r[3] = vOldRootPos;
	//m_Bones[m_iRoot_BoneIndex]->Set_TransformationMatrix(rootMatrix);	

	/* 바꿔야할 뼈들의 Transforemation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombindTransformationMatrix를 만들어준다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	
	

	return m_isFinished;
}

void CLoad_Model::Blend_Animation(_float fTimeDelta)
{
	m_BlendDesc.fElapsed += fTimeDelta;
	
	// clamp용 시간.
	_float t = m_BlendDesc.fElapsed / m_BlendDesc.fBlendDuration;
	t = min(t, 1.f); // clamp

	// 1. 현재 실행되었던 애니메이션.
	const auto& prevAnim = m_Animations[m_BlendDesc.iPrevAnimIndex];

	// 2. 다음 실행 애니메이션 => 이거는 같은 뼈를 공유하는 애니메이션끼리만 가능하다.
	// 3. Idle이 존재해. 오른쪽 공격이 존재해
	// 4. Idle로 넘어간 시점에 Idle은 오른쪽 공격에 해당하는 프레임을 가지고 있어요.

	const auto& nextAnim = m_Animations[m_iCurrentAnimIndex];

	// 3. 실제 블랜딩 진행.
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		// 4. 이 시간대에 영향받는 모든 본을 가져온다.
		_matrix matPrev = prevAnim->Get_BoneMatrixAtTime(i, m_BlendDesc.fPrevAnimTime);
		_matrix matNext = nextAnim->Get_BoneMatrixAtTime(i, nextAnim->Get_CurrentTrackPosition());

		// Slerp or Lerp each component
		_vector prevScale, prevRot, prevTrans;
		_vector nextScale, nextRot, nextTrans;

		XMMatrixDecompose(&prevScale, &prevRot, &prevTrans, matPrev);
		XMMatrixDecompose(&nextScale, &nextRot, &nextTrans, matNext);

		// t값을 이용해서 보간합니다.
		_vector blendScale = XMVectorLerp(prevScale, nextScale, t);
		_vector blendRot = XMQuaternionSlerp(prevRot, nextRot, t);
		_vector blendTrans = XMVectorLerp(prevTrans, nextTrans, t);

		_matrix blendedMat = XMMatrixAffineTransformation(blendScale, XMVectorZero(), blendRot, blendTrans);
		m_Bones[i]->Set_TransformationMatrix(blendedMat);
	}

	// 블렌딩 시간이 다되면 Blending을 제거합니다.
	if (t >= 1.f)
	{
		m_BlendDesc.isBlending = false;
	}
	
}

// 1. Animation Blending을 위한 정보를 교체합니다.
void CLoad_Model::Change_Animation_WithBlend(uint32_t iNextAnimIndex, _float fBlendTime)
{
	m_BlendDesc.isBlending = true;
	m_BlendDesc.fElapsed = 0.f;
	m_BlendDesc.fBlendDuration = fBlendTime;

	// 1. 현재 실행되고 있는 애니메이션 인덱스 담기.
	m_BlendDesc.iPrevAnimIndex = m_iCurrentAnimIndex;
	// 2. 현재 실행되고 있는 애니메이션의 시간 담기. => Key프레임을 가져 와야합니다.
	m_BlendDesc.fPrevAnimTime = m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
	// 3. 다음 실행될 애니메이션.
	m_BlendDesc.iNextAnimIndex = iNextAnimIndex;

	// 4. Animation Index를 변경.
	m_iCurrentAnimIndex = iNextAnimIndex;
	m_Animations[iNextAnimIndex]->Reset();
	m_isTrackEnd = false;
	m_isFinished = false;
}

void CLoad_Model::Apply_RootMotion(_vector vOld, _vector vNew)
{
	_vector vTranslate = vNew - vOld;

	// 모델이 PreTransformMatrix를 갖고 있으면 이를 반영
	vTranslate = XMVector3TransformCoord(vTranslate, XMLoadFloat4x4(&m_PreTransformMatrix));

	// 민감도 체크
	_float fLength = XMVectorGetX(XMVector3Length(vTranslate));
	if (fLength > 0.01f)
		m_pOwner->Translate(vTranslate);

	// 루트본의 행렬 되돌리기
	_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
	rootMatrix.r[3] = vOld;
	m_Bones[m_iRoot_BoneIndex]->Set_TransformationMatrix(rootMatrix);
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

		if (pLoadBone->Compare_Name("Hips")) 
			m_iRoot_BoneIndex = i;
		


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
	m_pOwner = nullptr;
		
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
