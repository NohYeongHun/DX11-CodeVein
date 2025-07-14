#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CInventoryStatus_Info final : public CUIObject
{
public:
	typedef struct tagStatusInfoDesc : CUIObject::UIOBJECT_DESC
	{

	}STATUS_INFO_DESC;

private:
	explicit CInventoryStatus_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInventoryStatus_Info(const CInventoryStatus_Info& Prototype);
	virtual ~CInventoryStatus_Info() = default;

public:
	// Status Info 체크.
	void Change_Info(const _wstring& strTextureTag , _uint iTextureIndex);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

	_uint m_iTextureIndex = {};

	_wstring m_statusString = {};


private:
	void Render_Info();

private:
	HRESULT Ready_Components();
	HRESULT Ready_Render_Resources();

public:
	static CInventoryStatus_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

