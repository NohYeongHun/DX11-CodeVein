CBT_Monster_IsRotate::CBT_Monster_IsRotate(CMonster* pOwner)
    : m_pOwner(pOwner)
{
    m_strTag = L"CBT_Monster_IsRotate";
}

/* 
* 현재 타겟과의 각도차를 확인.
*/
_bool CBT_Monster_IsRotate::Check_Condition()
{
    // 기본 무효 조건들 체크
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DEAD))
        return false;

    // 현재 다른 중요한 행동 중이면 회전하지 않음
    if (m_pOwner->HasAnyBuff(CMonster::BUFF_DOWN | CMonster::BUFF_HIT | CMonster::BUFF_DETECT))
        return false;

    // 공격 범위에 있으면 회전보다는 공격 우선
    _vector vMyPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pOwner->Get_Target()->Get_Transform()->Get_State(STATE::POSITION);
    _vector vToTarget = vTargetPos - vMyPos;

	_float fDistance = XMVectorGetX(XMVector3Length(vToTarget));
    /* Detection Range보다 크면 회전하지 않음. */
    if (fDistance >= m_pOwner->Get_DetectionRange())
        return false;

    
    // 타겟이 감지 범위에 있고, 각도 차이가 클 때만 회전
    return m_pOwner->Is_RotateCondition();
}

CBT_Monster_IsRotate* CBT_Monster_IsRotate::Create(CMonster* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_Monster_IsRotate");
        return nullptr;
    }
    return new CBT_Monster_IsRotate(pOwner);
}

void CBT_Monster_IsRotate::Free()
{
    CBTCondition::Free();
    m_pOwner = nullptr;
}
