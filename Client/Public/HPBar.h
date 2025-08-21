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
	void Increase_Hp(_float fHp, _float fTime);
	void Decrease_Hp(_float fHp, _float fTime);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Render_HP();


private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_uint  m_iTextureIndex = {};

#pragma region Shader용 HP Increase, Decrease 효과
	_float m_fRightRatio = {};
	_float m_fLeftRatio = {};

	_float  m_fHp = {};
	_float  m_fMaxHp = {};

	_bool  m_bIncrease = { false };
	_bool  m_bDecrease = { false };
#pragma endregion

	vector<EventType> m_Events = { };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_Events();

public:
	static CHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

