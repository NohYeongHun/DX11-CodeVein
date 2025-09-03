#pragma once
#include "Weapon.h"

NS_BEGIN(Client)
class CKnightLance final : public CWeapon
{
public:
	typedef struct tagKnightLanceDesc : public CWeapon::WEAPON_DESC
	{

	}KNIGHT_LANCE_DESC;

#pragma region 0. 기본 함수들
private:
	CKnightLance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKnightLance(const CKnightLance& Prototype);
	virtual ~CKnightLance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override ;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Finalize_Update(_float fTimeDelta) override;
	virtual HRESULT Render();
#pragma endregion


#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);


public:
	void OBBCollider_ChangeExtents(_float3 vExtents);
	const _float3 Get_OriginExtents() { return m_vOriginExtents; }

private:
	_float3 m_vOriginExtents = {};
#pragma endregion



#pragma region 0. 기본 값들
private:
	//_uint* m_pParentState = { nullptr };

#pragma region 1. WEAPON TRAIL
public:
	void TrailWeapon_Update(_matrix WeaponSocketMatrix);

private:
	class CSwordTrail* m_pTrailWeapon_Effect = { nullptr };
	_float3 m_vPointUp = {};
	_float3 m_vPointDown = {};

#pragma endregion



private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();
	HRESULT Ready_Effects();
	HRESULT Bind_ShaderResources();

public:
	static CKnightLance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion

#pragma region 2. EFFECT
public:
	void Start_Dissolve(); // Dissolve 재생.
	void ReverseStart_Dissolve(); // Dissolve 역재생.
	void End_Dissolve();

private:
	_uint m_iShaderPath = {};
	_float m_fDissolveTime = {};
	_float m_fEndDissolveTime = {};
#pragma endregion


#ifdef _DEBUG
private:
	void ImGui_Render();

private:
	const _float4x4* m_pMatrixArray[4] = { nullptr };
#endif // _DEBUG

};
NS_END

