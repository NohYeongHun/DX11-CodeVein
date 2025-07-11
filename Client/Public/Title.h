#pragma once

#include "UIObject.h"


/*
* 로고 화면 타이틀에 출력되는 UI
*/

NS_BEGIN(Client)
class CTitle final : public CUIObject
{
public:
	enum class Text : _ubyte
	{
		TEXT = 0,
		ENUM_LINE = 1,
		END
	};

	enum class BackGround : _ubyte
	{
		BACKGROUND_BLACK = 0,
		BACKGROUND_WHITE = 1,
		BACKGROUND_WHITE_MODIFY_LIGHT = 2,
		BACKGROUND_WHITE_BASE_LIGHT = 3,
		BACKGROUND_WHITE_SHADE = 4,
		BACKGROUND_END
	};

private:
	CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle(const CTitle& Prototype);
	virtual ~CTitle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Destroy();

public:
	void Logo_End();

private:
	vector<CTitleText*> m_TitleTexts = {};
	vector<CTitle_BackGround*> m_TitleBackGruonds = {};

	vector<pair<EventType, uint32_t>> m_Events = {};

	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	_float			m_fChangeTime = {};

	_bool m_IsLogoFadeOut = { false };
	_float m_fFadeTime = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs();
	HRESULT Ready_Events();
	
	HRESULT Ready_Title_BackGround_Black();
	HRESULT Ready_Title_BackGround_White();
	HRESULT Ready_Title_BackGround_Light();
	HRESULT Ready_Title_BackGround_Modify_Light();
	HRESULT Ready_Title_BackGround_White_Shade();
	HRESULT Ready_Title_Text();
	
	

public:
	static CTitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END