#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CMap final : public CGameObject
{
public:
	typedef struct tagMapDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel = LEVEL::END;
		const _tchar* PrototypeTag;
		MESH_SHADERPATH eShaderPath = { MESH_SHADERPATH::NONE };
		_float3 vScale;
	}MAP_DESC;

private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& Prototype);
	virtual ~CMap() = default;

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
	class CLoad_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };
	_uint m_iShaderPath = {};

private:
	HRESULT Ready_Components(MAP_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

