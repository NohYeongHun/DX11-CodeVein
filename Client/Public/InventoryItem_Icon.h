#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

/*
* 스킬 아이콘 객체는 누가 가지고 관리하고 있다가 교체해줘야하지?
*/
class CInventoryItem_Icon final : public CUIObject
{
public:
	typedef struct tagSkillIconDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
	}SKILLICON_DESC;

private:
	CInventoryItem_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryItem_Icon(const CInventoryItem_Icon& Prototype);
	virtual ~CInventoryItem_Icon() = default;

public:
	void Change_Item(const _wstring& strTextureTag , _uint iTextureIndex);

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

	_uint m_iTextureIndex = {};

	/* 스킬 정보? */
	


private:
	HRESULT Ready_Components(SKILLICON_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CInventoryItem_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

