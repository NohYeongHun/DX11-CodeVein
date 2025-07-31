#include "Monster.h"

// 생성자에서 초기화
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
    , m_eCurLevel(LEVEL::END)
    , m_eMonsterType(MONSTER_TYPE_NORMAL)
    , m_eCurrentDirection(MONSTERDIR::FRONT)
    , m_eCurrentState(MONSTER_IDLE)  // 애니메이션 상태 추가
    , m_fMaxHP(100.f)
    , m_fCurrentHP(100.f)
    , m_fAttackPower(20.f)
    , m_fDetectionRange(15.f)
    , m_fAttackRange(3.f)
    , m_fMoveSpeed(5.f)
    , m_fRotationSpeed(XMConvertToRadians(180.f))
{
    // ✅ 안전한 StateManager 초기화
    m_StateManager.Initialize(this);
}

CMonster::CMonster(const CMonster& Prototype)
    : CContainerObject(Prototype)
    , m_eCurLevel(Prototype.m_eCurLevel)
    , m_eMonsterType(Prototype.m_eMonsterType)
    , m_eCurrentDirection(Prototype.m_eCurrentDirection)
    , m_eCurrentState(Prototype.m_eCurrentState)
    , m_fMaxHP(Prototype.m_fMaxHP)
    , m_fCurrentHP(Prototype.m_fCurrentHP)
    , m_fAttackPower(Prototype.m_fAttackPower)
    , m_fDetectionRange(Prototype.m_fDetectionRange)
    , m_fAttackRange(Prototype.m_fAttackRange)
    , m_fMoveSpeed(Prototype.m_fMoveSpeed)
    , m_fRotationSpeed(Prototype.m_fRotationSpeed)
{
    // ✅ 복사 생성자에서도 안전한 초기화
    m_StateManager.Initialize(this);
}

HRESULT CMonster::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
    {
        CRASH("Failed Prototype");
        return E_FAIL;
    }
    return S_OK;
}

/* 몬스터 정보 초기화.*/
HRESULT CMonster::Initialize_Clone(void* pArg)
{
    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;
    m_pTarget = pDesc->pPlayer;

    if (FAILED(__super::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone");
        return E_FAIL;
    }

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
    // ✅ StateManager 업데이트 (자동 상태 관리)
    //m_StateManager.Update(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CMonster::Render()
{
    return S_OK;
}

void CMonster::On_Collision_Enter(CGameObject* pOther)
{
}

void CMonster::On_Collision_Stay(CGameObject* pOther)
{
}

void CMonster::On_Collision_Exit(CGameObject* pOther)
{
}

void CMonster::Take_Damage(_float fDamage, CPlayer* pAttacker)
{
    // 무적 상태면 데미지 무시
    if (Is_Invincible())
        return;

    // 데미지 적용
    m_fCurrentHP -= fDamage;
    if (m_fCurrentHP <= 0.f)
    {
        m_fCurrentHP = 0.f;
        Set_Dead(); // 죽음 상태 설정
    }
}

void CMonster::Heal(_float fHealAmount)
{
    m_fCurrentHP += fHealAmount;
    if (m_fCurrentHP > m_fMaxHP)
        m_fCurrentHP = m_fMaxHP;
}

_float CMonster::Get_Distance_To_Target() const
{
    if (!m_pTarget)
        return 0.0f;

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    return XMVectorGetX(XMVector3Length(vTargetPos - vPos));
}

void CMonster::Enter_Combat(CPlayer* pTarget)
{
    m_bInCombat = true;
    if (pTarget)
        m_pTarget = pTarget;
}

void CMonster::Exit_Combat()
{
    m_bInCombat = false;
}

_bool CMonster::IsTargetInRange()
{
    if (!m_pTarget)
        return false;

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPos));

    return fDistance <= m_fDetectionRange;
}

// MONSTERDIR을 사용한 이동 함수
void CMonster::Move_By_Direction(MONSTERDIR eDir, _float fTimeDelta, _float fSpeed)
{
    _vector vMoveDir = Get_Direction_Vector(eDir);
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    vPos += vMoveDir * fSpeed * fTimeDelta;
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
    m_eCurrentDirection = eDir;
}

void CMonster::Move_To_Target(_float fTimeDelta, _float fSpeed)
{
    if (!m_pTarget)
        return;

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vDirection = XMVector3Normalize(vTargetPos - vPos);

    // 타겟 방향으로 이동
    vPos += vDirection * fSpeed * fTimeDelta;
    m_pTransformCom->Set_State(STATE::POSITION, vPos);

    // 방향 업데이트
    m_eCurrentDirection = Vector_To_Direction(vDirection);
}

void CMonster::Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed)
{
    if (!m_pTarget)
        return;

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vDirection = XMVector3Normalize(vTargetPos - vPos);

    // 타겟을 향해 회전
    _float3 vLookAt;
    XMStoreFloat3(&vLookAt, vPos + vDirection);
    m_pTransformCom->LookAt(vLookAt);
}

// 방향 벡터 계산 헬퍼 함수
_vector CMonster::Get_Direction_Vector(MONSTERDIR eDir) const
{
    switch (eDir)
    {
    case MONSTERDIR::FRONT:
        return XMVectorSet(0.f, 0.f, 1.f, 0.f);
    case MONSTERDIR::FRONT_RIGHT:
        return XMVector3Normalize(XMVectorSet(1.f, 0.f, 1.f, 0.f));
    case MONSTERDIR::RIGHT:
        return XMVectorSet(1.f, 0.f, 0.f, 0.f);
    case MONSTERDIR::BACK_RIGHT:
        return XMVector3Normalize(XMVectorSet(1.f, 0.f, -1.f, 0.f));
    case MONSTERDIR::BACK:
        return XMVectorSet(0.f, 0.f, -1.f, 0.f);
    case MONSTERDIR::BACK_LEFT:
        return XMVector3Normalize(XMVectorSet(-1.f, 0.f, -1.f, 0.f));
    case MONSTERDIR::LEFT:
        return XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    case MONSTERDIR::FRONT_LEFT:
        return XMVector3Normalize(XMVectorSet(-1.f, 0.f, 1.f, 0.f));
    default:
        return XMVectorZero();
    }
}

// 벡터를 방향으로 변환하는 함수
MONSTERDIR CMonster::Vector_To_Direction(_vector vDirection) const
{
    vDirection = XMVector3Normalize(vDirection);
    _float fX = XMVectorGetX(vDirection);
    _float fZ = XMVectorGetZ(vDirection);

    // 각도 계산 (라디안)
    _float fAngle = atan2f(fX, fZ);
    if (fAngle < 0.f)
        fAngle += XM_2PI;

    // 8방향으로 분할 (각 방향당 45도 = π/4)
    _float fSectorAngle = XM_2PI / 8.f;
    _int iSector = static_cast<_int>((fAngle + fSectorAngle * 0.5f) / fSectorAngle) % 8;

    return static_cast<MONSTERDIR>(iSector);
}

// 방향에 따른 각도 반환 (도 단위)
_float CMonster::Get_Direction_Angle(MONSTERDIR eDir) const
{
    switch (eDir)
    {
    case MONSTERDIR::FRONT:         return 0.f;
    case MONSTERDIR::FRONT_RIGHT:   return 45.f;
    case MONSTERDIR::RIGHT:         return 90.f;
    case MONSTERDIR::BACK_RIGHT:    return 135.f;
    case MONSTERDIR::BACK:          return 180.f;
    case MONSTERDIR::BACK_LEFT:     return 225.f;
    case MONSTERDIR::LEFT:          return 270.f;
    case MONSTERDIR::FRONT_LEFT:    return 315.f;
    default:                        return 0.f;
    }
}

void CMonster::Update_AI(_float fTimeDelta)
{
    if (m_bUseBehaviourTree)
        Update_BT_AI(fTimeDelta);
    else
        Update_FSM_AI(fTimeDelta);
}

void CMonster::Update_Detection(_float fTimeDelta)
{
    // AI 탐지 로직 구현
}


_bool CMonster::Is_In_Attack_Range() const
{
    if (!m_pTarget)
        return false;

    _float fDistance = Get_Distance_To_Target();
    return fDistance <= m_fAttackRange;
}

void CMonster::Handle_State(_float fTimeDelta)
{
    // FSM 상태 처리 (필요시 구현)
}

void CMonster::Change_State(MONSTER_BASE_STATE eNewState)
{
    m_eCurrentState = eNewState;
}

void CMonster::Update_Animation(_float fTimeDelta)
{
    // 애니메이션 업데이트 로직
}

void CMonster::Chanage_Animation(_uint iAnimIndex, _bool bLoop)
{
    if (m_pModelCom)
        m_pModelCom->Set_Animation(iAnimIndex, bLoop);

    m_iCurrentAnimIndex = iAnimIndex;
    m_bAnimationLoop = bLoop;
}

_bool CMonster::Is_Animation_Finished() const
{
    if (m_pModelCom)
        return m_pModelCom->Is_Finished();
    return true;
}

void CMonster::Update_BT_AI(_float fTimeDelta)
{
    // BehaviorTree AI 업데이트 (하위 클래스에서 구현)
}

void CMonster::Update_FSM_AI(_float fTimeDelta)
{
    // FSM AI 업데이트 (하위 클래스에서 구현)
}

#pragma region StateManager 구현
void CMonster::StateManager::SetState(uint32_t state, _float duration)
{
    if (!m_pOwner)
        return;

    m_stateFlags |= state;

    // 지속시간 설정 (기본값 사용 또는 커스텀)
    if (duration < 0.0f)
        duration = GetDefaultDuration(state);

    if (duration > 0.0f)
        m_stateDurations[state] = duration;

    // 상태 시작 콜백
    m_pOwner->OnStateEnter(state);
}

void CMonster::StateManager::ClearState(uint32_t state)
{
    if (!m_pOwner)
        return;

}


void CMonster::StateManager::ClearAllStates()
{
    if (!m_pOwner)
        return;

    for (auto& pair : m_stateDurations)
    {
        m_pOwner->OnStateExit(pair.first);
    }
    m_stateFlags = STATE_NONE;
    m_stateDurations.clear();
}

_float CMonster::StateManager::GetStateDuration(uint32_t state) const
{
    auto it = m_stateDurations.find(state);
    return (it != m_stateDurations.end()) ? it->second : 0.0f;
}

void CMonster::StateManager::SetStateDuration(uint32_t state, _float duration)
{
    if (HasState(state))
        m_stateDurations[state] = duration;
}

void CMonster::StateManager::Update(_float fTimeDelta)
{
    std::vector<uint32_t> expiredStates;

    for (auto& pair : m_stateDurations)
    {
        pair.second -= fTimeDelta;
        if (pair.second <= 0.0f)
        {
            expiredStates.push_back(pair.first);
        }
    }

    // 만료된 상태들 제거
    for (uint32_t state : expiredStates)
    {
        ClearState(state);
    }
}

_float CMonster::StateManager::GetDefaultDuration(uint32_t state) const
{
    switch (state)
    {
    case STATE_HIT: return StateDefaultDurations::HIT_DURATION;
    case STATE_STRONG_HIT: return StateDefaultDurations::STRONG_HIT_DURATION;
    case STATE_STUNNED: return StateDefaultDurations::STUN_DURATION;
    case STATE_KNOCKDOWN: return StateDefaultDurations::KNOCKDOWN_DURATION;
    case STATE_INVINCIBLE: return StateDefaultDurations::INVINCIBLE_DURATION;
    case STATE_BURNING: return StateDefaultDurations::BURNING_DURATION;
    case STATE_FROZEN: return StateDefaultDurations::FROZEN_DURATION;
    case STATE_POISONED: return StateDefaultDurations::POISON_DURATION;
    case STATE_CONFUSED: return StateDefaultDurations::CONFUSION_DURATION;
    case STATE_BERSERKER: return StateDefaultDurations::BERSERKER_DURATION;
    case STATE_SHIELDED: return StateDefaultDurations::SHIELD_DURATION;
    default: return 0.0f;
    }
}
#pragma endregion

#pragma region 필수 컴포넌트 준비
HRESULT CMonster::Ready_Components(MONSTER_DESC* pDesc)
{
    return S_OK;
}

HRESULT CMonster::Ready_Collider()
{
    return S_OK;
}

HRESULT CMonster::Ready_Stats(MONSTER_DESC* pDesc)
{
    return S_OK;
}
#pragma endregion

void CMonster::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}

// 특정 방향을 바라보기
void CMonster::Look_At_Direction(MONSTERDIR eDir)
{
    _float fAngle = XMConvertToRadians(Get_Direction_Angle(eDir));
    _vector vForward = XMVectorSet(sinf(fAngle), 0.f, cosf(fAngle), 0.f);

    _vector vCurrentPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = vCurrentPos + vForward;

    _float3 vPos;
    XMStoreFloat3(&vPos, vTargetPos);
    m_pTransformCom->LookAt(vPos);
    m_eCurrentDirection = eDir;
}

// ===== 디버깅용 함수 =====
const wchar_t* MonsterDirToString(MONSTERDIR eDir)
{
    switch (eDir)
    {
    case MONSTERDIR::FRONT:         return L"FRONT";
    case MONSTERDIR::FRONT_RIGHT:   return L"FRONT_RIGHT";
    case MONSTERDIR::RIGHT:         return L"RIGHT";
    case MONSTERDIR::BACK_RIGHT:    return L"BACK_RIGHT";
    case MONSTERDIR::BACK:          return L"BACK";
    case MONSTERDIR::BACK_LEFT:     return L"BACK_LEFT";
    case MONSTERDIR::LEFT:          return L"LEFT";
    case MONSTERDIR::FRONT_LEFT:    return L"FRONT_LEFT";
    default:                        return L"UNKNOWN";
    }
}