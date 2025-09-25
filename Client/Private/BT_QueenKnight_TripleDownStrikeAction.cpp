#include "BT_QueenKnight_TripleDownStrikeAction.h"

CBT_QueenKnight_TripleDownStrikeAction::CBT_QueenKnight_TripleDownStrikeAction(CQueenKnight* pOwner)
    : m_pOwner{ pOwner }
{
    m_strTag = L"CBT_QueenKnight_TripleDownStrikeAction";

    /* 공격 애니메이션이 총 3단계로 나뉨 */

     // 1. WARP_START
    m_fReady_StartRatio = 0.f / 80.f;
    m_fReady_EndRatio = 54.f / 80.f;

    // 2. 점프하는 판정 => 해당 시간 동안 디졸브
    m_fJump_StartRatio = 55.f / 80.f;
    m_fJump_EndRatio = 80.f / 80.f;


    m_fAttack_StartRatio = 21.f / 261.f;
    m_fAttack_EndRatio = 40.f / 261.f;

    m_fSkillStart_Ratio = 55.f / 261.f;

    m_vDesecndTarget = { 1.f, 1.f, 1.f };

}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Perform_Action(_float fTimeDelta)
{
    // DEAD 상태라면 Update 도중에 멈추고 다음 프레임으로 넘아가야함.
    if (m_pOwner->HasBuff(CMonster::BUFF_DEAD))
        return BT_RESULT::FAILURE;

    switch (m_eAttackPhase)
    {
    case ATTACK_PHASE::NONE:
        return Enter_Attack(fTimeDelta);
    case ATTACK_PHASE::ASCEND:
        return Update_Ascend(fTimeDelta);
    case ATTACK_PHASE::HANG:
        return Update_Hang(fTimeDelta);
    case ATTACK_PHASE::DESCEND:
        return Update_Descend(fTimeDelta);
    case ATTACK_PHASE::COMPLETED:
    {
        m_IsWarpSound = false;
        m_IsWarpSkillSound = false;
        return BT_RESULT::SUCCESS;
    }
        
    }

    return BT_RESULT::FAILURE;
}

void CBT_QueenKnight_TripleDownStrikeAction::Reset()
{
    m_eAttackPhase = ATTACK_PHASE::NONE;

    m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
    m_pOwner->Reset_Collider_ActiveInfo();
    m_pOwner->Set_Visible(true);
    m_IsChangeSpeed = false;
    m_IsDissolveCheck = false;

    m_pOwner->Set_Animation_Speed(m_pOwner->Find_AnimationIndex(L"WARP_END"), 1.2f);

    m_pOwner->End_Dissolve();

    // 위치 변수들을 명확히 초기화
    m_vAscendTarget = _float3{ 0.f, 0.f, 0.f };
    // 목표 하강 지점.
    m_vDesecndTarget = _float3{ 1.f, 1.f, 1.f };
    
    // 시간 변수 초기화
    m_fWaitTime = 0.f;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Enter_Attack(_float fTimeDelta)
{
    // 안전 코드 추가
    if (m_eAttackPhase != ATTACK_PHASE::NONE)
    {
        CRASH("Failed Tree Attack Enter Logic");
    }

    // 1. 애니메이션 전환.
    _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"WARP_START");

    //m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.2f, true, true, true);
    m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx, false);
    m_pOwner->RotateTurn_ToTargetYaw();

    // 2. 다음 단계 진행
    m_eAttackPhase = ATTACK_PHASE::ASCEND;



    // 3. Navigation 끄기
    m_pOwner->AddBuff(CMonster::BUFF_NAVIGATION_OFF, 10.f);

    // 4. 목표 위치 지정.
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    vOwnerPos.m128_f32[1] += 30.f;
    XMStoreFloat3(&m_vAscendTarget, vOwnerPos);

    m_pOwner->Disable_Collider(CQueenKnight::PART_BODY);

    return BT_RESULT::RUNNING;
}


BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Ascend(_float fTimeDelta)
{
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _float fCurrentRatio = m_pOwner->Get_CurrentAnimationRatio();

    if (!m_IsDissolveCheck && !m_pOwner->HasBuff(CMonster::BUFF_DISSOLVE) && fCurrentRatio >= m_fDissolve_StartRatio)
    {
        // 디졸브 시작.
        m_IsDissolveCheck = true; // 이미 Dissolve가 발생했는지?
        m_pOwner->Start_Dissolve(1.f);
        //m_pOwner->AddBuff(CMonster::BUFF_DISSOLVE);

        
    }

    if (vOwnerPos.m128_f32[1] <= m_vAscendTarget.y && fCurrentRatio >= m_fJump_StartRatio)
    {
        m_pOwner->Move_Direction({ 0.f, 1.f, 0.f }, fTimeDelta * 0.5f);
    }


    if (!m_IsWarpSound && fCurrentRatio > m_fJump_StartRatio)
    {
        m_IsWarpSound = true;
        m_pOwner->Play_Sound(CQueenKnight::SOUND_WARP_START);
    }

    // 1. 목표 높이까지 도달했는지 확인. 아니면 위로 이동.
    if (m_pOwner->Is_Animation_Finished() && vOwnerPos.m128_f32[1] >= m_vAscendTarget.y)
    {
        // 2. 높이를 유지한 채로 순간이동 페이즈로 진입.
        m_eAttackPhase = ATTACK_PHASE::HANG;

        // 4. 순간이동 페이즈에 진입할때 Player 머리 위로 위치 이동. => 이게 부자연스러운데..
        _float vPosY = vOwnerPos.m128_f32[1];

        /// 타겟 포스가 아니라. 맵 중앙.

        _vector vTargetPos = XMVectorSetY(XMLoadFloat3(&m_vDesecndTarget), vPosY);

        m_pOwner->Get_Transform()->Set_State(STATE::POSITION, vTargetPos);
        
        // 5. 애니메이션 전환.
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"WARP_SKILL");

        m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, true);

        m_pOwner->RotateTurn_ToTargetYaw();

        // 시점과 운동 방향 변경 필요. => 운동 방향은 몬스터 중앙에서 시작해서 아래로 퍼져나감.
        m_pOwner->Create_QueenKnightWarp_Effect_Particle({ 0.f, -1.f, 0.f });


        m_IsDissolveCheck = false;
    }

    // DISSOLVE가 끝나면 Reverse Dissolv 시작.
    if (!m_IsDissolveCheck && !m_pOwner->HasBuff(CMonster::BUFF_DISSOLVE))
    {
        // 시야에 켜기.

        // 2. 목표 하강지점 도착했으므로 렌더링 켜기 (시야에서 나타남)
        m_pOwner->ReverseStart_Dissolve(0.3f);
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Hang(_float fTimeDelta)
{

    // 1. 현재 위치
    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _vector vTargetPos = XMLoadFloat3(&m_vDesecndTarget);

    // 2. Y축 거리만 확인 (수평 거리는 이미 텔레포트로 도착했음)
    _float fYDistance = abs(XMVectorGetY(vOwnerPos) - XMVectorGetY(vTargetPos));

    _float fCurrentRatio = m_pOwner->Get_CurrentAnimationRatio();

    if (fYDistance >= 1.f)  // Y축 거리가 0.5 이상일 때만 하강
    {
        // 목표 Y 위치보다 높을 때만 하강
        if (XMVectorGetY(vOwnerPos) > XMVectorGetY(vTargetPos))
        {
            m_pOwner->Move_Direction({ 0.f, -1.f, 0.f }, fTimeDelta * 0.7f);
        }
        else
        {
            // 목표 지점에 도착했으므로 공격 페이즈로 전환
            if (m_pOwner->Get_CurrentAnimationRatio() >= m_fAttack_StartRatio)
            {
                m_eAttackPhase = ATTACK_PHASE::DESCEND;
                m_pOwner->Set_Animation_Speed(m_pOwner->Find_AnimationIndex(L"WARP_SKILL"), 1.6f);
                m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);

                // 스킬 시작.
                m_pOwner->Start_PillarSkill();
            }
        }
    }
    else
    {
        if (fCurrentRatio >= m_fAttack_StartRatio)
        {
            // 1. 공격 시작 애니메이션에 페이즈 변경.
            m_eAttackPhase = ATTACK_PHASE::DESCEND;

            // 2. 목표 하강지점 도착했으므로 렌더링 켜기 (시야에서 나타남)
            m_pOwner->Set_Animation_Speed(m_pOwner->Find_AnimationIndex(L"WARP_SKILL"), 1.6f);
            // 3. Navigation Off 해제. => 다시 Navigation을 타게 만듭니다.
            m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);

            // 4. 스킬 시작.
            m_pOwner->Start_PillarSkill();
        }
    }

    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Descend(_float fTimeDelta)
{
    _float fCurrentRatio = m_pOwner->Get_CurrentAnimationRatio();
    /*  if (!m_IsWarpSkillSound && fCurrentRatio > m_fSkillStart_Ratio)
    {
        m_IsWarpSkillSound = true;
        m_pOwner->Play_Sound(CQueenKnight::SOUND_WARP_END);
    }*/

    if (m_pOwner->Is_Animation_Finished())
    {
        m_eAttackPhase = ATTACK_PHASE::COMPLETED;
        m_pOwner->Enable_Collider(CQueenKnight::PART_BODY);
        m_pOwner->RemoveBuff(CMonster::BUFF_NAVIGATION_OFF, true);
        _uint iNextAnimationIdx = m_pOwner->Find_AnimationIndex(L"IDLE");

        //m_pOwner->Change_Animation_Blend(iNextAnimationIdx, false, 0.1f, true, true, false);
        m_pOwner->Change_Animation_NonBlend(iNextAnimationIdx, false);

        m_pOwner->AddBuff(CQueenKnight::QUEEN_BUFF_DOWN_TRIPLE_STRIKE_COOLDOWN);
    }


    return BT_RESULT::RUNNING;
}

BT_RESULT CBT_QueenKnight_TripleDownStrikeAction::Update_Skill(_float fTimeDelta)
{
    return BT_RESULT::RUNNING;
}






CBT_QueenKnight_TripleDownStrikeAction* CBT_QueenKnight_TripleDownStrikeAction::Create(CQueenKnight* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_QueenKnight_TripleDownStrikeAction");
        return nullptr;
    }

    return new CBT_QueenKnight_TripleDownStrikeAction(pOwner);
}

void CBT_QueenKnight_TripleDownStrikeAction::Free()
{
    CBTAction::Free();
    m_pOwner = nullptr;
}
