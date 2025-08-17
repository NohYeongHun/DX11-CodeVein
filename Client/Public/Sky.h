#pragma once


NS_BEGIN(Client)

class CSky final : public CGameObject
{
public:
	typedef struct tagSkyDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel = LEVEL::END;
		const _tchar* PrototypeTag;
		_float3 vScale = { 1.f, 1.f, 1.f };
	}SKY_DESC;

private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& Prototype);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CLoad_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END