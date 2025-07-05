﻿#pragma once

#include "Base.h"

/* 추가하는 기능. */
/* 화면에 그려질 객체들을 그리는 순서대로 분류하여 보관한 클래스 */
/* 보관한 순서대로 객첻르의 렌더콜을 해준다. */

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize_Clone();

#pragma region ENGINE에 제공
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

	HRESULT Apply_BlendeState();
	HRESULT Apply_DepthStencilOff();
	HRESULT Apply_DefaultStates();
#pragma endregion

	

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };

	list<class CGameObject*>				m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];

#pragma region BlendState 설정.
	ID3D11BlendState* m_pAlphaBlend = nullptr;
	ID3D11DepthStencilState* m_pDepthOff = nullptr;
	ID3D11DepthStencilState* m_pDepthOn = nullptr;

#pragma endregion


private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Ready_Render_State();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

NS_END