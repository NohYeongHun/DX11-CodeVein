#pragma once
NS_BEGIN(Client)
class CFade_Out final : public CUIObject
{
public:
	typedef struct tagFadeOutDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
		POSTEX_SHADERPATH eShaderPath = {};
	}FADEOUT_DESC;

private:
	CFade_Out(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFade_Out(const CFade_Out& Prototype);
	virtual ~CFade_Out() = default;

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
	void Start_FadeIn();
	void Time_Calc(_float fTimeDelta);


private:
	_float m_fFadeTime = {};
	_bool m_IsFadeOut = { false };
	_bool m_IsFadeIn = { false };
#pragma endregion




private:
	LEVEL m_eCurLevel = { LEVEL::END };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	_uint m_iTextureIndex = {};

	_uint m_iShaderPath = {};
	_float m_fAlpha = {};
	_float m_fTime = {};

	

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resource();

public:
	static CFade_Out* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

