#pragma once


#include "Timer.h"

NS_BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);

public:
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);

private:		
	map<const _wstring, CTimer*>	m_Timers;

#pragma region 느리게

public:
	void Set_SlowMoment(_float fSlowTime, _float fSlowFactor);
	void Time_Pass(const _wstring& strTimerTag);

private:
	_float m_fSlowTime = {};   
	_float m_fSlowFactor = {}; // 느리게 할 크기.
#pragma endregion


private:
	CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free();
};

NS_END