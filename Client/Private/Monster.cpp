#include "Monster.h"


// 생성자에서 초기화
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& Prototype)
    : CContainerObject(Prototype)
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

/* 몬스터 정보 초기화.*/
HRESULT CMonster::Initialize_Clone(void* pArg)
{
    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;
    m_pTarget = pDesc->pPlayer;
    m_eMonsterType = pDesc->eMonsterType;

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


#pragma region 애니메이션 상태 변경.
void CMonster::Change_Animation_NonBlend(_uint iNextAnimIdx, _bool IsLoop)
{
    m_pModelCom->Set_Animation(iNextAnimIdx, IsLoop);
}

void CMonster::Change_Animation_Blend(_uint iNextAnimIdx, _bool IsLoop, _float fBlendDuration, _bool bScale, _bool bRotate, _bool bTranslate)
{
    /* 블렌드 정보를 먼저 설정하게 하기.*/
    m_pModelCom->Set_BlendInfo(iNextAnimIdx
        , fBlendDuration, bScale, bRotate
        , bTranslate);
    /* 애니메이션 변경. */
    m_pModelCom->Set_Animation(iNextAnimIdx, IsLoop);
}

/* 애니메이션 인덱스.*/
_uint CMonster::Find_AnimationIndex(const _wstring& strAnimationTag)
{
    auto iter = m_Action_AnimMap.find(strAnimationTag);

    // 찾는 애니메이션이 없는 경우
    if (iter == m_Action_AnimMap.end())
    {
        CRASH("Failed Find Animation"); 
        return 0;
    }

    return iter->second;
}

/* 현재 애니메이션이 끝났냐? */
const _bool CMonster::Is_Animation_Finished()
{
    return m_pModelCom->Is_Finished();
}



#pragma endregion


#pragma region BUFF Flag 관리

// Timer가 종료되거나 특정 시점에는 BuffFlag를 해제합니다.
void CMonster::RemoveBuff(uint32_t buffFlag)
{
    m_ActiveBuffs &= ~buffFlag;
    //m_BuffTimers.erase(buffFlag);
}
const _bool CMonster::AddBuff(_uint buffFlag, _float fCustomDuration)
{
    if (IsBuffOnCooldown(buffFlag))
        return false;

    m_ActiveBuffs |= buffFlag;

    // 시간을 지정한 경우에만 해당 시간으로 설정해줍니다. 
    m_BuffTimers[buffFlag] = fCustomDuration > 0.f ? fCustomDuration
        : m_BuffDefault_Durations[buffFlag];

    return true;
}

/* 현재 쿨타임인지 확인해주는 함수. */
const _bool CMonster::IsBuffOnCooldown(_uint buffFlag)
{
    auto iter = m_BuffTimers.find(buffFlag);
    return (iter != m_BuffTimers.end() && iter->second > 0.f);
}

// 하나만 확인 가능.
_bool CMonster::HasBuff(_uint buffFlag) const
{
    return (m_ActiveBuffs & buffFlag) != 0;
}

// 이 중에 아무거나 있으면 True | OR
_bool CMonster::HasAnyBuff(_uint buffFlags) const
{
    return (m_ActiveBuffs & buffFlags) != 0;
    }

// 다 가지고 있으면 True | And
_bool CMonster::HasAllBuffs(_uint buffFlags) const
{
    return (m_ActiveBuffs & buffFlags) == buffFlags;
}


void CMonster::Tick_BuffTimers(_float fTimeDelta)
{
    // 만료된 버프/디버프 들.
    std::vector<_uint> expiredBuffs;

    // 모든 타이머를 틱(감소)시킴
    for (auto& pair : m_BuffTimers)
    {
        uint32_t buffFlag = pair.first;
        _float& timer = pair.second;
        timer -= fTimeDelta;  

        if (timer <= 0.0f)
            expiredBuffs.push_back(buffFlag);
    }

    // 만료된 버프들 제거
    for (uint32_t expiredBuff : expiredBuffs)
        RemoveBuff(expiredBuff);

}

#pragma endregion





//void CMonster::Smooth_Rotate_To_Target(_float fTimeDelta, _float fRotationSpeed)
//{
//    if (!m_pTarget)
//        return;
//
//    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
//    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
//    _vector vDirection = XMVector3Normalize(vTargetPos - vPos);
//
//    // Y축 제거 (지면에서의 회전만)
//    vDirection = XMVectorSetY(vDirection, 0.f);
//    if (XMVector3Length(vDirection).m128_f32[0] < 1e-6f) return; // 너무 가까우면 회전하지 않음
//
//    vDirection = XMVector3Normalize(vDirection);
//
//    // 목표 각도 계산
//    _float fTargetYaw = atan2f(XMVectorGetX(vDirection), XMVectorGetZ(vDirection));
//    _float fCurrentYaw = m_pTransformCom->GetYawFromQuaternion();
//
//    // 각도 차이 (최단 경로)
//    _float fYawDiff = fTargetYaw - fCurrentYaw;
//    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
//    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;
//
//    // 부드러운 회전 적용
//    _float fRotationAmount = fRotationSpeed * fTimeDelta;
//
//    // 남은 각도가 작으면 더 천천히
//    if (fabsf(fYawDiff) < 0.5f) // 30도 이하일 때
//        fRotationAmount *= 0.1f; // 50% 감속
//
//    if (fabsf(fYawDiff) > fRotationAmount)
//    {
//        fYawDiff = (fYawDiff > 0) ? fRotationAmount : -fRotationAmount;
//    }
//
//    // 새로운 회전 적용
//    _float fNewYaw = fCurrentYaw + fYawDiff;
//    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
//    m_pTransformCom->Set_Quaternion(qNewRot);
//}


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
    m_MonsterStat.fMaxHP = pDesc->fMaxHP;
    m_MonsterStat.fAttackPower = pDesc->fAttackPower;
    m_MonsterStat.fDetectionRange = pDesc->fDetectionRange;
    m_MonsterStat.fAttackRange = pDesc->fAttackRange;
    m_MonsterStat.fMoveSpeed = pDesc->fMoveSpeed;
    m_MonsterStat.fRotationSpeed = pDesc->fRotationSpeed;

    return S_OK;
}
#pragma endregion

void CMonster::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

    m_pTarget = nullptr; // 상호 참조 문제.
}