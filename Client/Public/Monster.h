#pragma once
#include "ContainerObject.h"

NS_BEGIN(Client)
// 몬스터 공통 상태 정의
enum MONSTER_BASE_STATE : _uint
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
enum MONSTER_TYPE : _uint
{
    MONSTER_TYPE_BOSS = 0,
    MONSTER_TYPE_ELITE,
    MONSTER_TYPE_NORMAL,
    MONSTER_TYPE_WEAK,
    MONSTER_TYPE_END
};

typedef struct tagMonsterStat
{
    _float fMaxHP;
    _float fCurrentHP;
    _float fAttackPower;
    _float fDetectionRange;
    _float fAttackRange;
    _float fMoveSpeed;
    _float fRotationSpeed;
}MONSTER_STAT;

class CMonster final : public CContainerObject
{
public:
    typedef struct tagMonsterDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel;            // 레벨
        MONSTER_TYPE eMonsterType;  // 몬스터 타입
        MONSTER_STAT eStats; // 스탯
    }MONSTER_DESC;

#pragma region 기본 
protected:
    explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMonster(const CMonster& Prototype);
    virtual ~CMonster() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void On_Collision_Enter(CGameObject* pOther) override;
    virtual void On_Collision_Stay(CGameObject* pOther) override;
    virtual void On_Collision_Exit(CGameObject* pOther) override;


#pragma endregion


#pragma region 공통 인터페이스 함수들
public:
    // Getter/Setter
    ACTORDIR Get_Direction() const { return m_eCurrentDirection; }
    void Set_Direction(ACTORDIR eDir) { m_eCurrentDirection = eDir; }

    _float Get_HP() const { return m_eStats.fCurrentHP; }
    _float Get_MaxHP() const { return m_eStats.fMaxHP; }
    _float Get_HPRatio() const { return m_eStats.fMaxHP > 0.f ? m_eStats.fCurrentHP / m_eStats.fMaxHP : 0.f; }

    MONSTER_TYPE Get_MonsterType() const { return m_eMonsterType; }
    _bool Is_Dead() const { return m_eStats.fCurrentHP <= 0.f; }
    _bool Is_In_Combat() const { return m_bInCombat; }

    // 데미지 처리
    virtual void Take_Damage(_float fDamage, CGameObject* pAttacker = nullptr);
    virtual void Heal(_float fHealAmount);

    // 타겟 관련
    void Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
    CGameObject* Get_Target() const { return m_pTarget; }
    _float Get_Distance_To_Target() const;

    // 전투 상태 관리
    virtual void Enter_Combat(CGameObject* pTarget = nullptr);
    virtual void Exit_Combat();

protected:
    // 공통 컴포넌트들
    class CLoad_Model* m_pModelCom = { nullptr };
    class CShader* m_pShaderCom = { nullptr };
    class CFsm* m_pFsmCom = { nullptr };
    class CCollider* m_pColliderCom = { nullptr };

    // 공통 데이터
    LEVEL m_eCurLevel;
    MONSTER_TYPE m_eMonsterType;
    ACTORDIR m_eCurrentDirection;
    MONSTER_BASE_STATE m_eCurrentState;

    // 스탯
    MONSTER_STAT m_eStats = {};

    // 타겟 및 전투
    CGameObject* m_pTarget = { nullptr };
    _bool m_bInCombat = { false };
    _float m_fCombatTimer = { 0.f };
    _float m_fLastAttackTime = { 0.f };

    // 애니메이션
    _uint m_iCurrentAnimIndex = { 0 };
    _bool m_bAnimationLoop = { true };

    // 타이머들
    _float m_fDetectionTimer = { 0.f };
    _float m_fStateTimer = { 0.f };

#pragma endregion


public:
    virtual void Free() override;



};
NS_END

