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

    // CoolTime 계속 관리 중.
    for (auto& pair : m_fUseCoolTimes)
        if (pair.second > 0.f)
            pair.second -= fTimeDelta;

    // Exit CoolTime 계속 관리.
    for (auto& pair : m_fExitUseCoolTimes)
    {
        if (pair.second > 0.f)
            pair.second -= fTimeDelta;
    }
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

#pragma region 쿨타임 관리
void CFsm::Register_StateCoolTime(_uint iStateNum, _float fCoolTime)
{
    m_fCoolTimes.emplace(iStateNum, fCoolTime);
}

_float CFsm::Get_StateCoolTime(_uint iStateNum)
{
    return m_fCoolTimes[iStateNum];
}

void CFsm::Set_StateCoolTime(_uint iStateNum, _float fCoolTime)
{
    m_fUseCoolTimes[iStateNum] = fCoolTime;
}

void CFsm::Set_StateCoolTime(_uint iStateNum)
{
    m_fUseCoolTimes[iStateNum] = m_fCoolTimes[iStateNum];
}

_bool CFsm::Is_CoolTimeEnd(_uint iStateNum)
{
    
    return m_fUseCoolTimes[iStateNum] <= 0.f;
}

void CFsm::Register_StateExitCoolTime(_uint iStateNum, _float fCoolTime)
{
    m_fExitCoolTimes.emplace(iStateNum, fCoolTime);
}

_float CFsm::Get_StateExitCoolTime(_uint iStateNum)
{
    return m_fExitUseCoolTimes[iStateNum];
}

void CFsm::Set_StateExitCoolTime(_uint iStateNum, _float fCoolTime)
{
    m_fExitUseCoolTimes[iStateNum] = fCoolTime;
;}

void CFsm::Set_StateExitCoolTime(_uint iStateNum)
{
    m_fExitUseCoolTimes[iStateNum] = m_fExitCoolTimes[iStateNum];
}

_bool CFsm::Is_ExitCoolTimeEnd(_uint iStateNum)
{
    _bool bTest = m_fExitUseCoolTimes[iStateNum] <= 0.f;
    return m_fExitUseCoolTimes[iStateNum] <= 0.f;
}
#pragma endregion




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
    CComponent::Free();

    for (auto& vec : m_States)
        Safe_Release(vec);

    m_States.clear();

    m_pOwner = { nullptr };
}
