#include "Load_MeshMaterial.h"

CLoad_MeshMaterial::CLoad_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoad_MeshMaterial::Initialize(std::ifstream& ifs, _wstring textureFolderPath)
{

	/*
	*  uint32_t materialPathVectorSize; 
	*  vector<_wstring> materialPathVector;
	*  두 값 읽어오기.
	*/
	MATERIAL_INFO mat = {};
	
	// 1. Material VectorSize 가져오기.
	if (!ReadBytes(ifs, &mat.materialPathVectorSize, sizeof(uint32_t)))
		CRASH("Material Vector Size Get Failed");

	mat.materialPathVector.resize(mat.materialPathVectorSize);

	// 2. 가져온 Material VectorSize로부터 Texture Path 가져오기.
	for (uint32_t t = 0; t < mat.materialPathVectorSize; ++t)
	{
		std::wstring texPath = ReadWString(ifs);
		if (!ifs)
			return E_FAIL;

		mat.materialPathVector[t] = texPath;
	}

	// 현재 materialPath Vector에는 Texture 종류별로 1장씩만 들어가게 되어있음.
	vector<_wstring>& materialVector = mat.materialPathVector;


	// 3. 가져온 TexturePath로 Material 만들기. => Texture ShaderResourceView 만들기.
	for (uint32_t i = 0; i < mat.materialPathVectorSize; ++i)
	{
		_wstring strTexturePath = mat.materialPathVector[i];

		// 비어있다면 비어있는채로 둡니다.
		if (strTexturePath.empty()) 
			continue;

		ID3D11ShaderResourceView* pSRV = { nullptr };

		_tchar			szExt[MAX_PATH] = {};
		_tchar			szDir[MAX_PATH] = {};
		_tchar			szFilePath[MAX_PATH] = {};
		
		_wsplitpath_s(strTexturePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFilePath, MAX_PATH, szExt, MAX_PATH);
		
		// wstring 결합.
		_tchar			szFinalPath[MAX_PATH] = {};
		wcscpy_s(szFinalPath, szDir);
		wcscat_s(szFinalPath, textureFolderPath.c_str());
		wcscat_s(szFinalPath, szFilePath);
		
		HRESULT		hr = {};

		if (L".tga" == szExt)
			hr = E_FAIL;

		_wstring wExt = L".dds";
		// dds로만 읽어오게 변경.
		wcscat_s(szFinalPath, wExt.c_str());

  		hr = CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);

		//if (L".dds" == szExt)
		//	hr = CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
		//else
		//	hr = CreateWICTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);

		if (FAILED(hr))
			CRASH("Texture Create Failed");

		m_SRVs[i].emplace_back(pSRV);
	}

	return S_OK;
}

HRESULT CLoad_MeshMaterial::Bind_Resources(CShader* pShader, const _char* pConstantName, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs[eTextureType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eTextureType][iTextureIndex]);
}

CLoad_MeshMaterial* CLoad_MeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::ifstream& ifs, _wstring textureFolderPath)
{
	CLoad_MeshMaterial* pInstance = new CLoad_MeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(ifs, textureFolderPath)))
	{
		MSG_BOX(TEXT("Create Failed : CLoad_MeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoad_MeshMaterial::Free()
{
	CBase::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
