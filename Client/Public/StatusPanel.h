#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 플레이어 스테이터스 (왼쪽 아래를 고정해주는 패널)
* 실제 플레이어 스테이터스 정보를 고정해줍니다.
*/
class CStatusPanel final : public CUIObject
{
private:
	CStatusPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatusPanel(const CStatusPanel& Prototype);
	virtual ~CStatusPanel() = default;

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
	HRESULT Ready_Childs();

public:
	static CStatusPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

