#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

/*
* 인벤토리 슬롯 객체를 고정해주는 위치용 객체.
* 실제 인벤 슬롯들을 생성해서 고정해둡니다.
*/
class CSkillUI_Panel final : public CUIObject
{
public:
	enum PANELTYPE
	{
		SKILL_UI = 0,
		SKILL_INFO = 1,
		PANEL_END
	};

public:
	typedef struct tagSkillUIPanelDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fSlot_SizeX{};
		_float fSlot_SizeY{};
		PANELTYPE ePanelType = {};
		_uint iSkillSlot = {};

	}SKILLUI_PANEL_DESC;

private:
	explicit CSkillUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkillUI_Panel(const CSkillUI_Panel& Prototype);
	virtual ~CSkillUI_Panel() = default;

public:
	void Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	vector<class CSkill*> m_SkillSlots = {};
	
	PANELTYPE m_ePanelType = {};
	_uint m_iInventory_Slot = {};
	


private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs(SKILLUI_PANEL_DESC* pDesc);
	HRESULT Ready_Skill_Childs(SKILLUI_PANEL_DESC* pDesc);
	HRESULT Ready_Item_Childs(SKILLUI_PANEL_DESC* pDesc);

public:
	static CSkillUI_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;


};
NS_END

