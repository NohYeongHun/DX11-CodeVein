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
	, m_vOldPos { Prototype.m_vOldPos }
	, m_BoundingBox { Prototype.m_BoundingBox }
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

	Calculate_Bounding_Box();
	XMStoreFloat4(&m_vOldPos, m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix().r[3]);



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


void CLoad_Model::Set_Animation(_uint iAnimIndex, _bool isLoop)
{
	if (iAnimIndex >= m_Animations.size())
		return;

	// 루트 모션 연속성 처리 - 애니메이션 인덱스 변경 전에 해야 함!
	_vector vCurrentRootPos = XMVectorZero();
	XMVectorSetW(vCurrentRootPos, 1.f);
	if (m_bRootMotionTranslate && m_iCurrentAnimIndex != iAnimIndex && m_iCurrentAnimIndex < m_Animations.size())
	{
		// 현재 루트본 위치 저장
		_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_CombinedTransformationMatrix();
		vCurrentRootPos = rootMatrix.r[3];
	}

	// 애니메이션 설정 (한 번만!)
	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;

	// 새 애니메이션 리셋
	m_Animations[m_iCurrentAnimIndex]->Reset();
	m_isFinished = false;
	m_isTrackEnd = false;

	

	XMStoreFloat4(&m_vOldPos, vCurrentRootPos);
}


_float4x4* CLoad_Model::Get_BoneMatrix(const _char* pBoneName)
{
	auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CLoad_Bone* pBone) {
		if (true == pBone->Compare_Name(pBoneName))
			return true;
		return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

_uint CLoad_Model::Get_CurrentFrame()
{
	return _uint();
}

_float CLoad_Model::Get_Current_Ratio()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition() / m_Animations[m_iCurrentAnimIndex]->Get_Duration();
}

#pragma region 바운딩박스 구하기.
void CLoad_Model::Calculate_Bounding_Box()
{
	// 모든 메시의 정점들을 순회
	for (auto& pMesh : m_Meshes)
	{
		if (!pMesh)
			continue;

		// 각 메시의 바운딩 정보 가져오기 (Load_Mesh에 함수 추가 필요)
		BOUNDING_BOX meshBounding = pMesh->Get_MeshBoundingBox();

		// 전체 모델의 바운딩 박스 업데이트
		m_BoundingBox.vMin.x = min(m_BoundingBox.vMin.x, meshBounding.vMin.x);
		m_BoundingBox.vMin.y = min(m_BoundingBox.vMin.y, meshBounding.vMin.y);
		m_BoundingBox.vMin.z = min(m_BoundingBox.vMin.z, meshBounding.vMin.z);

		m_BoundingBox.vMax.x = max(m_BoundingBox.vMax.x, meshBounding.vMax.x);
		m_BoundingBox.vMax.y = max(m_BoundingBox.vMax.y, meshBounding.vMax.y);
		m_BoundingBox.vMax.z = max(m_BoundingBox.vMax.z, meshBounding.vMax.z);
	}

	// 중심점과 크기 계산
	m_BoundingBox.vCenter.x = (m_BoundingBox.vMin.x + m_BoundingBox.vMax.x) * 0.5f;
	m_BoundingBox.vCenter.y = (m_BoundingBox.vMin.y + m_BoundingBox.vMax.y) * 0.5f;
	m_BoundingBox.vCenter.z = (m_BoundingBox.vMin.z + m_BoundingBox.vMax.z) * 0.5f;

	m_BoundingBox.vExtents.x = (m_BoundingBox.vMax.x - m_BoundingBox.vMin.x) * 0.5f;
	m_BoundingBox.vExtents.y = (m_BoundingBox.vMax.y - m_BoundingBox.vMin.y) * 0.5f;
	m_BoundingBox.vExtents.z = (m_BoundingBox.vMax.z - m_BoundingBox.vMin.z) * 0.5f;

	// 모델 전체 높이
	m_BoundingBox.fHeight = m_BoundingBox.vMax.y - m_BoundingBox.vMin.y;

	Rotate_Bounding_Box();
}

void CLoad_Model::Rotate_Bounding_Box()
{
	_matrix preTransform = XMLoadFloat4x4(&m_PreTransformMatrix);

	//_vector vScale, vRotate, vTranslate;
	//XMMatrixDecompose(&vScale, &vRotate, &vTranslate, preTransform);


	_matrix rotationMatrix = XMMatrixRotationX(XM_PI * 0.5f);

	// 로컬 바운딩박스의 8개 꼭짓점 계산
	_float3 corners[8] = {
		// 하단 4개 점
		_float3(m_BoundingBox.vMin.x, m_BoundingBox.vMin.y, m_BoundingBox.vMin.z),
		_float3(m_BoundingBox.vMax.x, m_BoundingBox.vMin.y, m_BoundingBox.vMin.z),
		_float3(m_BoundingBox.vMax.x, m_BoundingBox.vMin.y, m_BoundingBox.vMax.z),
		_float3(m_BoundingBox.vMin.x, m_BoundingBox.vMin.y, m_BoundingBox.vMax.z),
		// 상단 4개 점
		_float3(m_BoundingBox.vMin.x, m_BoundingBox.vMax.y, m_BoundingBox.vMin.z),
		_float3(m_BoundingBox.vMax.x, m_BoundingBox.vMax.y, m_BoundingBox.vMin.z),
		_float3(m_BoundingBox.vMax.x, m_BoundingBox.vMax.y, m_BoundingBox.vMax.z),
		_float3(m_BoundingBox.vMin.x, m_BoundingBox.vMax.y, m_BoundingBox.vMax.z)
	};
	// 회전된 바운딩박스 계산을 위한 새로운 Min/Max
	_float3 rotatedMin = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
	_float3 rotatedMax = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// 8개 꼭짓점에 회전만 적용
	for (int i = 0; i < 8; ++i)
	{
		_vector localCorner = XMLoadFloat3(&corners[i]);

		// 회전만 적용 (XMVector3TransformNormal 사용 - 이동 적용 안됨)
		_vector rotatedCorner = XMVector3TransformNormal(localCorner, rotationMatrix);

		_float3 rotatedCornerFloat3;
		XMStoreFloat3(&rotatedCornerFloat3, rotatedCorner);

		// 회전된 바운딩박스 업데이트
		rotatedMin.x = min(rotatedMin.x, rotatedCornerFloat3.x);
		rotatedMin.y = min(rotatedMin.y, rotatedCornerFloat3.y);
		rotatedMin.z = min(rotatedMin.z, rotatedCornerFloat3.z);

		rotatedMax.x = max(rotatedMax.x, rotatedCornerFloat3.x);
		rotatedMax.y = max(rotatedMax.y, rotatedCornerFloat3.y);
		rotatedMax.z = max(rotatedMax.z, rotatedCornerFloat3.z);
	}

	m_BoundingBox.fHeight = rotatedMax.y - rotatedMin.y;

	// 회전된 바운딩박스 정보 업데이트
	m_BoundingBox.vMin = rotatedMin;
	m_BoundingBox.vMax = rotatedMax;

	// 중심점과 크기 다시 계산
	m_BoundingBox.vCenter.x = (rotatedMin.x + rotatedMax.x) * 0.5f;
	m_BoundingBox.vCenter.y = (rotatedMin.y + rotatedMax.y) * 0.5f - m_BoundingBox.fHeight * 0.5f;
	m_BoundingBox.vCenter.z = (rotatedMin.z + rotatedMax.z) * 0.5f;

	m_BoundingBox.vExtents.x = (rotatedMax.x - rotatedMin.x) * 0.5f;
	m_BoundingBox.vExtents.y = (rotatedMax.y - rotatedMin.y) * 0.5f;
	m_BoundingBox.vExtents.z = (rotatedMax.z - rotatedMin.z) * 0.5f;

 	

#ifdef _DEBUG
	char debugMsg[256];
	sprintf_s(debugMsg, "Rotated Bounding Box:\nMin: (%.2f, %.2f, %.2f)\nMax: (%.2f, %.2f, %.2f)\nHeight: %.2f\n",
		rotatedMin.x, rotatedMin.y, rotatedMin.z,
		rotatedMax.x, rotatedMax.y, rotatedMax.z,
		m_BoundingBox.fHeight);
	OutputDebugStringA(debugMsg);
#endif


}
#pragma endregion



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


// 내가 한실수는 애니메이션 굴리기전에 Root뼈를 못움직이게 해야함.
// 애니메이션 굴린다음에 루트뼈를 못움직이게함.
// 다른 뼈들은 이미 움직여버림.
// 굴리기전에 이동량 죽이고 
// => 트랜스폼에 적용하는건 움직인 정보니까.
_bool CLoad_Model::Play_Animation(_float fTimeDelta)
{
	/* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
	m_isFinished = false;
	m_isTrackEnd = false;

	// 2. 애니메이션을 실행합니다. => 실행하기전에 루트 움직이지마라.
	// 여기서 블렌딩 진행.
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(
		m_Bones, m_isLoop, &m_isFinished, m_BlendDesc, fTimeDelta
	);

	//_matrix mat = m_Bones[m_iRoot_BoneIndex]->Get_TransformationMatrix();
	//mat.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//m_Bones[m_iRoot_BoneIndex]->Set_TransformationMatrix(mat);

	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

		if (i == m_iRoot_BoneIndex)
			Handle_RootMotion(fTimeDelta);
	}
		
	if (m_isFinished)
		XMStoreFloat4(&m_vOldPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return m_isFinished;
}

// 1. 저번에 실행되던 애니메이션의 KeyFrame Lefft, Right를 가져와
// 2. 그 두 KeyFrame을 보간해
// 3. 보간한 KeyFrame과 내 다음 애니메이션 키프레임을 보간해.
// 4. 문제는 PlayAnimation은 계속 실행되는거야.
// 5. 그러므로 보간할 키프레임은 고정인데.
// 6. 다음 애니메이션 키프레임은 계속변경됨.


// 블렌딩 상태를 저장합니다.
void CLoad_Model::Set_BlendInfo(uint32_t iNextAnimIndex, _float fBlendTime, _bool bScale, _bool bRotation, _bool bTranslation)
{
	// 유효성 검사
	if (iNextAnimIndex >= m_Animations.size())
	{
		OutputDebugString(L"[ERROR] Invalid next animation index in blend\n");
		return;
	}

	// 블렌딩 플래그. 
	m_BlendDesc.isBlending = true;
	m_BlendDesc.fElapsed = 0.f;
	m_BlendDesc.fBlendDuration = fBlendTime;
	m_BlendDesc.bRotate = bRotation;
	m_BlendDesc.bScale = bScale;
	m_BlendDesc.bTranslate = bTranslation;

	// 1. 현재 실행되고 있는 애니메이션 인덱스 담기
	m_BlendDesc.iPrevAnimIndex = m_iCurrentAnimIndex;
	m_BlendDesc.fPrevAnimTime = m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();

	// 3. 다음 실행될 애니메이션
	// 4. Animation Index를 변경
	// m_iCurrentAnimIndex = iNextAnimIndex;

	m_Animations[iNextAnimIndex]->Reset();

	// 5. 사용할 Animation Pointer 전달.
	m_BlendDesc.pLoad_Animation = m_Animations[m_iCurrentAnimIndex];

	// 6. 상태 플래그 초기화
	m_isTrackEnd = false;
	m_isFinished = false;

}

void CLoad_Model::Handle_RootMotion(_float fTimeDelta)
{
	_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_CombinedTransformationMatrix();
	_vector vNewRootPos = rootMatrix.r[3];

	if (!m_isFinished)
	{

		// 0. 뼈의 이동 구하기.
		_vector vLocalTranslate = vNewRootPos - XMLoadFloat4(&m_vOldPos);
		//vLocalTranslate = XMVectorSetY(vLocalTranslate, 0.f); // Y축 제거


		_vector vWorldTranslate = vLocalTranslate; // 기본값

		// 1. 플레이어 RotMatrix 추출 => 만약 RootMotionRotate 설정을 할것이라면?
		if (m_bRootMotionRotate)
		{
			_matrix playerWorldMatrix = m_pOwner->Get_Transform()->Get_WorldMatrix();
			_vector playerScale, playerRot, playerTrans;
			XMMatrixDecompose(&playerScale, &playerRot, &playerTrans, playerWorldMatrix);
			_matrix playerRotMatrix = XMMatrixRotationQuaternion(playerRot);
			vWorldTranslate = XMVector3TransformNormal(vLocalTranslate, playerRotMatrix);
		}

		// 2. 이동값을 월드에 적용할 것인지 애니메이션 에서 설정
		if (m_bRootMotionTranslate)
		{
			m_pOwner->RootMotion_Translate(vWorldTranslate);
		}

		//_vector vNewRootPosNoY = XMVectorSetY(vNewRootPos, 0.f);
		//XMStoreFloat4(&m_vOldPos, vNewRootPosNoY);

		XMStoreFloat4(&m_vOldPos, vNewRootPos);
	}

	

	// 새로운 애니메이션의 루트본을 이전벡터에 넣어둔다.
	rootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	m_Bones[m_iRoot_BoneIndex]->Set_CombinedTransformationMatrix(rootMatrix);
}

void CLoad_Model::Reset_RootMotion()
{
	_matrix rootMatrix = m_Bones[m_iRoot_BoneIndex]->Get_CombinedTransformationMatrix();
	XMStoreFloat4(&m_vOldPos, rootMatrix.r[3]);
}




void CLoad_Model::Animation_Reset()
{
	m_Animations[m_iCurrentAnimIndex]->Reset();
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

		/*if (pLoadBone->Compare_Name("Hips")) 
			m_iRoot_BoneIndex = i;*/
		if (pLoadBone->Compare_Name("CHARA_OFFSET")) 
			m_iRoot_BoneIndex = i;
		


		if (nullptr == pLoadBone)
			CRASH("LOAD BONE FAILED");

		m_Bones.emplace_back(pLoadBone);
	}

	m_Bones[m_iRoot_BoneIndex]->Set_Root();
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

}
