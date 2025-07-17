#include "MeshMaterial.h"

CTool_MeshMaterial::CTool_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTool_MeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiscene)
{
	
	// 폴더 경로 지정. szDrive, szDir
	_char szDir[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};

	/* 1. Dir 가져오기. */
	/* C:\Users\dkznd\Desktop\DX11\DX11Code\Mine\MyFramework\Framework\Client\Bin\Resources\Models\Player.fbx */
	/* Drvie : C:\ */
	/* Dir : Users\dkznd\Desktop\DX11\DX11Code\Mine\MyFramework\Framework\Client\Bin\Resources\Models\ */
	/* Filename : Player */
	/* Ext .fbx */
	_splitpath_s(pModelFilePath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, szExt, MAX_PATH);

	string strDirPath = string(szDir);
	string strExt = string(szExt);

	if (strExt == ".fbx")
		Initialize_FBX(pModelFilePath, pAIMaterial, strDirPath);
	else if (strExt == ".glb")
		Initialize_GLB(pModelFilePath, pAIMaterial, pAiscene, strDirPath);
	

	return S_OK;
}

HRESULT CTool_MeshMaterial::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs[eTextureType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eTextureType][iTextureIndex]);
}

HRESULT CTool_MeshMaterial::Initialize_FBX(const _char* pModelFilePath, const aiMaterial* pAIMaterial, string strDirPath)
{
	static _uint iNum = 0;

	for (_uint i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
	{

		aiTextureType textureType = static_cast<aiTextureType>(i);
		_uint iNumtextures = pAIMaterial->GetTextureCount(textureType);

		for (_uint j = 0; j < iNumtextures; j++)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };

			aiString strTexturePath;

			// TextureType DIFFUSE, AMBIENT 등등..
			if (FAILED(pAIMaterial->GetTexture(textureType, j, &strTexturePath)))
			{
				break;
			}


			_char szFileName[MAX_PATH] = {};
			_char szExt[MAX_PATH] = {};
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			string fileName = { szFileName };
			string fileExt = { szExt };
			string filePath = strDirPath + fileName + fileExt;

			_wstring texPath = _wstring(filePath.begin(), filePath.end());

			// Texture 만들기 까지는 성공.
			HRESULT hr = CreateWICTextureFromFile(m_pDevice, texPath.c_str(), nullptr, &pSRV);

			if (FAILED(hr))
			{
				return E_FAIL;
			}


			//texPath = L"Material Name (" + to_wstring(iNum) + L") " + texPath;
			//MSG_BOX(texPath.c_str());

			m_SRVs[i].emplace_back(pSRV);
		}
	}

	iNum++;

	return S_OK;
}

HRESULT CTool_MeshMaterial::Initialize_GLB(const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiScene, string strDirPath)
{
	const aiMaterial* mat = pAIMaterial;
	const aiScene* scene = pAiScene;
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		aiString str;
		mat->GetTexture(static_cast<aiTextureType>(i), 0, &str);
		const char* pathStr = str.C_Str();
		if (pathStr[0] == '*') {
			int texIdx = atoi(pathStr + 1);

			const aiTexture* embeddedTexture = scene->mTextures[texIdx];
			std::string format = embeddedTexture->achFormatHint;
			HRESULT hr = E_FAIL;
			ID3D11ShaderResourceView* texture = nullptr;

			if (format == "dds") {
				hr = CreateDDSTextureFromMemory(
					m_pDevice,
					reinterpret_cast<const uint8_t*>(embeddedTexture->pcData),
					embeddedTexture->mWidth,
					nullptr,
					&texture
				);
			}
			else {
				hr = CreateWICTextureFromMemory(m_pDevice,
					reinterpret_cast<const uint8_t*>(embeddedTexture->pcData),
					embeddedTexture->mWidth,
					nullptr,
					&texture);
			}

			if (FAILED(hr))
				MSG_BOX(TEXT("임베디드 텍스처 생성 실패"));

			m_SRVs[i].emplace_back(texture);
		}
	}


	return S_OK;
}

CTool_MeshMaterial* CTool_MeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiScene)
{
	CTool_MeshMaterial* pInstance = new CTool_MeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial, pAiScene)))
	{
		MSG_BOX(TEXT("Create Failed : CTool_MeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_MeshMaterial::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
