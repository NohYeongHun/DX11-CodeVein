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
		PART_BODY = 2,
		PART_END
	};

	// 1 ~ 19은 커스텀사용
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

#pragma endregion

#pragma region 1. 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
	virtual void Collider_All_Active(_bool bActive);


public:
	virtual void Reset_Part_Colliders() override;

public:
	void WeaponOBB_ChangeExtents(_float3 vExtents);
	_float3 Get_WeaponOBBExtents();

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

	/* 무기를 특정 애니메이션에 회전 시켜놓기 위함.*/
public:
	virtual void Weapon_Rotation(_uint iPartType, _float3 vRadians, _bool bInverse = false) override;
	virtual void Encounter_Action() override;
#pragma endregion

#pragma region 7. 보스몹 체력 UI 관리
public:
	virtual void Take_Damage(_float fDamage, CGameObject* pGameObject) override;

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
	virtual void Set_Visible(_bool bVisible) override ;
#pragma endregion

#pragma region 9. Effect 객체 정의용 변수.
public:
	void Create_QueenKnightWarp_Effect_Particle(_float3 vDir);
	void Create_QueenKnightWarp_Effect_Particle_Spawn(_float3 vDir, _uint iSpawnCount);
	void Create_QueenKnightWarp_Effect_Particle_Explosion(_float3 vDir);

public:
	void Start_Dissolve(_float fDuration = 0.f); // Dissolve 재생.
	void ReverseStart_Dissolve(_float fDuration = 0.f); // Dissolve 역재생
	void End_Dissolve();


public:
	virtual void Enable_Trail(_uint iPartType) override;
	virtual void Disable_Trail(_uint iPartType) override;

private:
	_uint m_iShaderPath = {};
	_float m_fDissolveTime = {};

	_float m_fMaxDissolveTime = {};
	_float m_fCurDissolveTime = {};

	_float m_fReverseDissolveTime = {};
	_float m_fEndReverseDissolveTime = {};

	_bool m_IsDissolve = { false };
	_bool m_IsReverseDissolve = { false };


private:
	class CTexture* m_pDissolveTexture = { nullptr };


private:
	//CEffectParticle::EFFECTPARTICLE_ENTER_DESC m_EffectParticleEnterDesc = {};


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
	HRESULT Ready_PartObjects();
	HRESULT Ready_Effects(QUEENKNIGHT_DESC* pDesc);
	HRESULT Bind_Shader_Resource();
	
	

public:
	static CQueenKnight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
#pragma endregion


#ifdef _DEBUG
private:
	void ImGui_Render();
#endif // _DEBUG





};
NS_END

