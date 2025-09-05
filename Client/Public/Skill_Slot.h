#pragma once
NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CSkill_Slot final : public CUIObject
{

public:
	typedef struct tagSkillSlotDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
		POSTEX_SHADERPATH eShaderPath = {};
	}SKILLSLOT_DESC;

private:
	CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Slot(const CSkill_Slot& Prototype);
	virtual ~CSkill_Slot() = default;

public:
	void Change_Skill(class CSkillUI_Icon* pSkillIcon, _uint iTextureIndex);
	void Execute_Skill(_float fSkillCoolTime);

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
	class CSkill_Icon* m_pSkill = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};
	_uint m_iShaderPath = {};
	_bool  m_IsCoolTime = { false };
	_float m_fCoolTime = { 0.f };
	_float m_fTime = {};


private:
	HRESULT Ready_Components(SKILLSLOT_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CSkill_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

