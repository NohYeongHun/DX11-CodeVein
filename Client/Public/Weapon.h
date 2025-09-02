#pragma once
#include "PartObject.h"

NS_BEGIN(Client)
class CWeapon abstract : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr }; // 장착할 뼈
		class CGameObject* pOwner = { nullptr };
		LEVEL eCurLevel = {}; // 현재 레벨
		_float fAttackPower = {}; // 데미지
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
	virtual void Finalize_Update(_float fTimeDelta);

	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


public:
	virtual void Update_Timer(_float fTimeDelta);


public:
	void Update_AttackDirection(_float fTimeDelta);

protected:
	// 스윙 방향 계산을 위한 변수들
	_vector m_vPreviousPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);  // 이전 프레임 위치
	_vector m_vSwingDirection = XMVectorSet(1.f, 0.f, 0.f, 0.f);    // 스윙 방향
	_bool m_bFirstFrame = true;  // 첫 프레임 여부
#pragma endregion



#pragma region DEBUG 용도
#pragma endregion


#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

public:
	virtual void Activate_ColliderFrame(_float fDuration);
	virtual void Activate_Collider();
	virtual void Deactivate_Collider();

public:
	virtual void Update_ColliderFrame(_float fTimeDelta);
	


public:
	void Set_AttackPower(_float fAttackPower) { m_fAttackPower = fAttackPower; }
	virtual _float Get_AttackPower() { return m_fAttackPower; }
	CGameObject* Get_Owner() { return m_pOwner; }

protected:
	class CCollider* m_pColliderCom = { nullptr };
	_bool m_IsColliderActive = { false };
	_float m_fColliderLifeTime = { true };

	//_uint* m_pParentState = { nullptr };
	_float m_fAttackPower = {}; // 공격 데미지.
#pragma endregion


#pragma region 2. Rendering 설정

public:
	void Set_Visible(_bool isVisible) { m_bVisible = isVisible; }
	_bool Is_Visible() { return m_bVisible; }



protected:
	_bool m_bVisible = { true };
	


#pragma endregion

#pragma region 3. 데미지 증가.

public:
	void Increase_Damage(_float fDamage);
	void Decrease_Damage(_float fDamage);
	
#pragma endregion

#pragma region 4. TRAIL 설정.

public:
	void Set_Trail(_bool isTrail) { m_bTrail = isTrail;  }

protected:
	_bool m_bTrail = { false };
#pragma endregion





#pragma region 0. 기본 값들.
protected:
	CShader* m_pShaderCom = { nullptr };
	CLoad_Model* m_pModelCom = { nullptr };
	CGameObject* m_pOwner = { nullptr };

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

