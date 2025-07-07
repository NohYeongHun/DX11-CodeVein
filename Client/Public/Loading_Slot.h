#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CLoading_Slot final : public CUIObject
{


private:
	CLoading_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_Slot(const CLoading_Slot& Prototype);
	virtual ~CLoading_Slot() = default;

public:
	void Change_Shader_Pass(_uint iPass)
	{
		m_iCurrentPass = iPass;
	}
	/*void Change_Skill(const _wstring& strTextureTag, _uint iTextureIndex);
	void Execute_Skill();*/

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CSkill_Icon* m_pSkill = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};
	_uint m_iCurrentPass = {};

	_bool  m_IsCoolTime = { false };
	_float m_fCoolTime = { 2.f };
	_float m_fTime = {};


private:
	HRESULT Ready_Components();

public:
	static CLoading_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

