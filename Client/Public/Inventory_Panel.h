#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

/*
* 인벤토리 슬롯 객체를 고정해주는 위치용 객체.
* 실제 인벤 슬롯들을 생성해서 고정해둡니다.
*/
class CInventory_Panel final : public CUIObject
{
public:
	enum PANELTYPE
	{
		SKILL_PANEL = 0,
		ITEM_PANEL = 1,
		STATUS_PANEL = 2,
		STATUS_INFO_PANEL = 3,
		PANEL_END
	};

public:
	typedef struct tagInventoryPanel : public CUIObject::UIOBJECT_DESC
	{
		_float fSlot_SizeX{};
		_float fSlot_SizeY{};
		PANELTYPE ePanelType = {};
		_uint iInventorySlot = {};
		_uint iPanelIdx = {};

	}INVENTORY_PANEL_DESC;


private:
	explicit CInventory_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInventory_Panel(const CInventory_Panel& Prototype);
	virtual ~CInventory_Panel() = default;

public:
	void Change_Skill(_uint iSkillSlot, class CSkillUI_Icon* pSkillIcon, _uint iTextureIndex);

public:
	void Set_Visibility();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	vector<class CInventorySkill_Slot*> m_SkillSlots = {};
	vector<class CInventoryItem_Slot*> m_ItemSlots = {};
	vector<class CInventoryStatus_Icon*> m_StatusIcons = {};
	vector<class CInventoryStatus_Info*> m_StatusInfos = {};
	PANELTYPE m_ePanelType = {};
	_uint m_iPanelIdx = {};
	_uint m_iInventory_Slot = {};
	

	_bool m_IsVisibility = { };


private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs(INVENTORY_PANEL_DESC* pDesc);
	HRESULT Ready_Skill_Childs(INVENTORY_PANEL_DESC* pDesc);
	HRESULT Ready_Item_Childs(INVENTORY_PANEL_DESC* pDesc);
	HRESULT Ready_Status_Childs(INVENTORY_PANEL_DESC* pDesc);

public:
	static CInventory_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;


};
NS_END

