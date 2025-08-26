#pragma once


NS_BEGIN(Client)

class CSnow final : public CGameObject
{
public:
	typedef struct tagSnowDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
	}SNOW_DESC;
private:
	CSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnow(const CSnow& Prototype);
	virtual ~CSnow() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSnow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END