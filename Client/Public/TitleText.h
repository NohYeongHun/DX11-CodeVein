#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CTitleText final : public CUIObject
{
public:
	typedef struct tagTitleTextDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
	}TITLETEXT_DESC;

private:
	CTitleText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitleText(const CTitleText& Prototype);
	virtual ~CTitleText() = default;

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
	_float m_fChangeTime = {};
	_uint m_iTextureIndex = {};
	

private:
	HRESULT Ready_Components();

public:
	static CTitleText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

