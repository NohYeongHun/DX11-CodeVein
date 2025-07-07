#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체를 고정해주는 위치용 객체.
* 실제 스킬 슬롯들을 생성해서 고정해둡니다.
*/
class CSkill_Panel final : public CUIObject
{
public:
	typedef struct tagSkillPanelDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fSkillSizeX{};
		_float fSkillSizeY{};

	}SKILLPANEL_DESC;

private:
	CSkill_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Panel(const CSkill_Panel& Prototype);
	virtual ~CSkill_Panel() = default;

public:
	void Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);
	void Execute_Skill(_uint iSkillSlot, _float fSkillCoolTime);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:

private:
	_uint m_iSkillSlot = {};
	vector<class CSkill_Slot*> m_SkillSlots = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs(SKILLPANEL_DESC* pDesc);

public:
	static CSkill_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

