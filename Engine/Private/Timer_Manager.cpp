#include "Timer_Manager.h"

CTimer_Manager::CTimer_Manager()
{

}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	_float fTimeDelta = pTimer->Get_TimeDelta();

	if (m_fSlowTime > 0.f)
	{
		// Timer_Default는 실제 시간 흐름을 측정하므로 효과를 적용하지 않고,
		// 그 외의 타이머(예: Timer_60)에만 느려진 시간 값을 반환합니다.
		if (strTimerTag != TEXT("Timer_Default"))
			return fTimeDelta * m_fSlowFactor;
	}


	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Compute_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
	
}

#pragma region 느리게
void CTimer_Manager::Set_SlowMoment(_float fSlowTime, _float fSlowFactor)
{
	m_fSlowTime = fSlowTime;
	m_fSlowFactor = fSlowFactor;
}

void CTimer_Manager::Time_Pass(const _wstring& strTimerTag)
{
	// 슬로우 모션 효과가 활성화된 경우에만 실행
	if (m_fSlowTime > 0.f)
	{
		// 실제 시간이 얼마나 흘렀는지 기준 타이머(Timer_Default)를 통해 확인합니다.
		CTimer* pTimer = Find_Timer(strTimerTag);
		if (nullptr == pTimer)
			return;

		// 실제 흐른 시간만큼 남은 슬로우 모션 시간을 차감합니다.
		m_fSlowTime -= pTimer->Get_TimeDelta();

		// 남은 시간이 0보다 작아지면 효과를 종료합니다.
		if (m_fSlowTime < 0.f)
			m_fSlowTime = 0.f;
	}
}
#pragma endregion



HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	/*m_Timers.insert({ strTimerTag, pTimer });*/

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	/*auto		iter = find_if(m_mapTimer.NS_BEGIN(), m_mapTimer.end(), CTag_Finder(pTimerTag));*/
	auto		iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	CBase::Free();

	for (auto& Pair : m_Timers)	
		Safe_Release(Pair.second);

	m_Timers.clear();
	
}
