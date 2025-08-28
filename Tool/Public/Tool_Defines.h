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

	// PrototypeName.cpp와 연계되어있음.
	enum TEXTURE { TEXTURE_DIFFUSE, TEXTURE_GRADIENT, TEXTURE_GRADIENT_ALPHA, TEXTURE_MASK, TEXTURE_NOISE, TEXTURE_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Tool;


