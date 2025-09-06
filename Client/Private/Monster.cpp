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
    if (FAILED(CContainerObject::Initialize_Prototype()))
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
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);

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

    if (FAILED(CContainerObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone");
        return E_FAIL;
    }

    // 위치 지정.
    _vector vMonsterPos = XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, vMonsterPos);

    // 락온 위치 지정
    m_vLockOnOffset = pDesc->vLockOnOffset;

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
    CContainerObject::Finalize_Update(fTimeDelta);


    if (m_pColliderCom)
    {
        /* 콜라이더의 위치는 계속 업데이트 해주어야함. => 이상한 위치에 존재할 수도 있으므로.*/
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
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

#pragma region TRIGGER -> TO OBJECT_MANAGER

void CMonster::OnMoved_ToObjectManager()
{
    Collider_All_Active(true);
    
}
_float CMonster::Get_TargetDistance()
{
	_vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

    return XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));
}

#pragma endregion


#pragma region 몬스터 TRAIL 관리

// 추가
void CMonster::Add_Trail_Frame(_uint iAnimationIndex, _float fStartRatio, _float fEndRatio, _uint iPartType)
{
    MONSTER_TRAIL_FRAME traillFrame(fStartRatio, fEndRatio, iPartType);

    // 해당 애니메이션의 콜라이더 제어 정보가 없으면 생성
    if (m_TrailControlMap.find(iAnimationIndex) == m_TrailControlMap.end())
    {
        m_TrailControlMap[iAnimationIndex] = MONSTER_TRAIL_CONTROL();
    }

    // Trail 프레임 구조체를 넣어줍니다.
    m_TrailControlMap[iAnimationIndex].vecTrailFrames.push_back(traillFrame);

    // Part별 인덱스 초기화
    if (m_TrailControlMap[iAnimationIndex].partCurrentIndex.find(iPartType) ==
        m_TrailControlMap[iAnimationIndex].partCurrentIndex.end())
    {
        m_TrailControlMap[iAnimationIndex].partCurrentIndex[iPartType] = 0;
        m_TrailControlMap[iAnimationIndex].partProcessed[iPartType] = false;
    }
}

void CMonster::Handle_Trail_State()
{
    _float fCurrentRatio = m_pModelCom->Get_Current_Ratio();

    auto iter = m_TrailControlMap.find(Get_CurrentAnimation());
    if (iter == m_TrailControlMap.end())
        return;

    MONSTER_TRAIL_CONTROL& trailControl = iter->second;

    // 모든 콜라이더 프레임을 순회
    for (auto& trailFrame : trailControl.vecTrailFrames)
    {
        _uint partType = trailFrame.iPartType;

        // 현재 TRAIL 구간 확인
        _bool bShouldActive = (fCurrentRatio >= trailFrame.fStartRatio &&
            fCurrentRatio <= trailFrame.fEndRatio);

        // Part별 이전 상태와 비교
        _bool bPrevState = m_PartPrevColliderState[partType];

        if (bShouldActive != bPrevState)
        {
            if (bShouldActive)
            {
                // 콜라이더 활성화
                Enable_Trail(partType);
                trailFrame.bIsActive = true;
            }
            else
            {
                // Trail 비활성화
                Disable_Trail(partType);
                trailFrame.bIsActive = false;
            }

            m_PartPrevTrailState[partType] = bShouldActive;
        }

        // EndRatio를 벗어난 경우 강제로 콜라이더 비활성화
        if (fCurrentRatio > trailFrame.fEndRatio && trailFrame.bIsActive)
        {
            Disable_Trail(partType);
            trailFrame.bIsActive = false;
            m_PartPrevTrailState[partType] = false;
        }
    }
    
    // 애니메이션이 완료된 경우 모든 콜라이더 비활성화 (안전장치)
    if (m_pModelCom->Is_Finished())
    {
        for (auto& trailFrame : trailControl.vecTrailFrames)
        {
            if (trailFrame.bIsActive)
            {
                Disable_Trail(trailFrame.iPartType);
                trailFrame.bIsActive = false;
                m_PartPrevTrailState[trailFrame.iPartType] = false;
            }
        }
    }
}
void CMonster::Reset_Trail_ActiveInfo()
{
    for (auto& pair : m_TrailControlMap)
    {
        MONSTER_TRAIL_CONTROL& trailControl = pair.second;

        // Part별 인덱스와 상태 초기화
        for (auto& partPair : trailControl.partCurrentIndex)
        {
            trailControl.partCurrentIndex[partPair.first] = 0;
            trailControl.partProcessed[partPair.first] = false;
        }

        // 모든 콜라이더 프레임 비활성화
        for (auto& trailFrame : trailControl.vecTrailFrames)
        {
            trailFrame.bIsActive = false;
        }
    }

    // Part별 이전 상태도 초기화
    m_PartPrevTrailState.clear();
}
#pragma endregion


#pragma region 0. 충돌시 발생하는 이벤트에 대한 컨트롤.

void CMonster::On_Collision_Enter(CGameObject* pOther)
{
}

void CMonster::On_Collision_Stay(CGameObject* pOther)
{
}

void CMonster::On_Collision_Exit(CGameObject* pOther)
{
}


void CMonster::Take_Damage(_float fDamage, CGameObject* pGameObject)
{
    m_MonsterStat.fHP -= fDamage;

    CPlayerWeapon* pPlayerWeapon = dynamic_cast<CPlayerWeapon*>(pGameObject);
    if (nullptr != pPlayerWeapon)
    {
        const _float4x4* pCombinedWorldMatrix = pPlayerWeapon->Get_CombinedWorldMatrix();
        if (!pCombinedWorldMatrix) return;

        _vector vWeaponPosition = XMLoadFloat4(reinterpret_cast<const _float4*>(&pCombinedWorldMatrix->m[3][0]));

        CBounding_Sphere::BOUNDING_SPHERE_DESC* pDesc =
            static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(m_pColliderCom->Get_BoundingDesc());
        if (!pDesc) return;

        // === 안정화된 계산 ===
        _vector vMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vCenter = XMVectorAdd(vMonsterPos, XMLoadFloat3(&pDesc->vCenter));
        _float fRadius = pDesc->fRadius;

        _vector vDir = XMVectorSubtract(vWeaponPosition, vCenter);

        // 길이 체크
        _float fDirLength = XMVectorGetX(XMVector3Length(vDir));
        if (fDirLength < 0.001f)
        {
            // 방향이 없으면 몬스터 중심 위치 사용
            Show_Slash_UI_At_Position(vCenter, XMVectorSet(1, 0, 0, 0));
            return;
        }

        // 정규화
        vDir = XMVectorScale(vDir, 1.0f / fDirLength);  // Normalize 대신 수동 계산

        // 표면점 계산 - MultiplyAdd 대신 분리
        _vector vRadiusOffset = XMVectorScale(vDir, fRadius);
        _vector vClosestPoint = XMVectorAdd(vCenter, vRadiusOffset);

        // 공격 방향 계산
        _vector vSwingDirection = pPlayerWeapon->Get_SwingDirection();
        _vector vAttackDirection = vSwingDirection;

        _float fSwingLength = XMVectorGetX(XMVector3Length(vAttackDirection));
        if (fSwingLength < 0.1f)
        {
            vAttackDirection = vDir;  // vDir 재사용
        }
        else
        {
            vAttackDirection = XMVectorScale(vAttackDirection, 1.0f / fSwingLength);
        }

        Show_Slash_UI_At_Position(vClosestPoint, vAttackDirection);
    }

}
#pragma endregion



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

/* Animation Speed 증가.*/
void CMonster::Set_Animation_AddSpeed(_uint iAnimationIndex, _float fSpeed)
{
    m_pModelCom->Set_AnimSpeed(iAnimationIndex, m_pModelCom->Get_AnimSpeed(iAnimationIndex) + fSpeed);
}

_float CMonster::Get_Animation_Speed(_uint iAnimationIndex)
{
    return m_pModelCom->Get_AnimSpeed(iAnimationIndex);
}

void CMonster::Set_Animation_Speed(_uint iAnimationIndex, _float fSpeed)
{
    m_pModelCom->Set_AnimSpeed(iAnimationIndex, fSpeed);
}

/* 애니메이션 인덱스.*/
_uint CMonster::Find_AnimationIndex(const _wstring& strAnimationTag)
{
    auto iter = m_Action_AnimMap.find(strAnimationTag);

    // 찾는 애니메이션이 없는 경우
    if (iter == m_Action_AnimMap.end())
    {
        return (_uint)-1; // -1 반환으로 애니메이션 없음을 표시
    }

    return iter->second;
}

/* 현재 애니메이션이 끝났냐? */
const _bool CMonster::Is_Animation_Finished()
{
    return m_pModelCom->Is_Finished();
}

/* 현재 애니메이션이 전환 가능한가? */


void CMonster::Set_RootMotionRotation(_bool IsRotate)
{
    m_pModelCom->Set_RootMotionRotation(IsRotate);
}

void CMonster::Set_RootMotionTranslate(_bool IsTranslate)
{
    m_pModelCom->Set_RootMotionTranslate(IsTranslate);
}

void CMonster::Set_AnimationActivate()
{
    m_pModelCom->Set_AnimationActivate();
}

void CMonster::Set_AnimationDeActivate()
{
    m_pModelCom->Set_AnimationDeActivate();
}

void CMonster::Compute_OnCell()
{
    if (m_pNavigationCom)
    {
        m_pTransformCom->Set_State(STATE::POSITION
            , m_pNavigationCom->Compute_OnCell(
                m_pTransformCom->Get_State(STATE::POSITION)));
    }
    
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

// 히스테리시스 적용: 탐지 시작과 해제 범위를 다르게 설정
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


    return fDistance <= m_MonsterStat.fDetectionRange && fDistance >= m_fMinDetectionDistance;
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

// 현재 시간 반환.
const _float CMonster::Get_BuffTime(uint32_t buffFlag)
{
    auto iter = m_BuffTimers.find(buffFlag);
    if (iter == m_BuffTimers.end())
        return 0.f;
    return iter->second;;
}

const _float CMonster::Get_DefaultBuffTime(uint32_t buffFlag)
{
    auto iter = m_BuffDefault_Durations.find(buffFlag);
    if (iter == m_BuffDefault_Durations.end())
        return 0.f;
    return iter->second;;
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

/* Rotate를 해야하는가? */
_bool CMonster::Is_RotateCondition()
{
    if (nullptr == m_pTarget)
        return false;

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


    // 45도 이상이면 Rotate (더 자연스러운 회전 조건)
    return fAngleDiff >= XMConvertToRadians(30.f);
}

_vector CMonster::Get_TargetVector()
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

    return XMVector3Normalize(vTargetPos - vPos);
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

/* 근처 셀로 설정. */
void CMonster::NearCell_Translate()
{
    _float3 vPos = {};
    _float3 vFinalPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _int iNearCell = m_pNavigationCom->Find_NearCellIndex(vPos);

    if (iNearCell == -1)
    {
        CRASH("Failed Search Navigation Cell");
    }

    m_pNavigationCom->Set_CurrentCellIndex(iNearCell);
    XMStoreFloat3(&vFinalPos, m_pNavigationCom->Get_CellPos(iNearCell));
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vFinalPos));
}

const _float CMonster::Get_TargetDegreeNoPitch()
{
    if (!m_pTarget)
        return 0.f;

    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vToTarget = vTargetPos - vMyPos;
    vToTarget = XMVectorSetY(vToTarget, 0.f);
    vToTarget = XMVector3Normalize(vToTarget);

    _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);
    vLook = XMVector3Normalize(vLook);

    // 내적으로 각도 차이 계산
    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));
    _float fAngleDiff = acosf(max(-1.f, min(1.f, fDot)));

    // 라디안을 도로 변환
    return XMConvertToDegrees(fAngleDiff);
}

const CMonster::ROTATION_INFO CMonster::Get_SimpleTargetRotation()
{
    CMonster::ROTATION_INFO info = {};

    if (!m_pTarget)
        return info;

    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    _vector vToTarget = vTargetPos - vMyPos;
    vToTarget = XMVectorSetY(vToTarget, 0.f);

    _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);

    // atan2를 사용한 더 간단한 방법
    _float fTargetYaw = atan2f(XMVectorGetX(vToTarget), XMVectorGetZ(vToTarget));
    _float fCurrentYaw = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));

    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 최단 경로로 정규화
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    info.fSignedAngleDegrees = XMConvertToDegrees(fYawDiff);
    info.bIsRight = (fYawDiff > 0);

    return info;
}

/* Reset 시 모든 콜라이더 비활성화 */
void CMonster::Reset_Part_Colliders()
{
       
    // 2. 모든 Part 타입에 대해 콜라이더 비활성화 호출
    // 각 몬스터의 구현에 따라 처리됨 (WolfDevil의 경우 PART_WEAPON 등)
    Disable_Collider(0);
    
    // 3. 콜라이더 활성화 정보 초기화
    Reset_Collider_ActiveInfo();
}

void CMonster::Dead_Action()
{
    if (m_pColliderCom)
    {
        Collider_All_Active(false);
    }

}

/* 조우 거리인지만 체크.*/
_bool CMonster::Is_EncounterDistance()
{
    if (nullptr == m_pTarget)
        return false;

    _float fTargetDistnace = Get_TargetDistance();
    return fTargetDistnace <= m_fEncounterDistance;
}





/* 현재 타입에 맞는 콜라이더 부속을 Enable, Disable 합니다. */
/* Parts일 수도 있고, Type은 자기 마음대로.*/
#pragma endregion


#pragma region 7. 몬스터 삭제 처리. => 이건 Dead Node에서 처리하기?

_bool CMonster::Monster_Dead()
{
    // 피가 0이면서 Dead 상태이면서.
    return m_MonsterStat.fHP <= 0.f && HasBuff(BUFF_DEAD) 
        && m_BuffTimers[BUFF_DEAD] <= 0.f;
}

#pragma endregion

#pragma region 99. DEBUG용도 함수.
#ifdef _DEBUG

const _vector CMonster::Get_LockOnOffset()
{
    _vector vLockOnOffset = XMLoadFloat3(&m_vLockOnOffset);
    return vLockOnOffset;
}

void CMonster::Print_Position()
{
    _float3 vDebugPos = {};

    _wstring wstrTag = m_strObjTag + TEXT(" Pos : ");
    OutputDebugWstring(wstrTag);

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    XMStoreFloat3(&vDebugPos, vPos);
    OutPutDebugFloat3(vDebugPos);
}
#endif // _DEBUG
#pragma endregion

#pragma region 통합 콜라이더 제어 시스템 구현
void CMonster::Add_Collider_Frame(_uint iAnimationIndex, _float fStartRatio, _float fEndRatio, _uint iPartType)
{
	MONSTER_COLLIDER_FRAME colliderFrame(fStartRatio, fEndRatio, iPartType);

	// 해당 애니메이션의 콜라이더 제어 정보가 없으면 생성
	if (m_ColliderControlMap.find(iAnimationIndex) == m_ColliderControlMap.end())
	{
		m_ColliderControlMap[iAnimationIndex] = MONSTER_COLLIDER_CONTROL();
	}

	m_ColliderControlMap[iAnimationIndex].vecColliderFrames.push_back(colliderFrame);
	
	// Part별 인덱스 초기화
	if (m_ColliderControlMap[iAnimationIndex].partCurrentIndex.find(iPartType) == 
		m_ColliderControlMap[iAnimationIndex].partCurrentIndex.end())
	{
		m_ColliderControlMap[iAnimationIndex].partCurrentIndex[iPartType] = 0;
		m_ColliderControlMap[iAnimationIndex].partProcessed[iPartType] = false;
	}
}

void CMonster::Handle_Collider_State()
{
	_float fCurrentRatio = m_pModelCom->Get_Current_Ratio();
	
	auto iter = m_ColliderControlMap.find(Get_CurrentAnimation());
	if (iter == m_ColliderControlMap.end())
		return;

	MONSTER_COLLIDER_CONTROL& colliderControl = iter->second;
	
	// 모든 콜라이더 프레임을 순회
	for (auto& colliderFrame : colliderControl.vecColliderFrames)
	{
		_uint partType = colliderFrame.iPartType;
		
		// 현재 콜라이더 구간 확인
		_bool bShouldActive = (fCurrentRatio >= colliderFrame.fStartRatio && 
							   fCurrentRatio <= colliderFrame.fEndRatio);

		// Part별 이전 상태와 비교
		_bool bPrevState = m_PartPrevColliderState[partType];

		if (bShouldActive != bPrevState)
		{
			if (bShouldActive)
			{
				// 콜라이더 활성화
				Enable_Collider(partType);
				colliderFrame.bIsActive = true;
			}
			else
			{
				// 콜라이더 비활성화
				Disable_Collider(partType);
				colliderFrame.bIsActive = false;
			}

			m_PartPrevColliderState[partType] = bShouldActive;
		}
		
		// EndRatio를 벗어난 경우 강제로 콜라이더 비활성화
		if (fCurrentRatio > colliderFrame.fEndRatio && colliderFrame.bIsActive)
		{
			Disable_Collider(partType);
			colliderFrame.bIsActive = false;
			m_PartPrevColliderState[partType] = false;
		}
	}
	
	// 애니메이션이 완료된 경우 모든 콜라이더 비활성화 (안전장치)
	if (m_pModelCom->Is_Finished())
	{
		for (auto& colliderFrame : colliderControl.vecColliderFrames)
		{
			if (colliderFrame.bIsActive)
			{
				Disable_Collider(colliderFrame.iPartType);
				colliderFrame.bIsActive = false;
				m_PartPrevColliderState[colliderFrame.iPartType] = false;
			}
		}
	}
}

void CMonster::Reset_Collider_ActiveInfo()
{
	for (auto& pair : m_ColliderControlMap)
	{
		MONSTER_COLLIDER_CONTROL& colliderControl = pair.second;
		
		// Part별 인덱스와 상태 초기화
		for (auto& partPair : colliderControl.partCurrentIndex)
		{
			colliderControl.partCurrentIndex[partPair.first] = 0;
			colliderControl.partProcessed[partPair.first] = false;
		}
		
		// 모든 콜라이더 프레임 비활성화
		for (auto& colliderFrame : colliderControl.vecColliderFrames)
		{
			colliderFrame.bIsActive = false;
		}
	}
	
	// Part별 이전 상태도 초기화
	m_PartPrevColliderState.clear();
}
#pragma endregion

#pragma region CSlash UI 관련
void CMonster::Show_Slash_UI_At_Position(_fvector vHitPosition, _fvector vAttackDirection)
{
    // 1. 풀에서 꺼내씁니다.
    CSlash::SLASHACTIVATE_DESC Desc{};
    Desc.eCurLevel = m_eCurLevel;
    Desc.vHitPosition = vHitPosition;
    Desc.vHitDirection = vAttackDirection;
    Desc.fDisPlayTime = 0.5f;
    HRESULT hr = m_pGameInstance->Move_GameObject_ToObjectLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("SLASH_EFFECT"), TEXT("Layer_Effect"), 1, ENUM_CLASS(CSlash::EffectType), &Desc);

    if (FAILED(hr))
        CRASH("Failed Slash Effecet");

}


#pragma endregion

void CMonster::Destroy()
{
    CContainerObject::Destroy();
    
    if (m_pColliderCom)
        m_pColliderCom->Set_Active(false);

}

void CMonster::Free()
{
    CContainerObject::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);

    m_pTarget = nullptr; // 상호 참조 문제.
}