#pragma once

NS_BEGIN(Client)
class CBossHpBarUI final : public CUIObject
{
public:
	typedef struct tagBossHpBarDesc : CUIObject::UIOBJECT_DESC
	{
		_wstring strName = {};
		_uint iTextureIndex = {};
		_float fMaxHp = {};
	}BOSS_HPBAR_DESC;

private:
	CBossHpBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHpBarUI(const CBossHpBarUI& Prototype);
	virtual ~CBossHpBarUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};

	/* 스킬 정보? */
	_float  m_fHp = {};
	_float  m_fMaxHp = {};
	_wstring m_strName = {};

private:
	void Render_Text();

#pragma region HP 깎는 효과를 위한 SHADER 용도
public:
	void Increase_Hp(_float fHp, _float fTime);
	void Decrease_Hp(_float fHp, _float fTime);


private:
	_bool m_bDecrease = { false };
	_bool m_bIncrease = { false };

	_float m_fRightRatio = {};
	_float m_fLeftRatio = {};



#pragma endregion



private:
	HRESULT Ready_Components(BOSS_HPBAR_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CBossHpBarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;
};
NS_END

