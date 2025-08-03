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
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
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

#pragma endregion



#pragma region 0. 기본 값들
private:
	//_uint* m_pParentState = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKnightLance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion

};
NS_END

