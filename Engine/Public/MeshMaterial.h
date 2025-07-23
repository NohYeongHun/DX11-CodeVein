#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CMeshMaterial final : public CBase
{

private:
	explicit CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMeshMaterial() = default;


public:
	HRESULT Initialize(const _char* pModelFilePath, const _char* pTextureFolderPath, const aiMaterial* pAIMaterial, const aiScene* pAIscene);
	HRESULT Bind_Resources(CShader* pShader, const _char* pConstantName, aiTextureType iTextureType, _uint iTextureIndex);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_SRVs[AI_TEXTURE_TYPE_MAX];

private:
	HRESULT Initialize_FBX(const _char* pModelFilePath, const _char* pTextureFolderPath, const aiMaterial* pAIMaterial, string strDirPath);
	HRESULT Initialize_GLB(const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiScene, string strDirPath);

public:
	static CMeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const _char* pTextureFolderPath, const aiMaterial* pAIMaterial, const aiScene* pAiScene);
	virtual void Free();
};
NS_END

