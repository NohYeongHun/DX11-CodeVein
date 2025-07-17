#pragma once
#include "../Default/framework.h"
#include <process.h>
#include "Tool_Struct.h"

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Tool
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	//enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Tool;


