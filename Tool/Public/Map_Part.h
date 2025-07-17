#pragma once
#include "GameObject.h"

NS_BEGIN(Tool)
class CMap_Part final : public CGameObject
{
public:
	typedef struct tagMapPartDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _tchar* pModelTag;
	}MAP_PART_DESC;

private:
	CMap_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Part(const CMap_Part& Prototype);
	virtual ~CMap_Part() = default;
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
#pragma endregion


private:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	const _tchar* m_pModelTag = { nullptr };
	_wstring m_PartName = {  };
	int m_iPartID = {};


private:
	HRESULT Ready_Components(MAP_PART_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CMap_Part* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

