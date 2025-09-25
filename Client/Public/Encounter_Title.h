#pragma once
NS_BEGIN(Client)
class CEncounter_Title final : public CUIObject
{
public:
	typedef struct tagTitleTextDesc : CUIObject::UIOBJECT_DESC
	{

		_wstring strTextureTag = {};
		_float fFadeTime = {};
		POSTEX_SHADERPATH eShaderPath = {};
	}ENCOUNTER_TITLE_DESC;

private:
	CEncounter_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEncounter_Title(const CEncounter_Title& Prototype);
	virtual ~CEncounter_Title() = default;

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
	_uint m_iShaderPath = {};

	_uint m_iTextureIndex = {};

	_float m_fAlpha = {};
	_float m_fTime = {};

	_float m_fFadeEnd = {};
	_float m_fFadeTime = {};
	_bool m_IsFadeOut = { false };
	_uint m_iFrameCount = { 0 };

	_wstring m_strTextureTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resource();

public:
	static CEncounter_Title* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

