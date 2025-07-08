#pragma once

#include "UIObject.h"


NS_BEGIN(Client)

class CTitle_BackGround final : public CUIObject
{
public:
	typedef struct tagTitleBackGroundDesc : public CUIObject::UIOBJECT_DESC
	{
		_uint iTextureCount = {};
		_uint iTexture = {};
		_uint iPassIdx = { 0 };
		_float fAlpha = { 0.5f };
	}TITLE_BAKCGROUND_DESC;
private:
	CTitle_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle_BackGround(const CTitle_BackGround& Prototype);
	virtual ~CTitle_BackGround() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	_uint					m_iTextureCount = {};
	_uint					m_iTextureIdx = {};
	_uint					m_iPassIdx = {};
	
	_uint m_iRandID = { };

	_float					m_fAlpha = {};
	_float					m_fTexture_ChangeTime = {};
	_float					m_fTime = {};


private:
	HRESULT Ready_Components();

public:
	static CTitle_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END