#include "Fsm.h"

CFsm::CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{

}

CFsm::CFsm(const CFsm& Prototype)
    : CComponent(Prototype)
{
}

HRESULT CFsm::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFsm::Initialize_Clone(void* pArg)
{
    FSM_DESC* pDesc = static_cast<FSM_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    return S_OK;
}

void CFsm::Update(_float fTimeDelta)
{
    if (m_pNowState)
        m_pNowState->Update(fTimeDelta);
}

void CFsm::Add_State(CState* pState)
{
    m_States.emplace_back(pState);
}

void CFsm::Change_State(_uint iState, void* pArg)
{
    // 이전 상태를 현재 상태로 변경.
    if (m_pNowState != nullptr)
    {

        m_pPreState = m_pNowState;
        m_pNowState->Exit(); // 종료시 동작할 작업 진행.
        m_pNowState->Reset(); // 현재 상태 초기화.
    }

    m_pNowState = m_States[iState];
    // 상태 실행 시 필요한 값들을 채워넣습니다.
    m_pNowState->Enter(pArg); 
}

CFsm* CFsm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFsm* pInstance = new CFsm(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed FSM"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CFsm::Clone(void* pArg)
{
    CFsm* pInstance = new CFsm(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed FSM"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFsm::Free()
{
    __super::Free();

    for (auto& vec : m_States)
        Safe_Release(vec);

    m_States.clear();

    m_pOwner = { nullptr };
}
