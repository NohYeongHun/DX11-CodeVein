#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CSkillUI final : public CUIObject
{
public:
	enum PANEL : _uint
	{
		SKILLINFO_PANEL = 0,
		SKILLUI_PANEL = 1,
		PANEL_END
	};

private:
	explicit CSkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkillUI(const CSkillUI& Prototype);
	virtual ~CSkillUI() = default;


public:
	const _bool Get_Visibility();
	void Set_Visibility();

public:
	// 몇 번째 스킬 패널의 몇번 슬롯에 Skill을 변경할 것인지? => 나중에 이벤트 버스로 호출.
	void Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);

	

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Childs();
	HRESULT Ready_Events();
	HRESULT Ready_Skills(); // SKill 교체 창이므로 기본적으로 가지고 있는 스킬이 존재해야됨.

private:

	vector<class CSkillUI_Panel*> m_SkillUI_Panels = {};
	vector<EventType> m_Events = {};
	_bool m_IsVisibility = { };

	// 스킬 패널에 오기전에 선택한 스킬 슬롯 정보.
	_uint m_iSelect_PanelType = {};
	_uint m_iSelect_PanelIndex = {};
	_uint m_iSelect_SlotIndex = {};

public:
	static CSkillUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;
};
NS_END

