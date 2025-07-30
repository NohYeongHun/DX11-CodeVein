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

void CPlayerState::Handle_Input()
{
    m_eDir = m_pPlayer->Get_Direction();
    m_KeyInput = m_pPlayer->Get_KeyInput();

    if (m_pPlayer->Is_MovementKeyPressed())
        m_eDir = m_pPlayer->Calculate_Direction();
    else
        m_eDir = ACTORDIR::END;
}

void CPlayerState::Enter(void* pArg)
{
    m_pFsm->Set_StateCoolTime(m_iStateNum);
    m_pFsm->Set_StateExitCoolTime(m_iStateNum);
}

void CPlayerState::Handle_LockOn_Input(_float fTimeDelta)
{
    if (!Should_Use_LockOn_Logic())
        return;

    // LockOn 중에는 플레이어가 자동으로 타겟을 향해 회전하므로
    // 별도의 회전 처리는 Player 클래스에서 담당
}

void CPlayerState::Apply_LockOn_Movement(_float fTimeDelta, _float fSpeed)
{
    if (!Should_Use_LockOn_Logic())
        return;

    // LockOn 상태에서의 이동은 Player::Move_With_LockOn 사용
    m_pPlayer->Move_With_LockOn(fTimeDelta, fSpeed);
}

_bool CPlayerState::Should_Use_LockOn_Logic() const
{
    return m_LockOn && m_pPlayer->Get_LockOn_Target() != nullptr;
}


void CPlayerState::Free()
{
    __super::Free();
    m_pPlayer = nullptr;
    m_pModelCom = nullptr;
    
}
