#pragma once
#include "PartObject.h"

NS_BEGIN(Client)
class CPlayerWeapon final : public CWeapon
{
public:
	typedef struct tagPlayerWeaponDesc : public CWeapon::WEAPON_DESC
	{

	}PLAYER_WEAPON_DESC;

#pragma region 0. 기본 함수들
private:
	CPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerWeapon(const CPlayerWeapon& Prototype);
	virtual ~CPlayerWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta);

	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

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

	/* Weapon의 최종 행렬 가져오기. */
public:
	const _float4x4* Get_CombinedWorldMatrix() { return &m_CombinedWorldMatrix; }

	/* 무기의 스윙 방향 가져오기 */
	_vector Get_SwingDirection() const { return m_vSwingDirection; }

#pragma endregion


#pragma region TRAIL
public:
	void TrailWeapon_Update(_matrix WeaponSocketMatrix);

private:
	class CSwordTrail* m_pTrailWeapon_Effect = { nullptr };

	_float4x4* m_pWeaponTrailStart_SocketMatrix = {};
	_float4x4* m_pWeaponTrailEnd_SocketMatrix = {};

	_float3					m_vPointUp = {};
	_float3					m_vPointDown = {};
#pragma endregion


#pragma region 0. 기본 값들




private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();
	HRESULT Ready_Effects();
	HRESULT Bind_ShaderResources();

public:
	static CPlayerWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion



};
NS_END

