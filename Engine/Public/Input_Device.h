#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class  CInput_Device final : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	

#pragma region ENGINE에 제공
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)]; 
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *((reinterpret_cast<_int*>(&m_tMouseState)) + static_cast<_uint>(eMouseState));
	}


	// 이전 프레임에 키가 눌려있고 현재 프레임에도 누를 경우.
	_bool Get_KeyPress(_ubyte byKeyID);

	// 이전 프레임에 키가 안눌려있고 현재 프레임에 누를 경우.
	_bool Get_KeyDown(_ubyte byKeyID);


	// 이전 프레임에 키가 눌려져 있지 않고 현재 프레임에만 누른 경우
	_bool Get_KeyUp(_ubyte byKeyID);


	_bool Get_MouseKeyDown(MOUSEKEYSTATE eMouse)
	{
		_bool IsCurrent = m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
		_bool IsPrev = m_tPrevMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;

		return IsCurrent && IsPrev;
	}

	_bool Get_MouseKeyUp(MOUSEKEYSTATE eMouse)
	{
		_bool IsCurrent = m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
		_bool IsPrev = m_tPrevMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;

		return IsCurrent && !IsPrev;
	}
#pragma endregion



	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pJoystick = { nullptr };

	
private:
	// 키부터.
	_byte					m_byPrevKeyState[256] = {};
	_byte					m_byKeyState[256] = {};	


	DIMOUSESTATE			m_tPrevMouseState = {};
	DIMOUSESTATE			m_tMouseState = {};
	

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free(void);

};

NS_END
#endif // InputDev_h__



