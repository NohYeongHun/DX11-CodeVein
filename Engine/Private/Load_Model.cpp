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
	if (m_BlendDesc.isBlending)
	{
		Blend_Animation(fTimeDelta);
		return false; // 블렌딩 중에는 완료되지 않은 것으로 처리
	}

	// 애니메이션 유효성 검사
	if (m_iCurrentAnimIndex >= m_Animations.size())
	{
		OutputDebugString(L"[ERROR] Invalid current animation index\n");
		return true;
	}

	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
	m_isFinished = false;
	m_isTrackEnd = false;

	// 변경 전 Root Bone (루트 모션용)
	_matrix vOldRootMatrix = XMMatrixIdentity();
	if (m_iRoot_BoneIndex < m_Bones.size())
		vOldRootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();

	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(
		m_Bones, m_isLoop, &m_isFinished, &m_isTrackEnd, fTimeDelta
	);

	// 변경 이후 Root Bone 위치 (루트 모션용)
	_matrix vNewRootMatrix = XMMatrixIdentity();
	if (m_iRoot_BoneIndex < m_Bones.size())
		vNewRootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();

	// 루트 모션 적용 (필요시 주석 해제)
	 if (!m_isTrackEnd)
	     Apply_RootMotion(vOldRootMatrix, vNewRootMatrix);

	/* 바꿔야할 뼈들의 Transformation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombinedTransformationMatrix를 만들어준다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

	return m_isFinished;
}

// 1. 저번에 실행되던 애니메이션의 KeyFrame Left, Right를 가져와
// 2. 그 두 KeyFrame을 보간해
// 3. 보간한 KeyFrame과 내 다음 애니메이션 키프레임을 보간해.
// 4. 문제는 PlayAnimation은 계속 실행되는거야.
// 5. 그러므로 보간할 키프레임은 고정인데.
// 6. 다음 애니메이션 키프레임은 계속변경됨.


void CLoad_Model::Blend_Animation(_float fTimeDelta)
{
	// 현재 있는 디버그 출력 외에 추가
	//OutputDebugString((L"[BLEND] t: " + std::to_wstring(fTimeDelta) +
	//	L", PrevAnim: " + std::to_wstring(m_BlendDesc.iPrevAnimIndex) +
	//	L", NextAnim: " + std::to_wstring(m_iCurrentAnimIndex) + L"\n").c_str());

	m_BlendDesc.fElapsed += fTimeDelta;

	// clamp용 시간. => 보간 시간.
	_float t = m_BlendDesc.fElapsed / m_BlendDesc.fBlendDuration;
	t = min(t, 1.f); // clamp

	// 디버그 출력
	//if (static_cast<_uint>(m_BlendDesc.fElapsed * 10) % 5 == 0) // 0.5초마다 출력
	//{
	//	OutputDebugString((L"[BLEND] Progress: " + std::to_wstring(t * 100.f) + L"%\n").c_str());
	//}

	// 애니메이션 유효성 검사
	if (m_BlendDesc.iPrevAnimIndex >= m_Animations.size() ||
		m_iCurrentAnimIndex >= m_Animations.size())
	{
		OutputDebugString(L"[ERROR] Invalid animation indices in blending\n");
		m_BlendDesc.isBlending = false;
		return;
	}

	// 1. 현재 실행되었던 애니메이션.
	const auto& prevAnim = m_Animations[m_BlendDesc.iPrevAnimIndex];
	const auto& nextAnim = m_Animations[m_iCurrentAnimIndex];

	// 2. 다음 애니메이션의 TrackPosition을 변경한다.
	nextAnim->Update_TrackPosition(fTimeDelta);

	// 3. 실제 블랜딩 진행.
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		// 4. 이 시간대에 영향받는 본을 가져온다.
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
		OutputDebugString(L"[BLEND] Blending completed!\n");
	}

	// 모든 본들의 CombinedTransformationMatrix 업데이트
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	
}


// 1. Animation Blending을 위한 정보를 교체합니다.
void CLoad_Model::Change_Animation_WithBlend(uint32_t iNextAnimIndex, _float fBlendTime)
{
	// 유효성 검사
	if (iNextAnimIndex >= m_Animations.size())
	{
		OutputDebugString(L"[ERROR] Invalid next animation index in blend\n");
		return;
	}

	m_BlendDesc.isBlending = true;
	m_BlendDesc.fElapsed = 0.f;
	m_BlendDesc.fBlendDuration = fBlendTime;

	// 1. 현재 실행되고 있는 애니메이션 인덱스 담기.
	m_BlendDesc.iPrevAnimIndex = m_iCurrentAnimIndex;

	// 2. 현재 실행되고 있는 애니메이션의 시간 담기.
	if (m_iCurrentAnimIndex < m_Animations.size())
		m_BlendDesc.fPrevAnimTime = m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
	else
		m_BlendDesc.fPrevAnimTime = 0.f;

	// 3. 다음 실행될 애니메이션.
	m_BlendDesc.iNextAnimIndex = iNextAnimIndex;

	// 4. Animation Index를 변경.
	m_iCurrentAnimIndex = iNextAnimIndex;

	// 5. 다음 애니메이션을 처음부터 시작
	m_Animations[iNextAnimIndex]->Reset();

	// 6. 상태 플래그 초기화
	m_isTrackEnd = false;
	m_isFinished = false;
}


void CLoad_Model::Apply_RootMotion(_matrix matOld, _matrix matNew)
{
	// 루트 모션이 비활성화되어 있으면 무시
	if (!m_bEnableRootMotion)
		return;

	// 소유자가 없으면 무시
	if (!m_pOwner || !m_pOwner->Get_Transform())
		return;

	// Translate
	_vector vTranslate = matNew.r[3] - matOld.r[3];

	// PreTransformMatrix 적용 (모델 좌표계 → 월드 좌표계)
	vTranslate = XMVector3TransformCoord(vTranslate, XMLoadFloat4x4(&m_PreTransformMatrix));

	// Y축 이동 제거 (지상 이동만)
	vTranslate = XMVectorSetY(vTranslate, 0.f);

	// 루트 모션 스케일 적용
	vTranslate = XMVectorScale(vTranslate, m_fRootMotionScale);

	// 이동량 검증 및 적용
	_float fLength = XMVectorGetX(XMVector3Length(vTranslate));
	if (fLength > 0.001f && fLength < 10.0f) // 최소/최대 이동량 제한
	{
		m_pOwner->Get_Transform()->Move_Direction(vTranslate, 1.0f);

		// 디버그 출력
		/*OutputDebugString((L"[ROOT_MOTION] Translation: (" +
			std::to_wstring(XMVectorGetX(vTranslate)) + L", " +
			std::to_wstring(XMVectorGetZ(vTranslate)) + L"), Length: " +
			std::to_wstring(fLength) + L"\n").c_str());*/
	}

	// 회전 처리
	if (m_bEnableRootRotation)
	{
		_vector scale, qOld, qNew, translation;
		XMMatrixDecompose(&scale, &qOld, &translation, matOld);
		XMMatrixDecompose(&scale, &qNew, &translation, matNew);

		// 회전 차이 계산
		_vector qDelta = XMQuaternionMultiply(XMQuaternionInverse(qOld), qNew);

		// 회전량이 너무 크지 않을 때만 적용 (노이즈 방지)
		_float rotationAngle = 2.0f * acosf(fabsf(XMVectorGetW(qDelta)));
		if (rotationAngle < XM_PIDIV4) // 45도 이하
		{
			// Y축 회전만 추출 (Yaw)
			_float yaw = atan2f(2.0f * (XMVectorGetW(qDelta) * XMVectorGetY(qDelta) +
				XMVectorGetX(qDelta) * XMVectorGetZ(qDelta)),
				1.0f - 2.0f * (XMVectorGetY(qDelta) * XMVectorGetY(qDelta) +
					XMVectorGetZ(qDelta) * XMVectorGetZ(qDelta)));

			// 루트 모션 회전 적용 (강도 조절 가능)
			if (fabsf(yaw) > 0.001f)
			{
				m_pOwner->Get_Transform()->Add_Rotation(0.0f, yaw * 0.5f, 0.0f); // 50% 강도

				OutputDebugString((L"[ROOT_MOTION] Rotation: " +
					std::to_wstring(XMConvertToDegrees(yaw)) + L"°\n").c_str());
			}
		}
	}

	// 루트본 위치 복원 
	if (m_iRoot_BoneIndex < m_Bones.size())
	{
		_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
		rootMatrix.r[3] = matOld.r[3]; // 이전 위치로 복원
		m_Bones[m_iRoot_BoneIndex]->Set_TransformationMatrix(rootMatrix);
	}

}

void CLoad_Model::Change_Animation(_uint iAnimIndex, _float fBlendTime)
{
	if (fBlendTime > 0.f && m_iCurrentAnimIndex != iAnimIndex)
		Change_Animation_WithBlend(iAnimIndex, fBlendTime);
	else
		Change_Animation_Immediate(iAnimIndex); // 즉시 변경 함수도 필요

}

void CLoad_Model::Change_Animation_Immediate(_uint iAnimIndex)
{
	// 블렌딩 상태 초기화
	m_BlendDesc.isBlending = false;
	m_BlendDesc.fElapsed = 0.f;

	// 애니메이션 인덱스 변경
	m_iCurrentAnimIndex = iAnimIndex;

	// 애니메이션 리셋 (처음부터 시작)
	if (iAnimIndex < m_Animations.size())
	{
		m_Animations[iAnimIndex]->Reset();
	}

	// 상태 플래그 초기화
	m_isTrackEnd = false;
	m_isFinished = false;

	OutputDebugString((L"[INFO] Animation changed immediately to " + std::to_wstring(iAnimIndex) + L"\n").c_str());
}

_vector CLoad_Model::QuaternionSlerpShortest(_vector q1, _vector q2, _float t)
{
	// 내적 계산 (코사인 각도)
	_float dot = XMVectorGetX(XMQuaternionDot(q1, q2));

	// 내적이 음수면 더 긴 경로로 보간하게 되므로
	// q2를 -q2로 바꿔서 최단 경로 선택
	if (dot < 0.0f)
	{
		q2 = XMVectorNegate(q2);
		dot = -dot;
	}

	// 거의 같은 방향이면 선형 보간 (수치 안정성)
	if (dot > 0.9995f)
	{
		_vector result = XMVectorLerp(q1, q2, t);
		return XMQuaternionNormalize(result);
	}

	// 구면 선형 보간
	return XMQuaternionSlerp(q1, q2, t);
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

	// Bone 개수 확인
	if (m_Bones.empty())
	{
		OutputDebugString(L"[ERROR] No bones loaded! Cache build failed.\n");
		return E_FAIL;
	}

	// 각 애니메이션에 대해 Bone 채널 캐싱
	for (auto& pAnimation : m_Animations)
	{
		pAnimation->Build_BoneChannelCache(m_Bones.size());

		// 디버그: 캐시 크기 확인
		//OutputDebugString((L"[INFO] Animation cache built, bone count: " +
		//	std::to_wstring(m_Bones.size()) + L"\n").c_str());
	}

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
