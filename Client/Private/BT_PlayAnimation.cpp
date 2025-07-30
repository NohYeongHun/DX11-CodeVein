#include "BT_PlayAnimation.h"
CBT_PlayAnimation::CBT_PlayAnimation(_uint iAnimIndex, _bool bLoop, _float fMinDuration)
    : m_iAnimIndex(iAnimIndex), m_bLoop(bLoop), m_fMinDuration(fMinDuration)
{
}

BT_RESULT CBT_PlayAnimation::Execute(_float fTimeDelta)
{
    CMonster* pMonster = dynamic_cast<CMonster*>(m_pOwner);
    if (nullptr == pMonster)
        return BT_RESULT::FAILURE;

    // 애니메이션 시작
    if (!m_bAnimStarted)
    {
        pMonster->Play_Animation(m_iAnimIndex, m_bLoop);
        m_bAnimStarted = true;
    }

    m_fCurrentTime += fTimeDelta;

    // 최소 지속시간 체크
    if (m_fCurrentTime >= m_fMinDuration)
    {
        // 루프가 아니거나 애니메이션이 끝났으면 완료
        if (!m_bLoop)
            return BT_RESULT::SUCCESS;

        // 루프 애니메이션은 외부에서 중단시켜야 함
        return BT_RESULT::RUNNING;
    }

    return BT_RESULT::RUNNING;
}

void CBT_PlayAnimation::Reset()
{
    m_fCurrentTime = 0.f;
    m_bAnimStarted = false;
}

void CBT_PlayAnimation::Free()
{
    __super::Free();
}
