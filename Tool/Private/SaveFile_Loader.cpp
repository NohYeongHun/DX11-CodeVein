#include "SaveFile_Loader.h"

CSaveFile_Loader::CSaveFile_Loader()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSaveFile_Loader::Initialize()
{
	return S_OK;
}

// Transform 정보도 추가해야됨 나중에.

void CSaveFile_Loader::Load_MapFile(string filePath, LEVEL eLevel)
{
	std::ifstream ifs(filePath, std::ios::binary);
	if (!ifs.is_open())
		return;

	struct LayerInfo {
		const wchar_t* layerName;
		const wchar_t* prototypeName;
		
	};

	// 각 레이어 정보와 타입
	const LayerInfo layers[] = {
		{L"Layer_Map_Parts", L"Prototype_GameObject_Map_Part"}
	};

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	

	for (int i = 0; i < 1; ++i)
	{
		size_t count = 0;
		ifs.read(reinterpret_cast<char*>(&count), sizeof(size_t));

		for (size_t j = 0; j < count; ++j)
		{
			CToolMap_Part::MAP_PART_DESC Desc{};
			Desc.eArgType = CToolMap_Part::ARG_TYPE::MODEL_LOAD;
			
			MAP_PART_INFO modelInfo{};
			if (!Read_MapInfo(ifs, modelInfo))
				return;

			Desc.pData = reinterpret_cast<void*>(&modelInfo);

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(
				ENUM_CLASS(eLevel),
				layers[i].layerName,
				ENUM_CLASS(eLevel),
				layers[i].prototypeName,
				reinterpret_cast<void*>(&Desc))))
			{
				MSG_BOX(TEXT("Failed to Add GameObject to Layer"));
				return;
			}
		}
	}



}

/* 모델을 로드 해서 프로토타입에 추가합니다. */
void CSaveFile_Loader::Save_ModelFile(string filePath, const _wstring& strPrototypeTag)
{
	// 1. 파일 경로 열기.
	std::ofstream ofs(filePath, std::ios::binary);

	if (!ofs.is_open())
		return;

	struct PrototypeInfo {
		const wchar_t* PrototypeName;
		const std::type_info& typeInfo;
	};

	
	PrototypeInfo ePrototypeInfo = {
		strPrototypeTag.c_str(), typeid(CTool_Model)};

	// 2. 현재 프로토타입 모델 가져오기.
	CTool_Model* pModel = dynamic_cast<CTool_Model*>(m_pGameInstance->Get_Prototype(PROTOTYPE::COMPONENT
		, ENUM_CLASS(m_eCurLevel), strPrototypeTag));

	if (nullptr == pModel)
	{
		MSG_BOX(TEXT("Failed Get Prototype Save Model File"));
		return;
	}

	Save_AnimModel(ofs, pModel, strPrototypeTag);
	
	

	ofs.close();
}

void CSaveFile_Loader::Save_AnimModel(std::ofstream& ofs, CTool_Model* pModel, const _wstring& strModelTag)
{
	if (nullptr == pModel)
		CRASH("Model NULLPTR");

	ANIMMODEL_INFO animModelInfo = {};
	// Model Tag는 선택된 Prototype Tag로 저장됩니다.
	animModelInfo.strModelTag = strModelTag; 

	_matrix		PreTransformMatrix = XMMatrixIdentity();
	
	// 1. ANIMMODEL_INFO를 채웁니다.
	pModel->Save_AnimModel(animModelInfo, PreTransformMatrix);

	// 2. 채워진 값을 이용해서 .dat 파일을 씁니다.
	/* ----------- WString ModelTag ------------ */
	WriteWString(ofs, animModelInfo.strModelTag);   // ★ 모델 태그 먼저 기록

	/* ---------- Mesh ---------- */
	// 1. meshVector size
	ofs.write(reinterpret_cast<const char*>(&animModelInfo.meshVectorSize), sizeof(uint32_t));


	// - vector 순회.
	for (const ANIMMESH_INFO& mesh : animModelInfo.meshVector)
	{
		ofs.write(reinterpret_cast<const char*>(&mesh.iMarterialIndex), sizeof(uint32_t));
		ofs.write(reinterpret_cast<const char*>(&mesh.iVertexCount), sizeof(uint32_t));
		ofs.write(reinterpret_cast<const char*>(&mesh.iIndicesCount), sizeof(uint32_t));
		ofs.write(reinterpret_cast<const char*>(&mesh.iBoneCount), sizeof(uint32_t));
		
		WriteString(ofs, mesh.strName);

		// vertex 저장
		ofs.write(reinterpret_cast<const char*>(mesh.vertices.data()),
			mesh.iVertexCount * sizeof(VTXANIMMESH));

		// index 저장
		ofs.write(reinterpret_cast<const char*>(mesh.indices.data()),
			mesh.iIndicesCount * sizeof(uint32_t));

		// bone 저장
		ofs.write(reinterpret_cast<const char*>(&mesh.BoneIndexVectorSize), sizeof(uint32_t));
		ofs.write(reinterpret_cast<const char*>(mesh.Boneindices.data()),
			mesh.BoneIndexVectorSize * sizeof(_int));

		// Offset Matrix
		ofs.write(reinterpret_cast<const char*>(&mesh.OffSetVectorSize), sizeof(uint32_t));
		ofs.write(reinterpret_cast<const char*>(mesh.OffsetMatrices.data()),
			mesh.OffSetVectorSize * sizeof(_float4x4));
	}

	/* ---------- Material ---------- */
	ofs.write(reinterpret_cast<const char*>(&animModelInfo.materialVectorSize), sizeof(uint32_t));
	for (const MATERIAL_INFO& material : animModelInfo.materialVector)
	{
		ofs.write(reinterpret_cast<const char*>(&material.materialPathVectorSize), sizeof(uint32_t));

		for (const std::wstring& texPath : material.materialPathVector)
			WriteWString(ofs, texPath);   // 길이+바이트 순으로
	}

	/* ---------- Bone ---------- */
	ofs.write(reinterpret_cast<const char*>(&animModelInfo.boneVectorSize), sizeof(uint32_t));
	for (const BONE_INFO& bone : animModelInfo.boneVector)
	{
		ofs.write(reinterpret_cast<const char*>(&bone.iParentBoneIndex), sizeof(_int));
		WriteString(ofs, bone.strName);
		ofs.write(reinterpret_cast<const char*>(&bone.TransformMatrix), sizeof(_float4x4));
	}

	/* ---------- Animation 정보 저장. ---------- */
	ofs.write(reinterpret_cast<const char*>(&animModelInfo.iCurrentAnimIndex), sizeof(uint32_t));
	ofs.write(reinterpret_cast<const char*>(&animModelInfo.iNumAnimations), sizeof(uint32_t));

	for (const ANIMATION_INFO& anim : animModelInfo.animationVector)
	{
		// 애니메이션 이름 저장.
		WriteString(ofs, anim.strAnimName);
		ofs.write(reinterpret_cast<const char*>(&anim.fDuration), sizeof(_float));
		ofs.write(reinterpret_cast<const char*>(&anim.fTickPerSecond), sizeof(_float));
		ofs.write(reinterpret_cast<const char*>(&anim.fCurrentTrackPostion), sizeof(_float));
		ofs.write(reinterpret_cast<const char*>(&anim.iNumChannels), sizeof(uint32_t));

		
		/* Channel 정보 저장. */
		for (const CHANNEL_INFO& channel : anim.Channels)
		{
			// string 저장.
			WriteString(ofs, channel.channelName);
			ofs.write(reinterpret_cast<const char*>(&channel.iBoneIndex), sizeof(uint32_t));
			ofs.write(reinterpret_cast<const char*>(&channel.iNumKeyFrames), sizeof(uint32_t));
			ofs.write(reinterpret_cast<const char*>(channel.KeyFrames.data()),
				channel.iNumKeyFrames * sizeof(KEYFRAME));
		}

		/* Current KeyFrame 크기 저장. */
		ofs.write(reinterpret_cast<const char*>(&anim.iNumKeyFrameIndices), sizeof(uint32_t));
		/* Current KeyFrameIndex 정보 저장.*/
		ofs.write(reinterpret_cast<const char*>(anim.CurrentKeyFrameIndices.data()),
			anim.iNumKeyFrameIndices * sizeof(uint32_t));
	}

}

// 기존거 Prototype Load 되는지부터 확인
//void CSaveFile_Loader::Save_NonAnimModel(std::ofstream& ofs, CTool_Model* pModel, const _wstring& strModelTag)
//{
//	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
//	MODEL_INFO nonAnimModelInfo = {};
//
//	nonAnimModelInfo = pModel->Save_NonAminModel(PreTransformMatrix, strModelTag);
//}

void CSaveFile_Loader::Load_ModelFile(string filePath, LEVEL eLevel)
{
	std::ifstream ifs(filePath, std::ios::binary);
	if (!ifs.is_open())
		return;


	struct PrototypeInfo {
		const wchar_t* PrototypeName;
		const std::type_info& typeInfo;
	};

	const PrototypeInfo Prototypes[] = {
		{L"Prototype_Component_Model_PlayerLoad", typeid(CTool_Model)}
	};

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	
	// Model Component를 현재 레벨에 추가.

	//PrototypeInfo ePrototypeInfo = { strPrototypeTag.c_str(), typeid(CTool_Model) };

	ifs.close();
}

void CSaveFile_Loader::Load_AnimModel(std::ifstream& ifs, CTool_Model* pModel, const _wstring& strModelTag)
{

}

void CSaveFile_Loader::Save_NavigationFile(string filePath, const vector<class CCell*>& Cells)
{

	// 1. 파일 경로 열기.
	std::ofstream ofs(filePath, std::ios::binary);


	NAVIGATIONSAVE_DESC naviDesc = {};

	// 2. 구조체에 정보 채우기.
	Save_NaviDesc(naviDesc, Cells);

	/* 3. 채워준 정보를 바탕으로 Save하기. */
	ofs.write(reinterpret_cast<const char*>(&naviDesc.iCellCount), sizeof(uint32_t));
	ofs.write(reinterpret_cast<const char*>(naviDesc.Cells.data()), naviDesc.iCellCount * sizeof(CELLSAVE_DESC));


	ofs.close();
}

void CSaveFile_Loader::Save_NaviDesc(NAVIGATIONSAVE_DESC& naviDesc, const vector<class CCell*>& Cells)
{
	naviDesc.iCellCount = Cells.size(); // Vector 크기.

	for (auto& pCell : Cells)
	{
		CELLSAVE_DESC Desc{};
		Desc.vPointA = pCell->Get_PointPos(CELLPOINT::A);
		Desc.vPointB = pCell->Get_PointPos(CELLPOINT::B);
		Desc.vPointC = pCell->Get_PointPos(CELLPOINT::C);
		naviDesc.Cells.emplace_back(Desc);
	}
}

NAVIGATIONSAVE_DESC CSaveFile_Loader::Load_NavigationFile(string filePath)
{
	std::ifstream ifs(filePath, std::ios::binary);
	if (!ifs.is_open())
		CRASH("Load Navigation File Failed");

	NAVIGATIONSAVE_DESC NaviDesc = {};


	// 1. Cell 개수 세오기.
	ifs.read(reinterpret_cast<char*>(&NaviDesc.iCellCount), sizeof(uint32_t));

	// 2. Vector에 값 채우기.
	// 벡터 초기화
	NaviDesc.Cells.resize(NaviDesc.iCellCount); 
	ifs.read(reinterpret_cast<char*>(NaviDesc.Cells.data()), NaviDesc.iCellCount * sizeof(CELLSAVE_DESC));


	ifs.close();
		
	return NaviDesc;
}



bool CSaveFile_Loader::Read_MapInfo(std::ifstream& ifs, MAP_PART_INFO& outModel)
{
	
	/* ---------- ModelTag ---------- */
	_wstring strModelTag = ReadWString(ifs);
	if (strModelTag.empty())
		return false;
	
	outModel.strModelTag = strModelTag;

	/* ---------- Mesh ---------- */

	// 1. meshVectorsize 읽어오기.
	uint32_t meshVectorSize = {};
	ifs.read(reinterpret_cast<char*>(&meshVectorSize), sizeof(uint32_t));

	outModel.meshVectorSize = meshVectorSize;
	// 2. vector 순회해서 채워넣기.
	outModel.meshVector.resize(meshVectorSize); // 크기 지정.

	for (_uint m = 0; m < outModel.meshVectorSize; ++m)
	{
		MESH_INFO& meshInfo = outModel.meshVector[m];

		// 3. index, vertexcount, indices 초기화
		if (!ReadBytes(ifs, &meshInfo.iMarterialIndex, sizeof(uint32_t))) return false;
		if (!ReadBytes(ifs, &meshInfo.iVertexCount, sizeof(uint32_t))) return false;
		if (!ReadBytes(ifs, &meshInfo.iIndicesCount, sizeof(uint32_t))) return false;

		// 4. mesh Vertex 벡터 초기화.
		meshInfo.vertices.resize(meshInfo.iVertexCount);

		// 5. vertex 정보들 벡터에 담기.
		if (meshInfo.iVertexCount > 0)
		{
			if (!ReadBytes(ifs, meshInfo.vertices.data(), sizeof(VTXMESH) * meshInfo.iVertexCount))
				return false;
		}
			

		// 6. Indicies 정보들 벡터에 담기
		meshInfo.indices.resize(meshInfo.iIndicesCount);
		if (meshInfo.iIndicesCount > 0)
		{
			if (!ReadBytes(ifs, meshInfo.indices.data(), sizeof(uint32_t) * meshInfo.iIndicesCount))
				return false;
		}
		
	}

	/* ---------- Material ---------- */
	if (!ReadBytes(ifs, &outModel.materialVectorSize, sizeof(uint32_t)))
		return false;

	outModel.materialVector.resize(outModel.materialVectorSize);

	for (uint32_t k = 0; k < outModel.materialVectorSize; ++k)
	{
		MATERIAL_INFO& mat = outModel.materialVector[k];
		if (!ReadBytes(ifs, &mat.materialPathVectorSize, sizeof(uint32_t)))
			return false;

		mat.materialPathVector.clear();
		mat.materialPathVector.resize(mat.materialPathVectorSize);

		for (uint32_t t = 0; t < mat.materialPathVectorSize; ++t)
		{
			std::wstring texPath = ReadWString(ifs);
			if (!ifs)
				return false;

			mat.materialPathVector[t] = texPath;
		}
	}

	/* Transform 저장하기. */
	if (!ReadBytes(ifs, &outModel.transformInfo, sizeof(TRANSFORM_INFO)))
		return false;

	return true;

}

CSaveFile_Loader* CSaveFile_Loader::Create()
{
	CSaveFile_Loader* pInstance = new CSaveFile_Loader();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CSaveFile_Loader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSaveFile_Loader::Free()
{
	CBase::Free();
	Safe_Release(m_pGameInstance);
}
