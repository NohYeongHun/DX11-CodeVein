#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CSkillUI_Slot final : public CUIObject
{

public:
	typedef struct tagSkillSlotDesc : CUIObject::UIOBJECT_DESC
	{
		const _tchar* pText = {};
		_uint iTextureIndex = {};
		_uint iSlotIndex = {};
	}SKILLSLOT_DESC;

private:
	CSkillUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillUI_Slot(const CSkillUI_Slot& Prototype);
	virtual ~CSkillUI_Slot() = default;

public:
	void Set_Visibility();
	void Update_SelectedInfo(_uint iPanelType, _uint iPanelIndex, _uint iSlotIndex);
public:
	void Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex);

	void Change_Inventory_Skill();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Ready_Childs();

private:
	class CSkillUI_Icon* m_pSkill = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iIcon_TextureIndex = {};
	_uint m_iSlotIndex = {};
	_bool m_IsVisibility = {};
	_wstring m_strTextureTag = {};

	// 스킬 패널에 오기전에 선택한 스킬 슬롯 정보.
	_uint m_iSelect_PanelType = {};
	_uint m_iSelect_PanelIndex = {};
	_uint m_iSelect_SlotIndex = {};

private:
	HRESULT Ready_Components(SKILLSLOT_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CSkillUI_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

