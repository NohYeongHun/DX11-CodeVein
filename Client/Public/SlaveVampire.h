#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CSlaveVampire final : public CMonster
{
public:
	// 어떤 타입의 Collider를 제어할것인지 정의.
public:
	// 어떤 타입의 PART OBJECT 를 제어할것인지 정의.
	enum PART_TYPE : _uint
	{
		PART_WEAPON = 0,
		PART_END
	};

public:
	typedef struct tagSlaveVampireDesc : public CMonster::MONSTER_DESC
	{

	}SLAVE_VAMPIRE_DSEC;

#pragma region 기본 함수들
private:
	explicit CSlaveVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSlaveVampire(const CSlaveVampire& Prototype);
	virtual ~CSlaveVampire() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


#pragma endregion



#pragma region 0. 몬스터는 충돌에 대한 상태제어를 할 수 있어야한다. => 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
	virtual void Collider_All_Active(_bool bActive);
	// 무기 정의.
private:
	class CSlaveVampireSword* m_pWeapon = { nullptr };
#pragma endregion


#pragma region 1. AI 관리
public:
	virtual void Update_AI(_float fTimeDelta) override;

	virtual void Start_Dissolve(_float fDuration = 0.f) override; // Dissolve 재생.
	virtual void ReverseStart_Dissolve(_float fDuration = 0.f) override; // Dissolve 역재생
	virtual void End_Dissolve() override;
	virtual void Dead_Action() override;

private:
	class CMonsterTree* m_pTree = { nullptr };

	class CTexture* m_pDissolveTexture = { nullptr };

#pragma endregion

#pragma region 2. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
public:
	virtual HRESULT Initialize_Stats() override;
#pragma endregion


#pragma region 3. 몬스터는 내 애니메이션이 무엇인지 알아야한다.
public:
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion

#pragma region 4. 몬스터는 자신이 어떤 버프를 소유할 수 있는지를 알아야 한다. => 그리고 그에 맞는 쿨타임도 알아야한다.(몬스터마다 달라질 수 있다.)
public:
	virtual HRESULT Initialize_BuffDurations() override;
#pragma endregion



#pragma region 5. 특수한 상태를 제어하기 위한 함수들
public:
	virtual void Enable_Collider(_uint iType) override;
	virtual void Disable_Collider(_uint iType) override;

public:
	virtual void PlayHitSound() override;
	virtual void PlayWeaponSound() override;

private:
	_bool m_bPlayWeaponSound = { false };
	_float m_fPlayAttackSound = {};
#pragma endregion

#pragma region 6. 기본적으로 몬스터가 필요한 상태들을 정의합니다.
private:
	HRESULT Ready_Components(SLAVE_VAMPIRE_DSEC* pDesc);
	HRESULT Ready_Navigations();
	HRESULT Ready_BehaviorTree();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();
	HRESULT Initialize_ColliderFrames();
#pragma endregion

#pragma region 7. Monster HP Bar UI 관리
public:
	virtual void Take_Damage(_float fDamage, CGameObject* pGameObject) override;
public:
	virtual void Increase_HpUI(_float fHp, _float fTime);
	virtual void Decrease_HpUI(_float fHp, _float fTime);


private:
	class CMonsterHpBar* m_pMonsterHpBar = { nullptr };

private:
	HRESULT Initialize_UI();
#pragma endregion



public:
	static CSlaveVampire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
};
NS_END

