// ===== Monster.h 수정된 부분 =====
#pragma once


/* 모든 몬스터의 부모가 되는 클래스*/
NS_BEGIN(Client)
class CMonster abstract : public CContainerObject
{
public:
    // 지금 뭘하고 있니? => 한번에 하나밖에 수행 못함. (Is a 관계 하나만 됨.)

    // 액션에 맞는 Animation 동기화가 필요함.
    
    // 너 지금 어떤 버프/디버프에 걸려있니?
    // Has 관계 => 가지고 있는지 여부를 판단합니다. (여러 개)
    enum BUFF_FLAGS : _uint
    {
        BUFF_NONE = 0,
        BUFF_DOWN = 1 << 0,
        BUFF_HIT = 1 << 1, 
        BUFF_STUN = 1 << 2, 
        BUFF_END
    };

public:
    typedef struct tagMonsterDesc : public CGameObject::GAMEOBJECT_DESC
    {
        class CPlayer* pPlayer = { nullptr };
        LEVEL eCurLevel;
        MONSTERTYPE eMonsterType;
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

    /* 기본 존재하는 함수들.*/
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    /* 충돌 처리 */
public:
    virtual void On_Collision_Enter(CGameObject* pOther) override;
    virtual void On_Collision_Stay(CGameObject* pOther) override;
    virtual void On_Collision_Exit(CGameObject* pOther) override;
#pragma endregion

#pragma region 애니메이션 관리.
public:
    virtual void Change_Animation_NonBlend(_uint iAnimIdx, _bool IsLoop = false);
    virtual void Change_Animation_Blend(const BLEND_DESC& blendDesc, _bool IsLoop);


public:
    virtual HRESULT InitializeAction_ToAnimationMap() PURE;
    
public:
    void Change_Action(_wstring strAction) { m_CurrentAction = strAction; }
    _wstring Current_Action() { return m_CurrentAction; }
    _uint Get_CurrentAnimation() { return m_Action_AnimMap[m_CurrentAction]; }

protected:
    _wstring m_CurrentAction = { L"IDLE" };

protected:
    unordered_map<_wstring, _uint> m_Action_AnimMap;
#pragma endregion


#pragma region 수치 값 확인하기.
public:
    const MONSTER_STAT& Get_MonsterStat() { return m_MonsterStat; }
    const _float Get_DetectionRange() { return m_fDetectionRange; }

protected:
    MONSTER_STAT m_MonsterStat = {};
    _float m_fDetectionRange = { };
#pragma endregion


#pragma region BUFF FLAG 관리.
public:
    void RemoveBuff(uint32_t expiredBuff);
    void AddBuff(_uint buffFlag, _float fCustomDuration = -1.f);
    _bool HasBuff(_uint buffFlag) const;
    _bool HasAnyBuff(_uint buffFlags) const;
    _bool HasAllBuffs(_uint buffFlags) const;  // ⭐ 필수

public:
    void Tick_Timers(_float fTimeDelta);

public:
    virtual HRESULT Initialize_BuffDurations();

protected:
    uint32_t m_ActiveBuffs = { BUFF_NONE };
    unordered_map<uint32_t, _float> m_BuffTimers;  // 상태별 남은 시간
    unordered_map<uint32_t, _float> m_BuffDefaultDurations; // 상태별 기본 시간.
#pragma endregion


#pragma region 컴포넌트 준비
protected:
    virtual HRESULT Ready_Components(MONSTER_DESC* pDesc);
    virtual HRESULT Ready_Collider();
    virtual HRESULT Ready_Stats(MONSTER_DESC* pDesc);
#pragma endregion

#pragma region 공통 Initialize 값
public:
    class CPlayer* Get_Target() { return m_pTarget; }


protected:
    class CLoad_Model* m_pModelCom = { nullptr };
    class CShader* m_pShaderCom = { nullptr };
    class CCollider* m_pColliderCom = { nullptr };
    class CPlayer* m_pTarget = { nullptr };
    LEVEL m_eCurLevel = { LEVEL::END };
    MONSTERTYPE m_eMonsterType = { MONSTERTYPE::END };
    
#pragma endregion


public:
    virtual void Free() override;
};

NS_END