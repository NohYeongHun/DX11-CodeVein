#pragma once
NS_BEGIN(Client)
class CPlayer final : public CContainerObject
{

#pragma region PLAYER STATE 정의 STATE != ANIMATION
public:
	// Add_State 순서대로 넣습니다.
	enum PLAYER_STATE : _int
	{
		IDLE = 0, WALK, RUN, DODGE,
		STRONG_ATTACK, GUARD, ATTACK, DAMAGE,
		SKILL_1, SKILL_2,
		STATE_END
	};

	// 플레이어 버프 정의
	enum BUFF_FLAGS : _uint
	{
		BUFF_NONE = 0,
		BUFF_HIT = 1  << 24,
		BUFF_DOWN = 1 << 25,
		BUFF_STUN = 1 << 26,
		BUFF_INVINCIBLE = 1 << 27, // 무적시간.
		BUFF_END
	};

	enum COLLIDER_PARTS : _uint
	{
		PART_WEAPON = 0,
		PART_BODY,
		PART_END
	};

	typedef struct tagPlayerStat
	{
		_float fMaxHP;
		_float fHP;
		_float fAttackPower;
	}PLAYER_STAT;
#pragma endregion

public:
	typedef struct tagPlayerDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
		ANIMESH_SHADERPATH eShaderPath;
		_float fMaxHP;
		_float fHP;
		_float fAttackPower;
		_float3 vPos; // 초기 위치.
	}PLAYER_DESC;


#pragma region 초기 함수.
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	
	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta);

	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion



#pragma region 충돌 관리 
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

public:
	void Enable_Collider(COLLIDER_PARTS eColliderParts);
	void Disable_Collider(COLLIDER_PARTS eColliderParts);


private:
	// 데미지 
	void Take_Damage(_float fHp);
	ACTORDIR Calculate_Damage_Direction(class CMonster* pAttacker);

private:
	class CCollider* m_pColliderCom = { nullptr };
#pragma endregion

public:
	void Set_Camera(CCamera_Player* pCamera) { m_pPlayerCamera = pCamera; };


#pragma region PLAYER 함수 정의.
public:
	void HandleState(_float fTimeDelta);
	_vector  Calculate_Move_Direction(ACTORDIR eDir);


public:
	// 키 입력 상태 확인 함수들
	uint16_t Get_KeyInput() { return m_KeyInput; }

	_bool Is_KeyPressed(PLAYER_KEY ePlayerKey) const {
		return m_KeyInput & static_cast<uint16_t>(ePlayerKey);
	}

	_bool Is_KeyUp(PLAYER_KEY ePlayerKey) const {
		return (m_PrevKeyInput & static_cast<uint16_t>(ePlayerKey)) &&
			!(m_KeyInput & static_cast<uint16_t>(ePlayerKey));
	}

	_bool Is_KeyDown(PLAYER_KEY ePlayerKey) const {
		return !(m_PrevKeyInput & static_cast<uint16_t>(ePlayerKey)) &&
			(m_KeyInput & static_cast<uint16_t>(ePlayerKey));
	}

	// 움직임 키 확인
	_bool Is_MovementKeyPressed() const {
		return Is_KeyPressed(PLAYER_KEY::MOVE_FORWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_BACKWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_LEFT) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_RIGHT);
	}
	ACTORDIR Get_Direction() { return m_eCurrentDirection; }
	ACTORDIR Calculate_Direction();

	void Take_Damage(_float fDamage, class CMonster* pMonster);
#pragma endregion



#pragma region 0. 이동 관련 함수들
public:
	virtual void RootMotion_Translate(_fvector vTranslate);
	void Move_By_Camera_Direction_8Way(ACTORDIR eDir, _float fTimeDelta, _float fSpeed);
	void Move_Direction(_fvector vDirection, _float fTimeDelta);
	
	void Debug_CameraVectors();
	void Rotate_Player_To_Camera_Direction();
	// 현재 프레임 가져오기.
#pragma endregion

#pragma region 1. 락온
public:
	void Toggle_LockOn();                               // LockOn 토글
	void Search_LockOn_Target();                        // 타겟 검색
	void Set_LockOn_Target(CGameObject* pTarget);       // 타겟 설정
	void Clear_LockOn_Target();                         // 타겟 해제
	void Update_LockOn(_float fTimeDelta);              // LockOn 업데이트
	_bool Is_Valid_LockOn_Target(CGameObject* pTarget);  // 타겟 유효성 검사
	void Rotate_To_LockOn_Target(_float fTimeDelta, _float fSpeed = 3.0f); // 타겟 방향 회전
	_vector Calculate_LockOn_Direction() const;         // LockOn 방향 계산

	// Getter 함수들
	_bool Is_LockOn() const { return m_isLockOn; }
	CGameObject* Get_LockOn_Target() const { return m_pLockOn_Target; }

public:
	// LockOn 방향만 반환하는 함수 (공격 시 사용)
	_vector Get_LockOn_Attack_Direction() const;
	// 락온 상태인지 확인하기.
	_bool Has_LockOn_Target() const { return m_isLockOn && m_pLockOn_Target; }

private:
	// LockOn 시스템 관련 멤버변수들
	CGameObject* m_pLockOn_Target = { nullptr };         // 현재 LockOn 타겟 => 위치에 UI 띄우기.
	_bool m_isLockOn = { false };                        // LockOn 상태
	_float m_fLockOnRange = 15.0f;                       // LockOn 최대 거리
	_float m_fLockOnAngle = 120.0f;                      // LockOn 각도 (도 단위)
	_float m_fLockOnTimer = 0.0f;                        // LockOn 타이머
	_float m_fLockOnCheckInterval = 0.5f;                // LockOn 유효성 검사 주기
	_float m_fLockOnRotationSpeed = 3.0f;                // LockOn 시 회전 속도
	_bool m_bLockOnRotationEnabled = false;              // LockOn 시 자동 회전 여부
private:
	class m_pLockOn_Target* m_pTarget = { nullptr };
#pragma endregion


#pragma region 3. Animation 관리.
private:
	HRESULT InitializeAction_ToAnimationMap();

public:
	_uint Find_AnimationIndex(const _wstring& strAnimationTag);

private:
	unordered_map<_wstring, _uint> m_Action_AnimMap;

public:
	// Animation 교체.
	void Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange);
#pragma endregion




#pragma region 4. 플레이어 버프 관리
public:
	void RemoveBuff(uint32_t buffFlag, _bool removeTimer = false);
	const _bool AddBuff(_uint buffFlag, _float fCustomDuration = -1.f); // 적용이 실패할 수도 있음.
	const _bool IsBuffOnCooldown(_uint buffFlag);

public:
	// 현재 버프 소유 여부 확인
	_bool HasBuff(_uint buffFlag) const;
	_bool HasAnyBuff(_uint buffFlags) const;
	_bool HasAllBuffs(_uint buffFlags) const;

public:
	void Tick_BuffTimers(_float fTimeDelta);


private:
	HRESULT Initialize_BuffDurations();

private:
	uint32_t m_ActiveBuffs = { BUFF_NONE };
	unordered_map<uint32_t, _float> m_BuffTimers;           // 상태별 남은 시간
	unordered_map<uint32_t, _float> m_BuffDefault_Durations; // 상태별 기본 시간.

#pragma endregion

#pragma region 5. 플레이어 상태 스탯들.
private:
	PLAYER_STAT m_Stats = {};
	_bool m_IsInventoryDisplay = { false };

public:
	void Increase_Damage(_float fDamage);
	void Decrease_Damage(_float fDamage);
#pragma endregion

#pragma region 무기 TRAIL ON /OFF
public:
	void SetTrail_Visible(_bool bTrail);

#pragma endregion



#pragma region Player 기본 상태 값
private:
	// Load Model;
	class CLoad_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CPlayerWeapon* m_pPlayerWeapon = { nullptr };
	class CFsm* m_pFsmCom = { nullptr };
	class CCamera_Player* m_pPlayerCamera = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };
	_float m_fOffsetY = {};
	
#pragma endregion


#pragma region Key 입력 상태 값
private:
	uint16_t m_PrevKeyInput = {};
	uint16_t m_KeyInput = {};
	static const _uint m_KeyboardMappingsCount;
	static const pair<PLAYER_KEY, _ubyte> m_KeyboardMappings[];
	ACTORDIR m_eCurrentDirection = {};

private:
	void Update_KeyInput();
#pragma endregion

#pragma region STAT

private:
	// 스탯 관련
	_float m_fMaxHP = {} ;
	_float m_fCurrentHP = {};
	_float m_fAttackPower = {};
	_float m_fDetectionRange = {};
	_float m_fAttackRange = {};
	_float m_fMoveSpeed = {};
	_float m_fRotationSpeed = {};
#pragma endregion



#pragma region SHADER PATH
private:
	_uint m_iShaderPath = {};

#pragma endregion



private:
	HRESULT Ready_Components(PLAYER_DESC* pDesc);
	HRESULT Ready_Colliders(PLAYER_DESC* pDesc);
	HRESULT Ready_Navigations();
	HRESULT Ready_Fsm();
	void Register_CoolTime();
	HRESULT Bind_ShaderReosurces();
	HRESULT Ready_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;


#ifdef _DEBUG
private:
	void ImGui_Render(); // 디버그용 함수.
#endif // _DEBUG

};
NS_END

