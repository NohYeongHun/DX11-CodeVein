#include "PlayerState.h"

HRESULT CPlayerState::Initialize(_uint iStateNum, void* pArg)
{
    PLAYER_STATE_DESC* pDesc = static_cast<PLAYER_STATE_DESC*>(pArg);
    m_pFsm = pDesc->pFsm;
    m_pPlayer = dynamic_cast<CPlayer*>(pDesc->pOwner);
    m_pModelCom = dynamic_cast<CLoad_Model*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
    m_iStateNum = iStateNum;
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

// 누른 것에 대한 상태도 확인.
CPlayerState::INPUT_INFO CPlayerState::Get_MoveMentInfo()
{
    INPUT_INFO input{};
    input.bW = m_pGameInstance->Get_KeyPress(DIK_W);
    input.bS = m_pGameInstance->Get_KeyPress(DIK_S);
    input.bA = m_pGameInstance->Get_KeyPress(DIK_A);
    input.bD = m_pGameInstance->Get_KeyPress(DIK_D);
    input.bMouseL = m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::LB);

    input.bAnyMovementKey = input.bW || input.bS || input.bA || input.bD || input.bMouseL;
    input.eDirection = Calculate_Direction(input.bW, input.bS, input.bA, input.bD);

    return input;
}

DIR CPlayerState::Calculate_Direction(_bool bW, _bool bS, _bool bA, _bool bD)
{
    DIR eDir = DIR::END;

                
    // 방향 결정
    if (bW && bA)      eDir = DIR::LU;
    else if (bW && bD) eDir = DIR::RU;
    else if (bS && bA) eDir = DIR::LD;
    else if (bS && bD) eDir = DIR::RD;
    else if (bW)       eDir = DIR::U;
    else if (bS)       eDir = DIR::D;
    else if (bA)       eDir = DIR::L;
    else if (bD)       eDir = DIR::R;

    return eDir;
}


void CPlayerState::Free()
{
    __super::Free();
    m_pPlayer = nullptr;
    m_pModelCom = nullptr;
    
}
