#pragma once

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
	HRESULT Initialize();

#pragma region ENGINE에 제공
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();
#pragma endregion

	

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	list<class CGameObject*>				m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];


#pragma region 장면 캡쳐(직교 투영 텍스쳐)를 만들기 위해 소유해야할 것들.
private:
	class CShader* m_pShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	_float4x4 m_ViewMatrix{}, m_ProjMatrix{};
#pragma endregion



private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_StaticUI();

	HRESULT Render_Debug();


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

NS_END