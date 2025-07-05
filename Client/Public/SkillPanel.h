#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체를 고정해주는 위치용 객체.
* 실제 스킬 슬롯들을 생성해서 고정해둡니다.
*/
class CSkillPanel final : public CUIObject
{
public:
	typedef struct tagSkillPanelDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fSkillSizeX{};
		_float fSkillSizeY{};

	}SKILLPANEL_DESC;

private:
	CSkillPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillPanel(const CSkillPanel& Prototype);
	virtual ~CSkillPanel() = default;

public:
	void Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);

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
	vector<class CSkillSlot*> m_SkillSlots = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs(SKILLPANEL_DESC* pDesc);

public:
	static CSkillPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

