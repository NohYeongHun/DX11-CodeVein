#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* HP Bar 객체.
*/
class CHPBar final : public CUIObject
{
public:

private:
	CHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPBar(const CHPBar& Prototype);
	virtual ~CHPBar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};


private:
	HRESULT Ready_Components();

public:
	static CHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

