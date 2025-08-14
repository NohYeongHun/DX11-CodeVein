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

    // 1 ~ 21은 커스텀사용
    enum BUFFe_FLAGS : _uint
    {
        BUFF_NONE = 0,
        
        BUFF_NAVIGATION_OFF =  1 << 20,
        BUFF_DETECT = 1 << 21,
        BUFF_ATTACK_TIME = 1 << 22,
        BUFF_HIT = 1 << 23,
        BUFF_DOWN = 1 << 24,
        BUFF_STUN = 1 << 25,
        BUFF_INVINCIBLE = 1 << 26, // 무적시간.
        BUFF_DEAD = 1 << 27,
        BUFF_CORPSE = 1 << 28, // 거의 마지막에만 사용할듯?
        BUFF_DISSOLVE = 1 << 29,
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
        _float3 vPos = { 0.f, 0.f, 0.f }; // 시작 위치.
    }MONSTER_DESC;

public:
    // 개선된 콜라이더 정보 구조체
    typedef struct tagMonsterColliderFrame
    {
        _float fStartRatio;
        _float fEndRatio;
        _uint iPartType;        // 어떤 Part 타입을 제어할 것인가?
        _bool bIsActive;
        
        tagMonsterColliderFrame() : fStartRatio(0.0f), fEndRatio(0.0f), iPartType(0), bIsActive(false) {}
        tagMonsterColliderFrame(_float start, _float end, _uint part) 
            : fStartRatio(start), fEndRatio(end), iPartType(part), bIsActive(false) {}
    }MONSTER_COLLIDER_FRAME;

    // 하나의 애니메이션에서 여러 Part, 여러 타이밍의 콜라이더 제어
    typedef struct tagMonsterColliderControl
    {
        vector<MONSTER_COLLIDER_FRAME> vecColliderFrames;  // 여러 콜라이더 구간들
        unordered_map<_uint, _uint> partCurrentIndex;      // Part별 현재 처리 중인 인덱스
        unordered_map<_uint, _bool> partProcessed;         // Part별 모든 처리 완료 여부
        
        tagMonsterColliderControl() {}
    }MONSTER_COLLIDER_CONTROL;

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
    virtual void Finalize_Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

#pragma endregion

#pragma region TRIGGER -> TO OBJECT_MANAGER
    virtual void OnMoved_ToObjectManager();
#pragma endregion


#pragma region 0. 몬스터는 충돌에 대한 상태제어를 할 수 있어야한다. => 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
    virtual void On_Collision_Enter(CGameObject* pOther) override;
    virtual void On_Collision_Stay(CGameObject* pOther) override;
    virtual void On_Collision_Exit(CGameObject* pOther) override;

    
public:
    // 무기 및 스킬과 충돌 시 받는 데미지 처리.
    virtual void Take_Damage(_float fDamage); 
    virtual void Collider_Part_Active(_uint iPartType, _bool bActive) {};
    virtual void Collider_All_Active(_bool bActive) {};

protected:
    class CCollider* m_pColliderCom = { nullptr };




protected:
    // 통합된 콜라이더 제어 시스템
    unordered_map<_uint, MONSTER_COLLIDER_CONTROL> m_ColliderControlMap; // 애니메이션별 콜라이더 제어
    unordered_map<_uint, _bool> m_PartPrevColliderState; // Part별 이전 콜라이더 상태

#pragma endregion

#pragma region Update AI
public:
    virtual void Update_AI(_float fTimeDelta) PURE;
#pragma endregion

#pragma region 통합 콜라이더 제어 시스템
public:
	// 콜라이더 프레임 추가 함수
	void Add_Collider_Frame(_uint iAnimationIndex, _float fStartRatio, _float fEndRatio, _uint iPartType);
	
	// 콜라이더 상태 제어 함수들
	void Handle_Collider_State();
	void Reset_Collider_ActiveInfo();
	
	// 편의 함수들 - 기존 Add_Attack_Frame 호환성 유지
	void Add_Attack_Frame(_uint iAnimationIndex, _float fStartRatio, _float fEndRatio) {
		Add_Collider_Frame(iAnimationIndex, fStartRatio, fEndRatio, 0); // 기본값은 0번 Part
	}
	
	// 기존 Handle_Attack_Collider_State 호환성 유지
	void Handle_Attack_Collider_State() { Handle_Collider_State(); }
	void Reset_Attack_ActiveInfo() { Reset_Collider_ActiveInfo(); }

#pragma endregion


#pragma region 1. 애니메이션 관리. => 몬스터는 내 애니메이션이 무엇인지 알아야한다.
public:
    virtual void Change_Animation_NonBlend(_uint iNextAnimIdx, _bool IsLoop = false);
    virtual void Change_Animation_Blend(_uint iNextAnimIdx, _bool IsLoop = false, _float fBlendDuration = 0.2f, _bool bScale = true, _bool bRotate = true, _bool bTranslate = true);

    void Change_Animation_Combo(_uint iAnimationIndex); // 연계공격 전용

    virtual void RootMotion_Translate(_fvector vTranslate);

    void Move_Direction(_fvector vDirection, _float fTimeDelta); // 이동용 함수. 


public:
    void Set_Animation_AddSpeed(_uint iAnimationIndex, _float fSpeed);
    _float Get_Animation_Speed(_uint iAnimationIndex);
    void Set_Animation_Speed(_uint iAnimationIndex, _float fSpeed);

public:
    virtual HRESULT InitializeAction_ToAnimationMap() PURE; // 필수적으로 애니메이션 초기화를 진행해야합니다. => 인스턴스화 될거라면.
    
public:
    _uint Get_CurrentAnimation() { return m_pModelCom->Get_CurrentAnimationIndex(); }
    _uint Find_AnimationIndex(const _wstring& strAnimationTag);
    const _bool Is_Animation_Finished();
    _float Get_CurrentAnimationRatio() { return m_pModelCom->Get_Current_Ratio(); }

public:
    void Set_RootMotionRotation(_bool IsRotate);
    void Set_RootMotionTranslate(_bool IsTranslate);

public:
    void Set_AnimationActivate();
    void Set_AnimationDeActivate();

    // 현재 위치에서 셀 위로 강제로 올리는법
    void Compute_OnCell();

        
protected:
    _wstring m_CurrentAction = { L"IDLE" };

protected:
    unordered_map<_wstring, _uint> m_Action_AnimMap;
#pragma endregion


#pragma region 2. 몬스터는 자신에게 필요한 수치 값이 존재한다. => Stat, 여러 상태(탐지 거리, 수치화값들.)
public:
    const MONSTER_STAT& Get_MonsterStat() { return m_MonsterStat; }
    const _float Get_DetectionRange() { return m_MonsterStat.fDetectionRange; }
    const _float Get_MinDetectionRange() { return m_fMinDetectionDistance; }
    const _float Get_AttackRange() { return m_MonsterStat.fAttackRange; }

public:
    virtual HRESULT Initialize_Stats() PURE; // 객체가 사용해야하는 수치값 정의하기.
    
public:
    // 수치값을 이용한 함수들.
    virtual const _bool Is_TargetAttackRange();
    virtual const _bool Is_TargetDetectionRange();

protected:
    MONSTER_STAT m_MonsterStat = {};
    _float m_fMinDetectionDistance = 5.f;
    _float m_fMinRotationDistance = 2.f;
    _bool m_bIsCurrentlyDetecting = false;  // 현재 탐지 중인지 상태
#pragma endregion


#pragma region 3. 몬스터는 자신이 어떤 버프를 소유할 수 있는지를 알아야합니다. => 그리고 그에 맞는 쿨타임도 알아야합니다.
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

public:
    virtual HRESULT Initialize_BuffDurations() PURE;

protected:
    /* 고민해봐야 될 점. => 버프의 진행시간은 있지만 해당 버프의 진행시간 보다 쿨타임이 긴 경우가 많음.*/
    uint32_t m_ActiveBuffs = { BUFF_NONE };
    unordered_map<uint32_t, _float> m_BuffTimers;           // 상태별 남은 시간
    unordered_map<uint32_t, _float> m_BuffDefault_Durations; // 상태별 기본 시간.

#pragma endregion


#pragma region 4. 기본적으로 몬스터는 생성하기 위한 컴포넌트 준비가 필요하다.
protected:
    virtual HRESULT Ready_Components(MONSTER_DESC* pDesc);
    virtual HRESULT Ready_Collider();
    virtual HRESULT Ready_Stats(MONSTER_DESC* pDesc);
#pragma endregion


#pragma region 5. 특수한 상태들을 제어하기 위한 함수들.
public:
    virtual void Rotate_ToTarget(_float fTimeDelta); // 플레이어를 보면서 회전한다.
    virtual void RotateTurn_ToTarget(); // 플레이어를 보면서 회전한다.
    virtual void RotateTurn_ToTargetYaw(_float fTimeDelta); // 플레이어를 보면서 회전하는데 Yaw로만 회전.
    virtual void RotateTurn_ToTargetYaw(); // 플레이어를 보면서 회전하는데 Yaw로만 회전.
    

    virtual const _bool IsRotateFinished(_float fRadian);

    virtual void NearCell_Translate();

public:
    /* 콜라이더 제어.*/
    virtual void Enable_Collider(_uint iType) PURE; 
    virtual void Disable_Collider(_uint iType) PURE;
    
    /* Reset 시 파츠 콜라이더 비활성화 */
    virtual void Reset_Part_Colliders();

    virtual void Dead_Action();
#pragma endregion
    
#pragma region 6. 체력 감소시 UI 연동
public:
    virtual void Increase_HpUI(_float fHp, _float fTime) {};
    virtual void Decrease_HpUI(_float fHp, _float fTime) {};

private:


#pragma endregion


#pragma region 7. 몬스터 삭제 처리.
public:
    virtual _bool Monster_Dead();

#pragma endregion


#pragma region 99. DEBUG 용도 함수.

#ifdef _DEBUG
public:
    virtual void Print_Position();
#endif // _DEBUG



#pragma endregion


#pragma region 0. 공통된 Initialize 값 => Monster들은 해당 정보들을 필수적으로 소유해야합니다.
public:
    class CPlayer* Get_Target() { return m_pTarget; } // 플레이어 포인터를 생성과 동시에 가지고 있고 위치를 탐지합니다.


protected:
    class CLoad_Model* m_pModelCom = { nullptr };
    class CShader* m_pShaderCom = { nullptr };
    class CPlayer* m_pTarget = { nullptr };
    LEVEL m_eCurLevel = { LEVEL::END };
    MONSTERTYPE m_eMonsterType = { MONSTERTYPE::END };

    _float m_fOffsetY = {};
    
#pragma endregion


public:
    virtual void Destroy() override;
    virtual void Free() override;
};

NS_END