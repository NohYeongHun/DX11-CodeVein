#include "MeshMaterial.h"

CMeshMaterial::CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext {pContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	static _uint iNum = 0;
	// 폴더 경로 지정. szDrive, szDir
	_char szDir[MAX_PATH] = {};

	/* 1. Dir 가져오기. */
	/* C:\Users\dkznd\Desktop\DX11\DX11Code\Mine\MyFramework\Framework\Client\Bin\Resources\Models\Player.fbx */
	/* Drvie : C:\ */
	/* Dir : Users\dkznd\Desktop\DX11\DX11Code\Mine\MyFramework\Framework\Client\Bin\Resources\Models\ */
	/* Filename : Player */
	/* Ext .fbx */
	_splitpath_s(pModelFilePath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	string strDirPath = string(szDir);

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

			m_SRVs[i].push_back(pSRV);

		}
	}

	iNum++;
	

	return S_OK;
}

HRESULT CMeshMaterial::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, aiTextureType eTextureType, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs[eTextureType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eTextureType][iTextureIndex]);
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX(TEXT("Create Failed : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshMaterial::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
