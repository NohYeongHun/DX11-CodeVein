#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

/*
* 스킬 아이콘 객체는 누가 가지고 관리하고 있다가 교체해줘야하지?
*/
class CSkillUI_Icon final : public CUIObject
{
public:
	typedef struct tagSkillIconDesc : CUIObject::UIOBJECT_DESC
	{
		const _tchar* pText = {};
		_uint iTextureIndex = {};
	}SKILLICON_DESC;

private:
	CSkillUI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillUI_Icon(const CSkillUI_Icon& Prototype);
	virtual ~CSkillUI_Icon() = default;

public:
	void Set_Visibility();
	void Update_SelectedInfo(_uint iPanelType, _uint iPanelIndex, _uint iSlotIndex);

public:
	void Change_Inventory_Skill();
	void Change_Skill(const _wstring& strTextureTag , _uint iTextureIndex);

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

	_wstring m_strTextureTag = {};
	_uint m_iTextureIndex = {};
	_bool m_IsVisibility = {};

	// 스킬 패널에 오기전에 선택한 인벤토리 스킬 슬롯 정보.
	_uint m_iSelect_PanelType = {};
	_uint m_iSelect_PanelIndex = {};
	_uint m_iSelect_SlotIndex = {};


private:
	HRESULT Ready_Components(SKILLICON_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CSkillUI_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

