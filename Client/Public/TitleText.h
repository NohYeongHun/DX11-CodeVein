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
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


#pragma region FadeOut
public:
	void Start_FadeOut();
	void Time_Calc(_float fTimeDelta);
#pragma endregion




private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_uint m_iTextureIndex = {};

	_uint m_iPassIdx = {};
	_float m_fAlpha = {};
	_float m_fTime = {};

	_float m_fFadeTime = {};
	_bool m_IsFadeOut = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resource();

public:
	static CTitleText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

