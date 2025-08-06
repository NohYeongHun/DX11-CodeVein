#pragma once
#include "PartObject.h"

NS_BEGIN(Client)
class CWeapon abstract : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr }; // 장착할 뼈
		LEVEL eCurLevel = {}; // 현재 레벨
	}WEAPON_DESC;

#pragma region 0. 기본 함수들
protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma endregion

#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

protected:
	class CCollider* m_pColliderCom = { nullptr };
#pragma endregion



#pragma region 0. 기본 값들.
protected:
	CShader* m_pShaderCom = { nullptr };
	CLoad_Model* m_pModelCom = { nullptr };


protected:
	const _float4x4* m_pSocketMatrix = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
#pragma endregion



};
NS_END

