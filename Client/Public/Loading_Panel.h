#pragma once
NS_BEGIN(Client)
/*
* 스킬 슬롯 객체를 고정해주는 위치용 객체.
* 실제 스킬 슬롯들을 생성해서 고정해둡니다.
*/
class CLoading_Panel final : public CUIObject
{
public:
	typedef struct tagLoadingPanelDesc : public CUIObject::UIOBJECT_DESC
	{
		_float fSlotSizeX{};
		_float fSlotSizeY{};
		POSTEX_SHADERPATH eNormalShaderPath = {};
		POSTEX_SHADERPATH eLoadingShaderPath = {};

	}LOADING_PANEL_DESC;

private:
	CLoading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_Panel(const CLoading_Panel& Prototype);
	virtual ~CLoading_Panel() = default;

public:
	/*void Change_Skill(_uint iSkillSlot, const _wstring& strTextureTag, _uint iTextureIndex);
	void Execute_Skill(_uint iSkillSlot);*/

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_uint m_iLoading_Slot = {};
	vector<class CLoading_Slot*> m_LoadingSlots = {};
	_uint m_iPrevIdx = {};
	_uint m_iCurrentIdx = {};
	_float m_fLoadingTime = {};
	_uint m_iNormalShaderPath = {};
	_uint m_iLoadingShaderPath = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs(LOADING_PANEL_DESC* pDesc);

public:
	static CLoading_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

