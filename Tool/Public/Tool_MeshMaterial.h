#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CTool_MeshMaterial final : public CBase
{

private:
	explicit CTool_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_MeshMaterial() = default;


public:
	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAIscene);
	HRESULT Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, aiTextureType iTextureType, _uint iTextureIndex);
	HRESULT Initialize_FBX(const _char* pModelFilePath, const aiMaterial* pAIMaterial, string strDirPath);
	HRESULT Initialize_GLB(const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiScene, string strDirPath);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_SRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CTool_MeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial, const aiScene* pAiScene);
	virtual void Free();
};
NS_END

