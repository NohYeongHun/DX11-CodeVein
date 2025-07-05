#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CSkillSlot final : public CUIObject
{

public:
	typedef struct tagSkillSlotDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
	}SKILLSLOT_DESC;

private:
	CSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillSlot(const CSkillSlot& Prototype);
	virtual ~CSkillSlot() = default;

public:
	void Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex);

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
	class CSkillIcon* m_pSkill = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};


private:
	HRESULT Ready_Components(SKILLSLOT_DESC* pDesc);

public:
	static CSkillSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

