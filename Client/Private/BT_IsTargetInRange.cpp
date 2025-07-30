#include "BT_IsTargetInRange.h"

CBT_IsTargetInRange::CBT_IsTargetInRange(CGameObject* pOwner, _float fRange)
	: m_pOwner(pOwner)
	, m_fDetectionRange(fRange)
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
	 Safe_AddRef(m_pOwner);
     Safe_AddRef(m_pGameInstance);
}

_bool CBT_IsTargetInRange::Check_Condition()
{
    if (!m_pOwner) return false;

    // GameInstance에서 플레이어 찾기

   /* CGameObject* pPlayer = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);

    if (!pPlayer) return false;

    _vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(STATE::POSITION);
    _vector vPlayerPos = pPlayer->Get_Transform()->Get_State(STATE::POSITION);

    _float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vOwnerPos));
    return fDistance <= m_fDetectionRange;*/

    return true;
}

CBT_IsTargetInRange* CBT_IsTargetInRange::Create(CGameObject* pOwner, _float fRange)
{
    return new CBT_IsTargetInRange(pOwner, fRange);
}

void CBT_IsTargetInRange::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pOwner);
}
