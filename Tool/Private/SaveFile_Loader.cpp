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
void CSaveFile_Loader::Save_File(string filePath)
{
	// 1. 파일 경로 열기.
	std::ofstream ofs(filePath, std::ios::binary);

	if (!ofs.is_open())
		return;

	struct LayerInfo {
		const wchar_t* layerName;
		const std::type_info& typeInfo;
	};

	// 각 레이어 정보와 타입
	const LayerInfo layers[] = {
		{L"Layer_Map_Parts", typeid(CToolMap_Part)}
	};

	for (_uint i = 0; i < 1; i++)
	{
		// 2. Object 가져오기.
		CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), layers[i].layerName);
		if (nullptr == pLayer)
		{
			MSG_BOX(TEXT("Failed to Get Layer"));
			return;
		}

		// 3. 객체 리스트 가져오기.
		const list<CGameObject*>& objList = pLayer->Get_GameObjects();
		size_t count = objList.size();
		ofs.write(reinterpret_cast<const char*>(&count), sizeof(size_t));

		// 4. 리스트 순회
		for (const auto& pGameObject : objList)
		{
			CToolMap_Part* pMapPart = dynamic_cast<CToolMap_Part*>(pGameObject);
			_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
			MODEL_INFO Model_Info = pMapPart->Save_ModelInfo(PreTransformMatrix);

			/* ----------- WString ModelTag ------------ */
			WriteWString(ofs, Model_Info.strModelTag);   // ★ 모델 태그 먼저 기록

			/* ---------- Mesh ---------- */
			// 1. meshVector size
			ofs.write((char*)&Model_Info.meshVectorSize, sizeof(uint32_t));

			// - vector 순회.
			for (const MESH_INFO& mesh : Model_Info.meshVector)
			{
				ofs.write((char*)&mesh.iMarterialIndex, sizeof(uint32_t));
				ofs.write((char*)&mesh.iVertexCount, sizeof(uint32_t));
				ofs.write((char*)&mesh.iIndicesCount, sizeof(uint32_t));

				ofs.write((char*)mesh.vertices.data(),
					mesh.iVertexCount * sizeof(VTXMESH));
				ofs.write((char*)mesh.indices.data(),
					mesh.iIndicesCount * sizeof(uint32_t));
			}
			
			/* ---------- Material ---------- */
			ofs.write((char*)&Model_Info.materialVectorSize, sizeof(uint32_t));

			for (const MATERIAL_INFO& mat : Model_Info.materialVector)
			{
				ofs.write((char*)&mat.materialVectorSize, sizeof(uint32_t));

				for (const std::wstring& texPath : mat.materialPathVector)
					WriteWString(ofs, texPath);   // 길이+바이트 순으로
			}
		}
	}

	ofs.close();
}

void CSaveFile_Loader::Load_File(string filePath, LEVEL eLevel)
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
			
			MODEL_INFO modelInfo{};
			if (!Read_ModelInfo(ifs, modelInfo))
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

bool CSaveFile_Loader::Read_ModelInfo(std::ifstream& ifs, MODEL_INFO& outModel)
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
		if (!ReadBytes(ifs, &mat.materialVectorSize, sizeof(uint32_t)))
			return false;

		mat.materialPathVector.clear();
		mat.materialPathVector.reserve(mat.materialVectorSize);

		for (uint32_t t = 0; t < mat.materialVectorSize; ++t)
		{
			std::wstring texPath = ReadWString(ifs);
			if (!ifs)
				return false;

			mat.materialPathVector.push_back(texPath);
		}
	}

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
	__super::Free();
	Safe_Release(m_pGameInstance);
}
