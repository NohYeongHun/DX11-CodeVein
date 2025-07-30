#include "Monster.h"
// 생성자에서 초기화
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
    , m_eCurLevel(LEVEL::END)
    , m_eMonsterType(MONSTER_TYPE_NORMAL)
    , m_eCurrentDirection(MONSTERDIR::FRONT)  // ACTORDIR::FRONT → MONSTERDIR::FRONT
    , m_eCurrentState(MONSTER_IDLE)
    , m_fMaxHP(100.f)
    , m_fCurrentHP(100.f)
    , m_fAttackPower(20.f)
    , m_fDetectionRange(15.f)
    , m_fAttackRange(3.f)
    , m_fMoveSpeed(5.f)
    , m_fRotationSpeed(XMConvertToRadians(180.f))
{
}

CMonster::CMonster(const CMonster& Prototype)
    : CContainerObject(Prototype)
    , m_eCurLevel(Prototype.m_eCurLevel)
    , m_eMonsterType(Prototype.m_eMonsterType)
    , m_eCurrentDirection(Prototype.m_eCurrentDirection)  // ACTORDIR::FRONT → MONSTERDIR::FRONT
    , m_eCurrentState(Prototype.m_eCurrentState)
    , m_fMaxHP(Prototype.m_fMaxHP)
    , m_fCurrentHP(Prototype.m_fCurrentHP)
    , m_fAttackPower(Prototype.m_fAttackPower)
    , m_fDetectionRange(Prototype.m_fDetectionRange)
    , m_fAttackRange(Prototype.m_fAttackRange)
    , m_fMoveSpeed(Prototype.m_fMoveSpeed)
    , m_fRotationSpeed(Prototype.m_fRotationSpeed)
{

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

HRESULT CMonster::Initialize_Clone(void* pArg)
{
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

void CMonster::Take_Damage(_float fDamage, CGameObject* pAttacker)
{
}

void CMonster::Heal(_float fHealAmount)
{
}

_float CMonster::Get_Distance_To_Target() const
{
    return _float();
}

void CMonster::Enter_Combat(CGameObject* pTarget)
{
}

void CMonster::Exit_Combat()
{
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
}

void CMonster::Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed)
{
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
}

void CMonster::Update_Detection(_float fTimeDelta)
{
}

_bool CMonster::Can_See_Target() const
{
    return _bool();
}

_bool CMonster::Is_In_Attack_Range() const
{
    return _bool();
}

void CMonster::Handle_State(_float fTimeDelta)
{
}

void CMonster::Change_State(MONSTER_BASE_STATE eNewState)
{
}

void CMonster::Update_Animation(_float fTimeDelta)
{
}

void CMonster::Play_Animation(_uint iAnimIndex, _bool bLoop)
{
}

_bool CMonster::Is_Animation_Finished() const
{
    return _bool();
}

void CMonster::Update_BT_AI(_float fTimeDelta)
{
}

void CMonster::Update_FSM_AI(_float fTimeDelta)
{
}

_bool CMonster::Has_CooldownData(const _wstring& strKey) const
{
    return _bool();
}

_float CMonster::Get_CooldownData(const _wstring& strKey) const
{
    return _float();
}

void CMonster::Set_CooldownData(const _wstring& strKey, _float fTime)
{
}

void CMonster::Clear_CooldownData(const _wstring& strKey)
{
}

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
    Safe_Release(m_pBehaviourTreeCom);
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