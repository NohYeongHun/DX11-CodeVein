#pragma once
#include "UIObject.h"

/*
* 로딩 때 나올 화면.
* 최상위 부모 객체로 할 것. => 자식 패널 만들어서 붙일 예정?
*/
NS_BEGIN(Client)
class CLoading_BackGround final : public CUIObject
{
private:
	CLoading_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_BackGround(const CLoading_BackGround& Prototype);
	virtual ~CLoading_BackGround() = default;

public:
	const _bool Get_Visibility();
	void Set_Visibility(_bool IsVIsibility);
	void Loading_End();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_bool m_IsVisibility = { false };

	_bool m_IsLoadingFadeOut = { false };
	_float m_fFade = {};

	_uint m_iTextureIndex = {};
	vector<pair<EventType, uint32_t>> m_Events = {};

	// 패널은 한개.
	vector<class CLoading_Panel*> m_LoadingPanels = {};

	_uint m_iShaderPath = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Events();
	HRESULT Ready_Childs();
	HRESULT Ready_Render_Resources();

public:
	static CLoading_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

