#include "PlayerState.h"

HRESULT CPlayerState::Initialize(_uint iStateNum, void* pArg)
{
    PLAYER_STATE_DESC* pDesc = static_cast<PLAYER_STATE_DESC*>(pArg);
    m_pFsm = pDesc->pFsm;
    m_pPlayer = dynamic_cast<CPlayer*>(pDesc->pOwner);
    m_pModelCom = dynamic_cast<CLoad_Model*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
    if (nullptr == m_pModelCom)
    {
        CRASH("Create Failed Player State");
        return E_FAIL;
    }

    if (nullptr == m_pPlayer)
    {
        CRASH("Create Failed Player State");
        return E_FAIL;
    }
    
    return S_OK;
}

void CPlayerState::Free()
{
    __super::Free();
    m_pPlayer = nullptr;
    m_pModelCom = nullptr;
    
}
