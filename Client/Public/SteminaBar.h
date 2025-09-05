#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* HP Bar 객체.
*/
class CSteminaBar final : public CUIObject
{
public:

private:
	CSteminaBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSteminaBar(const CSteminaBar& Prototype);
	virtual ~CSteminaBar() = default;

public:
	void Increase_Stemina(_uint iStemina, _float fTime);
	void Decrease_Stemina(_uint iStemina, _float fTime);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_uint  m_iTextureIndex = {};

	_uint  m_iAddStemina = {};
	
#pragma region Shader용 Stemina Increase, Decrease 효과
	_uint m_iShaderPath = {};
	_float m_fRightRatio = {};
	_float m_fLeftRatio = {};

	_float m_fIncreaseTime = {};

	_uint  m_iStemina = {};
	_uint  m_iMaxStemina = {};

	_bool  m_bIncrease = { false };
	_bool  m_bDecrease = { false };
#pragma endregion

	vector<EventType> m_Events = { };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_Events();

	void Ratio_Calc(_float fTimeDelta);

public:
	static CSteminaBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

