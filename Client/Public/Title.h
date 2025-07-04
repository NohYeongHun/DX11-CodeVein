#pragma once

#include "UIObject.h"


/*
* 로고 화면 타이틀에 출력되는 UI
*/

NS_BEGIN(Client)
class CTitle final : public CUIObject
{
private:
	CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle(const CTitle& Prototype);
	virtual ~CTitle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	_float					m_fChangeTime = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs();

public:
	static CTitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END