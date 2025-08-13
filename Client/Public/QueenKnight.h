#pragma once

NS_BEGIN(Client)
class CQueenKnight final : public CMonster
{
public:
	// 어떤 타입의 PART OBJECT 를 제어할것인지 정의.
	enum PART_TYPE : _uint
	{
		PART_WEAPON = 0,
		PART_SHIELD = 1,
		PART_END
	};

	// 1 ~ 20은 커스텀사용
	enum QUEEN_BUFF_FLAGS : _uint
	{
		QUEEN_BUFF_NONE = 0,
		QUEEN_BUFF_SPECIAL_ATTACK1 = 1 << 1, // 몬스터 상태와 안곂치게
		QUEEN_BUFF_SPECIAL_ATTACK2 = 1 << 2, // 몬스터 상태와 안곂치게
		QUEEN_BUFF_SPECIAL_ATTACK3 = 1 << 3, // 몬스터 상태와 안곂치게
		QUEEN_BUFF_PHASE_FIRST = 1 << 4, // 어떤 페이즈인지?
		QUEEN_BUFF_PHASE_SECOND = 1 << 5,
		QUEEN_BUFF_PHASE_LAST = 1 << 6,
		QUEEN_BUFF_PHASE_ATTACK_COOLDOWN = 1 << 7, // 페이즈 마다 다른 공격 시퀀스 => 쿨타임 존재
		QUEEN_BUFF_DASH_ATTACK_COOLDOWN = 1 << 8, // 돌진 공격 시퀀스 => 쿨타임 존재.
		QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN = 1 << 9, // Down Strike 시퀀스 => 쿨타임 존재.
		QUEEN_BUFF_END = 1 << 30
	};

public:
	typedef struct tagQueenKnightDesc : public CMonster::MONSTER_DESC
	{

	}QUEENKNIGHT_DESC;


private:
	explicit CQueenKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CQueenKnight(const CQueenKnight& Prototype);
	virtual ~CQueenKnight() = default;

#pragma region 0. 기본 함수 관리
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 1. 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);


#pragma endregion


#pragma region 2. 몬스터 AI 관리.
public:
	// AI에 대한 호출 순서를 정의합니다.
	virtual void Update_AI(_float fTimeDelta) override;

private:
	class CQueenKnightTree* m_pTree = { nullptr };
#pragma endregion

#pragma region 3. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
public:
	virtual HRESULT Initialize_Stats() override;


public:
	_bool Is_TargetDashRange();
	_bool Is_TargetDodgeRange();
	_bool Is_TargetDownStrikeRange();


private:
	_float m_fDashMaxDistance = {};
	_float m_fDashMinDistance = {};
	_float m_fDashDodgeDistance = {};
	_float m_fDownStrikeDistance = {};
#pragma endregion

#pragma region 4. 몬스터는 자신의 애니메이션을 관리해야한다.
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion

#pragma region 5. 사용되는 버프/디버프에 대한 타이머를 관리해야한다.
	virtual HRESULT Initialize_BuffDurations() override;
#pragma endregion

#pragma region 6. 특수한 상태를 제어하기 위한 함수들입니다.

	
public:
	/* 어떤 파츠의 Colider를 제어할 것인지? */
	virtual void Enable_Collider(_uint iType) override;
	virtual void Disable_Collider(_uint iType) override;
#pragma endregion

#pragma region 7. 보스몹 체력 UI 관리
public:
	virtual void Take_Damage(_float fDamage) override;

	virtual void Increase_HpUI(_float fHp, _float fTime) override;
	virtual void Decrease_HpUI(_float fHp, _float fTime) override;


private:
	HRESULT Initailize_UI();
	

private:
	class CBossHpBarUI* m_pBossHpBarUI = { nullptr };

public:
	
#pragma endregion


#pragma region 8. 렌더링 제어
public:
	void Set_Visible(_bool bVisible);
	_bool Is_Visible() const { return m_bVisible; }

private:
	_bool m_bVisible = { true };  // 기본적으로 보이는 상태
#pragma endregion

#pragma region 0. 기본 함수들 정의
private:
	class CKnightLance* m_pWeapon = { nullptr };
	class CKnightShield* m_pShield = { nullptr };

private:
	HRESULT Ready_Components(QUEENKNIGHT_DESC* pDesc);
	HRESULT Ready_Colliders(QUEENKNIGHT_DESC* pDesc);
	HRESULT Ready_Navigations();
	HRESULT Ready_BehaviorTree();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();

public:
	static CQueenKnight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
#pragma endregion


};
NS_END

