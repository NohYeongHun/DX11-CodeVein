#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CInventoryItem_Slot final : public CUIObject
{
public:
	typedef struct tagSkillSlotDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
	}SKILLSLOT_DESC;

private:
	CInventoryItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryItem_Slot(const CInventoryItem_Slot& Prototype);
	virtual ~CInventoryItem_Slot() = default;

public:
	void Change_Item(const _wstring& strTextureTag, _uint iTextureIndex);
	

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
	class CInventoryItem_Icon* m_pItem = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};


private:
	HRESULT Ready_Components(SKILLSLOT_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CInventoryItem_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

