// ===== Monster.h 수정된 부분 =====
#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

// 몬스터 방향 정의 (기존 ACTORDIR 대신)
enum class MONSTERDIR : _ubyte
{
    FRONT = 0,      // 앞 (북쪽)
    FRONT_RIGHT,    // 앞-오른쪽 (북동)
    RIGHT,          // 오른쪽 (동쪽)
    BACK_RIGHT,     // 뒤-오른쪽 (남동)
    BACK,           // 뒤 (남쪽)
    BACK_LEFT,      // 뒤-왼쪽 (남서)
    LEFT,           // 왼쪽 (서쪽)
    FRONT_LEFT,     // 앞-왼쪽 (북서)
    END
};

// 🎯 비트 플래그로 몬스터 상태 정의
enum MONSTER_STATE_FLAGS : uint32_t
{
    STATE_NONE = 0,
    STATE_DEAD = 1 << 0,        // 0x00000001
    STATE_HIT = 1 << 1,         // 0x00000002
    STATE_STRONG_HIT = 1 << 2,  // 0x00000004
    STATE_STUNNED = 1 << 3,     // 0x00000008
    STATE_KNOCKDOWN = 1 << 4,   // 0x00000010
    STATE_INVINCIBLE = 1 << 5,  // 0x00000020
    STATE_BURNING = 1 << 6,     // 0x00000040
    STATE_FROZEN = 1 << 7,      // 0x00000080
    STATE_POISONED = 1 << 8,    // 0x00000100
    STATE_CONFUSED = 1 << 9,    // 0x00000200
    STATE_BERSERKER = 1 << 10,  // 0x00000400
    STATE_SHIELDED = 1 << 11,   // 0x00000800
};

// 상태별 기본 지속시간 정의
struct StateDefaultDurations
{
    static constexpr _float HIT_DURATION = 0.6f;
    static constexpr _float STRONG_HIT_DURATION = 1.2f;
    static constexpr _float STUN_DURATION = 2.0f;
    static constexpr _float KNOCKDOWN_DURATION = 1.5f;
    static constexpr _float INVINCIBLE_DURATION = 1.0f;
    static constexpr _float BURNING_DURATION = 5.0f;
    static constexpr _float FROZEN_DURATION = 3.0f;
    static constexpr _float POISON_DURATION = 8.0f;
    static constexpr _float CONFUSION_DURATION = 4.0f;
    static constexpr _float BERSERKER_DURATION = 10.0f;
    static constexpr _float SHIELD_DURATION = 15.0f;
};

// 몬스터 타입 정의
enum MONSTER_TYPE : _int
{
    MONSTER_TYPE_BOSS = 0,
    MONSTER_TYPE_ELITE,
    MONSTER_TYPE_NORMAL,
    MONSTER_TYPE_WEAK,
    MONSTER_TYPE_END
};

// 몬스터 공통 상태 정의 (애니메이션/FSM용)
enum MONSTER_BASE_STATE : _int
{
    MONSTER_IDLE = 0,
    MONSTER_WALK,
    MONSTER_RUN,
    MONSTER_ATTACK,
    MONSTER_GUARD,
    MONSTER_DODGE,
    MONSTER_HURT,
    MONSTER_DEATH,
    MONSTER_STATE_BASE_END
};

class CMonster abstract : public CContainerObject
{
public:
    typedef struct tagMonsterDesc : public CGameObject::GAMEOBJECT_DESC
    {
        class CPlayer* pPlayer = { nullptr };
        LEVEL eCurLevel;
        MONSTER_TYPE eMonsterType;
        _float fMaxHP;
        _float fAttackPower;
        _float fDetectionRange;
        _float fAttackRange;
        _float fMoveSpeed;
        _float fRotationSpeed;
    }MONSTER_DESC;

    // 전방 선언
    class StateManager;

#pragma region 기본 함수들
protected:
    explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMonster(const CMonster& Prototype);
    virtual ~CMonster() = default;

public:
    virtual HRESULT Initialize_Prototype() PURE;
    virtual HRESULT Initialize_Clone(void* pArg) PURE;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void On_Collision_Enter(CGameObject* pOther) override;
    virtual void On_Collision_Stay(CGameObject* pOther) override;
    virtual void On_Collision_Exit(CGameObject* pOther) override;
#pragma endregion

#pragma region 상태 관리 시스템
public:
    // 상태 관리 클래스
    class StateManager
    {
    private:
        uint32_t m_stateFlags = STATE_NONE;
        std::unordered_map<uint32_t, _float> m_stateDurations;
        CMonster* m_pOwner;

    public:
        StateManager() : m_pOwner(nullptr) {} // 기본 생성자
        void Initialize(CMonster* pOwner) { m_pOwner = pOwner; } // 초기화 함수

        // 상태 설정/해제
        void SetState(uint32_t state, _float duration = -1.0f);
        void ClearState(uint32_t state);
        void ClearAllStates();

        // 상태 확인
        bool HasState(uint32_t state) const { return (m_stateFlags & state) != 0; }
        bool HasAnyState(uint32_t states) const { return (m_stateFlags & states) != 0; }
        bool HasAllStates(uint32_t states) const { return (m_stateFlags & states) == states; }
        uint32_t GetAllStates() const { return m_stateFlags; }

        // 시간 관리
        _float GetStateDuration(uint32_t state) const;
        void SetStateDuration(uint32_t state, _float duration);
        void Update(_float fTimeDelta);

    private:
        _float GetDefaultDuration(uint32_t state) const;
    };

protected:
    StateManager m_StateManager;

public:
    // 🎯 상태 확인 함수들 (BT 노드에서 사용)
    _bool Is_Dead() const { return m_StateManager.HasState(STATE_DEAD); }
    _bool Is_Hit() const { return m_StateManager.HasState(STATE_HIT); }
    _bool Is_StrongHit() const { return m_StateManager.HasState(STATE_STRONG_HIT); }
    _bool Is_Stunned() const { return m_StateManager.HasState(STATE_STUNNED); }
    _bool Is_Knockdown() const { return m_StateManager.HasState(STATE_KNOCKDOWN); }
    _bool Is_Invincible() const { return m_StateManager.HasState(STATE_INVINCIBLE); }
    _bool Is_Burning() const { return m_StateManager.HasState(STATE_BURNING); }
    _bool Is_Frozen() const { return m_StateManager.HasState(STATE_FROZEN); }
    _bool Is_Poisoned() const { return m_StateManager.HasState(STATE_POISONED); }
    _bool Is_Confused() const { return m_StateManager.HasState(STATE_CONFUSED); }
    _bool Is_Berserker() const { return m_StateManager.HasState(STATE_BERSERKER); }
    _bool Is_Shielded() const { return m_StateManager.HasState(STATE_SHIELDED); }

    // 🎭 복합 상태 확인 
    _bool Is_AnyHit() const { return m_StateManager.HasAnyState(STATE_HIT | STATE_STRONG_HIT); }
    _bool Is_Disabled() const { return m_StateManager.HasAnyState(STATE_STUNNED | STATE_KNOCKDOWN | STATE_FROZEN); }
    _bool Is_Buffed() const { return m_StateManager.HasAnyState(STATE_BERSERKER | STATE_SHIELDED | STATE_INVINCIBLE); }
    _bool Is_Debuffed() const { return m_StateManager.HasAnyState(STATE_BURNING | STATE_POISONED | STATE_CONFUSED); }

    // 📋 상태 설정 (외부에서 호출)
    void Set_Dead() { m_StateManager.SetState(STATE_DEAD); }
    void Set_Hit(_float duration = -1.0f) { m_StateManager.SetState(STATE_HIT, duration); }
    void Set_StrongHit(_float duration = -1.0f) { m_StateManager.SetState(STATE_STRONG_HIT, duration); }
    void Set_Stunned(_float duration = -1.0f) { m_StateManager.SetState(STATE_STUNNED, duration); }
    void Set_Knockdown(_float duration = -1.0f) { m_StateManager.SetState(STATE_KNOCKDOWN, duration); }
    void Set_Invincible(_float duration = -1.0f) { m_StateManager.SetState(STATE_INVINCIBLE, duration); }
    void Set_Burning(_float duration = -1.0f) { m_StateManager.SetState(STATE_BURNING, duration); }
    void Set_Frozen(_float duration = -1.0f) { m_StateManager.SetState(STATE_FROZEN, duration); }

    // 상태 해제
    void Clear_Hit() { m_StateManager.ClearState(STATE_HIT); }
    void Clear_StrongHit() { m_StateManager.ClearState(STATE_STRONG_HIT); }
    void Clear_AllStates() { m_StateManager.ClearAllStates(); }

    // 📊 상태 정보
    _float Get_HitDuration() const { return m_StateManager.GetStateDuration(STATE_HIT); }
    _float Get_StunDuration() const { return m_StateManager.GetStateDuration(STATE_STUNNED); }

    // 🎬 상태 변화 콜백 (하위 클래스에서 오버라이드)
    virtual void OnStateEnter(uint32_t state) {}
    virtual void OnStateExit(uint32_t state) {}
#pragma endregion

#pragma region 공통 인터페이스 함수들
public:
    // Getter/Setter - MONSTERDIR 사용
    MONSTERDIR Get_Direction() const { return m_eCurrentDirection; }
    void Set_Direction(MONSTERDIR eDir) { m_eCurrentDirection = eDir; }

    _float Get_HP() const { return m_fCurrentHP; }
    _float Get_MaxHP() const { return m_fMaxHP; }
    _float Get_HPRatio() const { return m_fMaxHP > 0.f ? m_fCurrentHP / m_fMaxHP : 0.f; }
    MONSTER_TYPE Get_MonsterType() const { return m_eMonsterType; }

    // 타겟 관련
    void Set_Target(class CPlayer* pTarget) { m_pTarget = pTarget; }
    class CPlayer* Get_Target() const { return m_pTarget; }
    _float Get_Distance_To_Target() const;

    // BT 노드에서 사용할 Getter들
    _float Get_AttackPower() const { return m_fAttackPower; }
    _float Get_AttackRange() const { return m_fAttackRange; }
    _float Get_MoveSpeed() const { return m_fMoveSpeed; }
    _float Get_RotationSpeed() const { return m_fRotationSpeed; }
    _float Get_DetectionRange() const { return m_fDetectionRange; }
    MONSTER_BASE_STATE Get_CurrentState() const { return m_eCurrentState; }
#pragma endregion

#pragma region 상태 제어
public:
    virtual void Take_Damage(_float fDamage, class CPlayer* pAttacker = nullptr);
    virtual void Heal(_float fHealAmount);

    // 전투 상태 관리
    virtual void Enter_Combat(CPlayer* pTarget = nullptr);
    virtual void Exit_Combat();
#pragma endregion

#pragma region 공통 Condition Check
public:
    _bool IsTargetInRange();
#pragma endregion

#pragma region 공통 행동 함수들
public:
    // 이동 관련 - MONSTERDIR 사용
    virtual void Move_By_Direction(MONSTERDIR eDir, _float fTimeDelta, _float fSpeed);
    virtual void Move_To_Target(_float fTimeDelta, _float fSpeed);
    // 기존 함수는 유지하고 새로운 부드러운 회전 함수 추가
    virtual void Smooth_Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed);
    virtual void Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed);
    virtual void Look_At_Direction(MONSTERDIR eDir);

    // 방향 계산 헬퍼 함수들
    _vector Get_Direction_Vector(MONSTERDIR eDir) const;
    MONSTERDIR Vector_To_Direction(_vector vDirection) const;
    _float Get_Direction_Angle(MONSTERDIR eDir) const;

    // AI 관련
    virtual void Update_AI(_float fTimeDelta);
    virtual void Update_Detection(_float fTimeDelta);
    //virtual _bool Can_See_Target() const;
    virtual _bool Is_In_Attack_Range() const;

    // 상태 관리
    virtual void Handle_State(_float fTimeDelta);
    virtual void Change_State(MONSTER_BASE_STATE eNewState);

    // 애니메이션 관리
    virtual void Update_Animation(_float fTimeDelta);
    virtual void Chanage_Animation(_uint iAnimIndex, _bool bLoop = false);
    
    virtual _bool Is_Animation_Finished() const;

protected:
    // AI 선택적 사용
    virtual void Update_BT_AI(_float fTimeDelta);
    virtual void Update_FSM_AI(_float fTimeDelta);
#pragma endregion

#pragma region 컴포넌트 준비
protected:
    virtual HRESULT Ready_Components(MONSTER_DESC* pDesc);
    virtual HRESULT Ready_Collider();
    virtual HRESULT Ready_Stats(MONSTER_DESC* pDesc);
#pragma endregion

protected:
    // 공통 컴포넌트들
    class CLoad_Model* m_pModelCom = { nullptr };
    class CShader* m_pShaderCom = { nullptr };
    class CCollider* m_pColliderCom = { nullptr };

    // 공통 데이터
    LEVEL m_eCurLevel;
    MONSTER_TYPE m_eMonsterType;
    MONSTERDIR m_eCurrentDirection;
    MONSTER_BASE_STATE m_eCurrentState;

    // 스탯 관련
    _float m_fMaxHP;
    _float m_fCurrentHP;
    _float m_fAttackPower;
    _float m_fDetectionRange;
    _float m_fAttackRange;
    _float m_fMoveSpeed;
    _float m_fRotationSpeed;

    // 타겟 및 전투
    class CPlayer* m_pTarget = { nullptr };
    _bool m_bInCombat = { false };
    _float m_fCombatTimer = { 0.f };
    _float m_fLastAttackTime = { 0.f };

    // AI 선택
    _bool m_bUseBehaviourTree = { false };

    // 애니메이션
    _uint m_iCurrentAnimIndex = { 0 };
    _bool m_bAnimationLoop = { true };

    // 타이머들
    _float m_fDetectionTimer = { 0.f };
    _float m_fStateTimer = { 0.f };

    // BT 쿨다운 데이터
    std::unordered_map<std::wstring, _float> m_CooldownData;

    // 게임 시간
    static _float s_fGameTime;

public:
    virtual void Free() override;
};

NS_END