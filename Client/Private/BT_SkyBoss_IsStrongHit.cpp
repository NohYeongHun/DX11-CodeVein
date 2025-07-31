#include "BT_SkyBoss_IsStrongHit.h"

CBT_SkyBoss_IsStrongHit::CBT_SkyBoss_IsStrongHit(CSkyBoss* pOwner)
    : m_pOwner(pOwner)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pOwner);
    Safe_AddRef(m_pGameInstance);
}

/* 강한 피격을 받았는가? */
_bool CBT_SkyBoss_IsStrongHit::Check_Condition()
{
    // SkyBoss에서 강한 피격 상태인지 확인
    // Monster 클래스에 Is_StrongHit() 함수가 있다고 가정
    return m_pOwner->Is_StrongHit();
}

CBT_SkyBoss_IsStrongHit* CBT_SkyBoss_IsStrongHit::Create(CSkyBoss* pOwner)
{
    if (nullptr == pOwner)
    {
        CRASH("Failed Create CBT_SkyBoss_IsStrongHit");
        return nullptr;
    }
    return new CBT_SkyBoss_IsStrongHit(pOwner);
}

void CBT_SkyBoss_IsStrongHit::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pOwner);
}
