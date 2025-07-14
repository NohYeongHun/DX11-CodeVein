#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CHUD final : public CUIObject
{
public:
	enum SKILLPANEL : _uint
	{
		SKILL_PANEL_TOP = 0,
		SKILL_PANEL_BOTTOM = 1,
		SKILL_PANEL_END = 2
	};

	enum STATUSPANEL : _uint
	{
		STATUS_PANEL_BAR = 0,
		STATUS_PANEL_END = 1
	};

private:
	explicit CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHUD(const CHUD& Prototype);
	virtual ~CHUD() = default;

public:
	const _bool Get_Visibility();
	void Set_Visibility(_bool IsVIsibility);

public:
	// 몇 번째 스킬 패널의 몇번 슬롯에 Skill을 변경할 것인지? => 나중에 이벤트 버스로 호출.
	void Change_Skill(_uint iSkillPanel, _uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);
	// 스킬 사용 함수. => 이벤트 버스로 호출.
	void Execute_Skill(_uint iSkillPanel, _uint iSkillSlot, _float fSkillCoolTime);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Childs();
	HRESULT Ready_SkillPanel();
	HRESULT Ready_StatusPanel();
	HRESULT Ready_Events();


private:
	vector<class CSkill_Panel*> m_SkillPanels = {};
	vector<class CStatusPanel*> m_StatusPanels = {};

	vector<EventType> m_Events = {};
	_bool m_IsVisibility = { };

public:
	static CHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

