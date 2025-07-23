#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_MeshMaterial final : public CBase
{

private:
	explicit CLoad_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoad_MeshMaterial() = default;


public:
	HRESULT Initialize(std::ifstream& ifs, _wstring textureFolderPath);
	HRESULT Bind_Resources(CShader* pShader, const _char* pConstantName, aiTextureType iTextureType, _uint iTextureIndex);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*> m_SRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CLoad_MeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::ifstream& ifs, _wstring textureFolderPath);
	virtual void Free();
};
NS_END

