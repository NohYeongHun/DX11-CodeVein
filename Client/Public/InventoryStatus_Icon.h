#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

/* 인벤토리 위에 있는 아이콘들. */
class CInventoryStatus_Icon final : public CUIObject
{
public:
	typedef struct tagStatusIconDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
	}STATUS_ICON_DESC;

private:
	explicit CInventoryStatus_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInventoryStatus_Icon(const CInventoryStatus_Icon& Prototype);
	virtual ~CInventoryStatus_Icon() = default;

public:
	void Set_Visibility() { m_IsVisibility = !m_IsVisibility; }

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
	_bool m_IsVisibility = {};
	/* 스킬 정보? */
	


private:
	HRESULT Ready_Components(STATUS_ICON_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CInventoryStatus_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

