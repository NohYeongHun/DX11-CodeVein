#include "Load_MeshMaterial.h"

CLoad_MeshMaterial::CLoad_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoad_MeshMaterial::Initialize(std::ifstream& ifs)
{

	/*
	*  uint32_t materialPathVectorSize; 
	*  vector<_wstring> materialPathVector;
	*  두 값 읽어오기.
	*/
	MATERIAL_INFO mat = {};
	
	// 1. Material VectorSize 가져오기.
	if (!ReadBytes(ifs, &mat.materialPathVectorSize, sizeof(uint32_t)))
		return E_FAIL;

	// 2. 가져온 Material VectorSize로부터 Texture Path 가져오기.
	for (uint32_t t = 0; t < mat.materialPathVectorSize; ++t)
	{
		std::wstring texPath = ReadWString(ifs);
		if (!ifs)
			return E_FAIL;

		mat.materialPathVector[t] = texPath;
	}

	vector<_wstring>& materialVector = mat.materialPathVector;

	// 3. 가져온 TexturePath로 Material 만들기. => Texture ShaderResourceView 만들기.
	for (auto& vec : materialVector)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };

		_tchar			szExt[MAX_PATH] = {};
		
		
		_wsplitpath_s(vec.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT		hr = {};

		if (L".tga" == szExt)
			hr = E_FAIL;

		if (L".dds" == szExt)
			hr = CreateDDSTextureFromFile(m_pDevice, vec.c_str(), nullptr, &pSRV);
		else 
			hr = CreateWICTextureFromFile(m_pDevice, vec.c_str(), nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs->emplace_back(pSRV);
	}
	
	

	return S_OK;
}

HRESULT CLoad_MeshMaterial::Bind_Resources(CShader* pShader, const _char* pConstantName, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs[eTextureType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eTextureType][iTextureIndex]);
}

CLoad_MeshMaterial* CLoad_MeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::ifstream& ifs)
{
	CLoad_MeshMaterial* pInstance = new CLoad_MeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(ifs)))
	{
		MSG_BOX(TEXT("Create Failed : CLoad_MeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoad_MeshMaterial::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
