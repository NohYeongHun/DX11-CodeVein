#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CInventory final : public CUIObject
{
public:
	enum SKILL_PANEL : _uint
	{
		SKILL_PANEL_RIGHT_TOP = 0, // 오른쪽 위
		SKILL_PANEL_RIGHT_BOTTOM = 1, // 오른쪽 아래.
		SKILL_PANEL_LEFT = 2, // 왼쪽
		SKILL_PANEL_END
	};

	enum ITEM_PANEL : _uint
	{
		ITEM_PANEL_TOP = 0,
		ITEM_PANEL_END
	};

	enum STATUS_PANEL : _uint
	{
		STATUS_PANEL_TOP = 0,
		STATUS_PANEL_INFO = 1,
		STATUS_PANEL_END
	};

private:
	explicit CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInventory(const CInventory& Prototype);
	virtual ~CInventory() = default;


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
	HRESULT Ready_Components();
	HRESULT Ready_Childs();
	HRESULT Ready_SkillPanel();
	HRESULT Ready_ItemPanel();
	HRESULT Ready_StatusPanel();
	HRESULT Ready_Events();

	HRESULT Ready_Skills(); // 이벤트 호출로. 기본 스킬 채워넣기. 

private:
	HRESULT Ready_Render_Resources();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};

	vector<class CInventory_Panel*> m_InventorySkill_Panels = {};
	vector<class CInventory_Panel*> m_InventoryItem_Panels = {};
	vector<class CInventory_Panel*> m_InventoryStatus_Panels = {};

	vector<EventType> m_Events = {};
	_bool m_IsVisibility = { };


	

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;
};
NS_END

