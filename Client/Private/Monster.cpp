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

    /* 생성과 동시에 어느정도 필요한 값들의 초기화를 진행 */
    m_MonsterStat.fAttackPower = pDesc->fAttackPower;
    m_MonsterStat.fAttackRange = pDesc->fAttackRange;
    m_MonsterStat.fDetectionRange = pDesc->fDetectionRange;
    m_MonsterStat.fMaxHP = pDesc->fMaxHP;
    m_MonsterStat.fHP = pDesc->fMaxHP;
    m_MonsterStat.fMoveSpeed = pDesc->fMoveSpeed;
    m_MonsterStat.fRotationSpeed = pDesc->fRotationPerSec;
    

    /* 기본적으로 몬스터는 모두 Navigation 설정. */
    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components");
        return E_FAIL;
    }

    if (FAILED(__super::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone");
        return E_FAIL;
    }

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{ 
    CContainerObject::Update(fTimeDelta);
}

void CMonster::Finalize_Update(_float fTimeDelta)
{
    __super::Finalize_Update(fTimeDelta);


    if (m_pColliderCom)
    {
        /* 콜라이더의 위치는 계속 업데이트 해주어야함. => 이상한 위치에 존재할 수도 있으므로.*/
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

        /* 콜라이더 매니저에서 상태비교 하는거는 카메라 프러스텀을 이용해서 제어. */
        //if (m_pGameInstance->Is_In_Camera_Frustum(m_pTransformCom->Get_State(STATE::POSITION)))
        //    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom);
    }
}

void CMonster::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

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

// 연계 공격 전용
void CMonster::Change_Animation_Combo(_uint iAnimationIndex)
{
    // 루트 모션 연속성을 보장하면서 즉시 전환
    m_pModelCom->Set_Animation(iAnimationIndex, false);
}

void CMonster::RootMotion_Translate(_fvector vTranslate)
{
    if (nullptr == m_pTransformCom)
    {
    	CRASH("Transform Component Not Exist");
    	return;
    }
    
    m_pTransformCom->Translate(vTranslate, m_pNavigationCom);
}

/* Navigation 진행 함수 */
void CMonster::Move_Direction(_fvector vDirection, _float fTimeDelta)
{
    m_pTransformCom->Move_Direction(vDirection, fTimeDelta, m_pNavigationCom);
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

void CMonster::Set_RootMotionRotation(_bool IsRotate)
{
    m_pModelCom->Set_RootMotionRotation(IsRotate);
}

void CMonster::Set_RootMotionTranslate(_bool IsTranslate)
{
    m_pModelCom->Set_RootMotionTranslate(IsTranslate);
}



#pragma endregion

#pragma region 2. 몬스터는 자신에게 필요한 수치 값이 존재한다.
const _bool CMonster::Is_TargetAttackRange()
{
    if (nullptr == m_pTarget)
        CRASH("Failed Target Search");

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 두 위치의 차이벡터 계산
    _vector vDirection = vTargetPos - vMyPos;

    // 거리 계산
    _float fDistance = XMVectorGetX(XMVector3Length(vDirection));

    return fDistance <= m_MonsterStat.fAttackRange;
}

// 현재 문제점 => 최소 탐지거리일때도 계속 탐지됨 => 그떄는 탐지 상태로 빠지면 안된다.
const _bool CMonster::Is_TargetDetectionRange()
{
    if (nullptr == m_pTarget)
        CRASH("Failed Target Search");

    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 두 위치의 차이벡터 계산
    _vector vDirection = vTargetPos - vMyPos;

    // 거리 계산
    _float fDistance = XMVectorGetX(XMVector3Length(vDirection));

    return (fDistance <= m_MonsterStat.fDetectionRange) && (fDistance >= m_fMinDetectionDistance);
}

#pragma endregion



#pragma region BUFF Flag 관리



// Timer가 종료되거나 특정 시점에는 BuffFlag를 해제합니다.
void CMonster::RemoveBuff(uint32_t buffFlag, _bool removeTimer)
{
    m_ActiveBuffs &= ~buffFlag;

    if (removeTimer)
        m_BuffTimers.erase(buffFlag); 
}

const _bool CMonster::AddBuff(_uint buffFlag, _float fCustomDuration)
{
    if (IsBuffOnCooldown(buffFlag))
        return false;

    m_ActiveBuffs |= buffFlag;

    // 시간을 지정한 경우에만 해당 시간으로 설정해줍니다. 
    if (m_BuffDefault_Durations[buffFlag] > 0.f)
    {
        m_BuffTimers[buffFlag] = fCustomDuration > 0.f ? fCustomDuration
            : m_BuffDefault_Durations[buffFlag];
        // 버프 타이머를 설정하면 자동으로 삭제되므로 상시 유지되는 버프도 삭제될 수도 있음
        // => Phase 상태.
    }
    

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
        RemoveBuff(expiredBuff, true);

    expiredBuffs.clear();

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
    m_MonsterStat.fMaxHP = pDesc->fMaxHP;
    m_MonsterStat.fAttackPower = pDesc->fAttackPower;
    m_MonsterStat.fDetectionRange = pDesc->fDetectionRange;
    m_MonsterStat.fAttackRange = pDesc->fAttackRange;
    m_MonsterStat.fMoveSpeed = pDesc->fMoveSpeed;
    m_MonsterStat.fRotationSpeed = pDesc->fRotationSpeed;
    m_MonsterStat.fRotationSpeed = pDesc->fRotationPerSec;

    return S_OK;
}

#pragma endregion

#pragma region 5. 특수한 상태들을 제어하기 위한 함수들.

// 프레임에 따라 회전.
void CMonster::Rotate_ToTarget(_float fTimeDelta)
{

    // 현재 몬스터 위치와 타겟 위치
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

    // 방향 벡터 계산 (Y축 제거하여 지면에서만 회전)
    _vector vDirection = vTargetPos - vMyPos;
    vDirection = XMVectorSetY(vDirection, 0.f);
    vDirection = XMVector3Normalize(vDirection);

    // 방향이 0벡터인 경우 처리
    if (XMVector3Equal(vDirection, XMVectorZero()))
        return;

    // 목표 Yaw 각도 계산
    _float x = XMVectorGetX(vDirection);
    _float z = XMVectorGetZ(vDirection);
    _float fTargetYaw = atan2f(x, z);

    // 각도 정규화 (-π ~ π)
    while (fTargetYaw > XM_PI) fTargetYaw -= XM_2PI;
    while (fTargetYaw < -XM_PI) fTargetYaw += XM_2PI;

    // 현재 회전 각도 가져오기
    _float fCurrentYaw = m_pTransformCom->GetYawFromQuaternion();

    // 회전 차이 계산
    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 최단 경로로 회전하도록 각도 조정
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    // 프레임 기반 회전 속도 적용 (몬스터 스탯의 회전 속도 사용)
    _float fMaxRotation = m_MonsterStat.fRotationSpeed * fTimeDelta;

    // 회전 속도 제한
    if (fabsf(fYawDiff) > fMaxRotation)
    {
        fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
    }

    // 새로운 회전 적용
    _float fNewYaw = fCurrentYaw + fYawDiff;
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);
}

void CMonster::RotateTurn_ToTarget()
{
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vDirection = vTargetPos - vMyPos;  // 방향 벡터 계산

    _float3 vResult = {};
    XMStoreFloat3(&vResult, vTargetPos);
    m_pTransformCom->LookAt(vResult);
}

void CMonster::RotateTurn_ToTargetYaw(_float fTimeDelta)
{

    // 현재 몬스터 위치와 타겟 위치
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

    // 방향 벡터 계산 (Y축 제거하여 지면에서만 회전)
    _vector vDirection = vTargetPos - vMyPos;
    
    // 너무 가까이에 있으면 회전을 자제한다. => 
    if (XMVectorGetX(XMVector4Length(vDirection)) < m_fMinRotationDistance)
        return;

    vDirection = XMVectorSetY(vDirection, 0.f);
    vDirection = XMVector3Normalize(vDirection);

    // 방향이 0벡터인 경우 처리
    if (XMVector3Equal(vDirection, XMVectorZero()))
        return;

    // 목표 Yaw 각도 계산
    _float x = XMVectorGetX(vDirection);
    _float z = XMVectorGetZ(vDirection);
    _float fTargetYaw = atan2f(x, z);

    // 각도 정규화 (-π ~ π)
    while (fTargetYaw > XM_PI) fTargetYaw -= XM_2PI;
    while (fTargetYaw < -XM_PI) fTargetYaw += XM_2PI;

    // 현재 회전 각도 가져오기
    _float fCurrentYaw = m_pTransformCom->GetYawFromQuaternion();

    // 회전 차이 계산
    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 최단 경로로 회전하도록 각도 조정
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    // 프레임 기반 회전 속도 적용 (몬스터 스탯의 회전 속도 사용)
    _float fMaxRotation = m_MonsterStat.fRotationSpeed * fTimeDelta;

    // 회전 속도 제한
    if (fabsf(fYawDiff) > fMaxRotation)
    {
        fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
    }

    // 새로운 회전 적용
    _float fNewYaw = fCurrentYaw + fYawDiff;
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);
}


void CMonster::RotateTurn_ToTargetYaw()
{
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

    
    _vector vDirection = vTargetPos - vMyPos;

    if (XMVectorGetX(XMVector4Length(vDirection)) < m_fMinRotationDistance)
        return;

    // 방향 벡터 계산 (Y축 제거)
    vDirection = XMVectorSetY(vDirection, 0.f);

    if (XMVectorGetX(XMVector3Length(vDirection)) < 0.001f)
        return;

    vDirection = XMVector3Normalize(vDirection);

    // Yaw 각도 계산
    _float x = XMVectorGetX(vDirection);
    _float z = XMVectorGetZ(vDirection);
    _float fTargetYaw = atan2f(x, z);

    // 즉시 회전 적용
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetYaw);
    m_pTransformCom->Set_Quaternion(qNewRot);
}

const _bool CMonster::IsRotateFinished(_float fRadian)
{
    if (nullptr == m_pTarget)
        return true;

    // 현재 몬스터와 타겟 방향 계산
    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vToTarget = vTargetPos - vMyPos;
    vToTarget = XMVectorSetY(vToTarget, 0.f);
    vToTarget = XMVector3Normalize(vToTarget);

    // 현재 몬스터의 전방 벡터
    _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);
    vLook = XMVector3Normalize(vLook);

    // 내적으로 각도 차이 계산
    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));
    _float fAngleDiff = acosf(max(-1.f, min(1.f, fDot)));

    // 허용 각도 (약 10도)
    const _float ROTATION_THRESHOLD = fRadian;

    return fAngleDiff <= ROTATION_THRESHOLD;
}





/* 현재 타입에 맞는 콜라이더 부속을 Enable, Disable 합니다. */
/* Parts일 수도 있고, Type은 자기 마음대로.*/
#pragma endregion


#pragma region 6. 몬스터 삭제 처리.
_bool CMonster::Monster_Dead()
{
    // 피가 0이면서 Dead 상태이면서.
    return m_MonsterStat.fHP <= 0.f && HasBuff(BUFF_DEAD) 
        && m_BuffTimers[BUFF_DEAD] <= 0.f;
}

#pragma endregion

#pragma region 99. DEBUG용도 함수.
#ifdef _DEBUG

void CMonster::Print_Position()
{
    _float3 vDebugPos = {};

    _wstring wstrTag = m_strObjTag + TEXT("| Pos : ");
    OutputDebugWstring(wstrTag);

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    XMStoreFloat3(&vDebugPos, vPos);
    OutPutDebugFloat3(vDebugPos);
}
#endif // _DEBUG
#pragma endregion




void CMonster::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);

    m_pTarget = nullptr; // 상호 참조 문제.
}