#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CTitleBackGround final : public CUIObject
{
private:
	CTitleBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitleBackGround(const CTitleBackGround& Prototype);
	virtual ~CTitleBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CTitleBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

