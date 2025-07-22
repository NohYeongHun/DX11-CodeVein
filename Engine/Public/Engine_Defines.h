#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
//#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>


/* DX의 Device Input 사용.*/
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Fx11/d3dx11effect.h"
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>

#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

using namespace DirectX;



#include <memory>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <codecvt>
#include <locale> 
#include <queue>

#include <thread>
#include <io.h>
#include <iostream>
#include <fstream>
#include <variant>

using namespace std;


namespace Engine
{
	// 한 메시에서 최대로 사용가능한 Bone의 개수.
	static const unsigned int g_iMaxNumBones = 512;
}


#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#include "SaveFile_Define.h"
//#include "LoadFile_Define.h"


#define CRASH(cause)                  \
{                                 \
uint32_t* crash = nullptr;            \
__analysis_assume(crash != nullptr);   \
*crash = 0xDEADBEEF;               \
}

#define ASSERT_CRASH(expr)         \
{                           \
if (!(expr))               \
{                        \
CRASH("ASSERT_CRASH");      \
__analysis_assume(expr);   \
}                        \
}


#pragma region 헬퍼 함수.

static void WriteWString(std::ofstream& ofs, const std::wstring& ws)
{
    uint32_t len = static_cast<uint32_t>(ws.size());
    ofs.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
    if (len > 0)
        ofs.write(reinterpret_cast<const char*>(ws.data()), len * sizeof(wchar_t));
}

static void WriteString(std::ofstream& ofs, const std::string& st)
{
    uint32_t len = static_cast<uint32_t>(st.size());
    ofs.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
    if (len > 0)
        ofs.write(st.c_str(), len * sizeof(char));
}

static inline bool ReadBytes(std::ifstream& ifs, void* dst, size_t bytes)
{
    ifs.read(reinterpret_cast<char*>(dst), bytes);
    return !!ifs;
}

static inline wstring ReadWString(std::ifstream& ifs)
{
    uint32_t len = 0;
    if (!ReadBytes(ifs, &len, sizeof(uint32_t)))
        return {};

    std::wstring out;
    if (len > 0)
    {
        out.resize(len);
        if (!ReadBytes(ifs, &out[0], sizeof(wchar_t) * len))
            out.clear();
    }
    return out;
}

static inline string ReadString(std::ifstream& ifs)
{
    uint32_t len = 0;
    if (!ReadBytes(ifs, &len, sizeof(uint32_t)))
        return {};

    std::string out;
    if (len > 0)
    {
        out.resize(len);
        if (!ReadBytes(ifs, out.data(), sizeof(char) * len))
        {
            out.clear();
        }
    }

    return out;
}

#pragma endregion


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif


using namespace Engine;


