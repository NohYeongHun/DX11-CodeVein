

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

// 몬스터 공통 상태 정의
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

// 몬스터 타입 정의
enum MONSTER_TYPE : _int
{
    MONSTER_TYPE_BOSS = 0,
    MONSTER_TYPE_ELITE,
    MONSTER_TYPE_NORMAL,
    MONSTER_TYPE_WEAK,
    MONSTER_TYPE_END
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

#pragma region 기본 함수들
protected:
    explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMonster(const CMonster& Prototype);
    virtual ~CMonster() = default;

public:
    // 가상 함수들 - 하위 클래스에서 구현
    virtual HRESULT Initialize_Prototype() = 0;
    virtual HRESULT Initialize_Clone(void* pArg) = 0;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    // 충돌 처리
    virtual void On_Collision_Enter(CGameObject* pOther) override;
    virtual void On_Collision_Stay(CGameObject* pOther) override;
    virtual void On_Collision_Exit(CGameObject* pOther) override;
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
    _bool Is_Dead() const { return m_fCurrentHP <= 0.f; }
    _bool Is_In_Combat() const { return m_bInCombat; }

    // 데미지 처리
    virtual void Take_Damage(_float fDamage, class CPlayer* pAttacker = nullptr);
    virtual void Heal(_float fHealAmount);

    // 타겟 관련
    void Set_Target(class CPlayer* pTarget) { m_pTarget = pTarget; }
    class CPlayer* Get_Target() const { return m_pTarget; }
    _float Get_Distance_To_Target() const;

    // 전투 상태 관리
    virtual void Enter_Combat(CPlayer* pTarget = nullptr);
    virtual void Exit_Combat();

    // BT 노드에서 사용할 Getter들
    _float Get_AttackPower() const { return m_fAttackPower; }
    _float Get_AttackRange() const { return m_fAttackRange; }
    _float Get_MoveSpeed() const { return m_fMoveSpeed; }
    _float Get_RotationSpeed() const { return m_fRotationSpeed; }
    _float Get_DetectionRange() const { return m_fDetectionRange;  }
    MONSTER_BASE_STATE Get_CurrentState() const { return m_eCurrentState; }
#pragma endregion

#pragma region 공통 Conidtion Check
    _bool IsTargetInRange();
#pragma endregion


#pragma region 공통 행동 함수들
public:
    // 이동 관련 - MONSTERDIR 사용
    virtual void Move_By_Direction(MONSTERDIR eDir, _float fTimeDelta, _float fSpeed);
    virtual void Move_To_Target(_float fTimeDelta, _float fSpeed);
    virtual void Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed);
    virtual void Look_At_Direction(MONSTERDIR eDir);

    // 방향 계산 헬퍼 함수들
    _vector Get_Direction_Vector(MONSTERDIR eDir) const;
    MONSTERDIR Vector_To_Direction(_vector vDirection) const;
    _float Get_Direction_Angle(MONSTERDIR eDir) const;

    // AI 관련
    virtual void Update_AI(_float fTimeDelta);
    virtual void Update_Detection(_float fTimeDelta);
    virtual _bool Can_See_Target() const;
    virtual _bool Is_In_Attack_Range() const;

    // 상태 관리
    virtual void Handle_State(_float fTimeDelta);
    virtual void Change_State(MONSTER_BASE_STATE eNewState);

    // 애니메이션 관리
    virtual void Update_Animation(_float fTimeDelta);
    virtual void Play_Animation(_uint iAnimIndex, _bool bLoop = true);
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
    MONSTERDIR m_eCurrentDirection;  // ACTORDIR → MONSTERDIR 변경
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
    unordered_map<_wstring, _float> m_CooldownData;

    // 게임 시간
    static _float s_fGameTime;

public:
    virtual void Free() override;
};

NS_END