#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

/*
* UI가 크기를 가지고 있잖아?
* Begin 할때만 크기 더해서 Set_State해주고
* 행렬은 그대로 쓰면되잖아?
*/
class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fX{}, fY{}, fSizeX{}, fSizeY{};
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region COMPOSITE Pattern 용도
public:
	_fmatrix Get_RenderMatrix();
	//_fmatrix Get_LocalMatrix();
public:
	// UI 자식 객체 추가.
	void AddChild(CUIObject* pChild);
	void RemoveChild(CUIObject* pChild);
	void Set_Parent(CUIObject* pParent);
#pragma endregion




protected:
	CUIObject*				m_pParent = { nullptr };
	vector<CUIObject*>		m_Children = {};
	_float4x4				m_RenderMatrix = {}; // 부모의 행렬을 곱한 최종 렌더링 위치.
	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};
	
	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float					m_iWinSizeX{}, m_iWinSizeY{};

protected:
	void Update_Transform();

	HRESULT Begin();
	HRESULT End();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END