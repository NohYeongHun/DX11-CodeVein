#ifndef Engine_Struct_h__
#define Engine_Struct_h__



namespace Engine
{

	typedef struct tagEngineDesc
	{
		HINSTANCE		hInst;
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;	

	typedef struct tagLightDesc
	{
		enum TYPE { DIRECTIONAL, POINT, END };

		TYPE		eType;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

	}LIGHT_DESC;

	/* 기본 스탯 구조체 */
	typedef struct tagDefaultMonsterStat
	{
		_float fMaxHP;
		_float fAttackPower;
		_float fDetectionRange;
		_float fAttackRange;
		_float fMoveSpeed;
		_float fRotationSpeed;
	}MONSTER_STAT;

	typedef struct tagRayCastHit
	{
		_float3 vHitPoint; // Transform 데이터
		_float3 vHitLocal; // Local 정점 위치.
		_float3 vHitNormal;
		_float  fDistance;
		class CGameObject* pHitObject = nullptr;
	}RAYHIT_DESC;

#pragma region NAVIGATION 저장용 구조체
	typedef struct tagCellSaveDesc
	{
		_float3 vPointA;
		_float3 vPointB;
		_float3 vPointC;
	}CELLSAVE_DESC;

	typedef struct tagNavigationSaveDesc
	{
		uint32_t iCellCount;
		vector<CELLSAVE_DESC> Cells;
	}NAVIGATIONSAVE_DESC;
#pragma endregion


	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;

		const static _uint iNumElements = { 1 };
		constexpr static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

	}VTXPOS;


	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		const static _uint iNumElements = { 2 };
		constexpr static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

	}VTXPOSTEX;

	typedef struct tagVertexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXCUBE;
	
	typedef struct tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		const static _uint iNumElements = { 3 };
		constexpr static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	}VTXNORTEX;

	typedef struct tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;

		const static _uint iNumElements = { 5 };
		constexpr static D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	}VTXMESH;

	/* KEYFRAME에 대한 정보. */
	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;

		float		fTrackPosition;
	}KEYFRAME;

	typedef struct tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		/* 정점에게 적용되야할 뼈들의 인덱스*/
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXANIMMESH;


	typedef struct ScopedTimer {
		const char* tag;
		LARGE_INTEGER  start, freq;
		ScopedTimer(const char* t) : tag(t) {
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&start);
		}
		~ScopedTimer() {
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			double ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
			printf("%s : %.3f ms\n", tag, ms);
		}
	}SCOPE_TIMER;

	typedef struct BlendDesc
	{
		_bool isBlending = false;
		_float fElapsed = 0.f;
		_float fBlendDuration = 2.f;

		uint32_t iPrevAnimIndex = 0;
		_float fPrevAnimTime = 0.f;

		uint32_t iNextAnimIndex = 0;
		_matrix matPrevRoot;
		class CLoad_Animation* pLoad_Animation = { nullptr };
		_bool bScale = { true };
		_bool bRotate = { true };
		_bool bTranslate = { true };
		
	}BLEND_DESC;

	// Animation Chnage Desc
	typedef struct tagChangeAnimationDesc
	{
		_uint	iNextAnimIndex;
		_uint	iStartFrame;
		_float	fChangeTime = 0.f;
		_float	fChangeDuration = 0.f;
	}CHANGEANIMATION_DESC;

	typedef struct tagBoundingBox
	{
		_float3 vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
		_float3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		_float3 vCenter = { 0.f, 0.f, 0.f };
		_float3 vExtents = { 0.f, 0.f, 0.f };
		_float fHeight = 0.f;
	}BOUNDING_BOX;


	typedef struct ModelPickingInfo {
		_bool bHit = false;
		_float fDistance = FLT_MAX;
		_float3 vHitPoint;
		_float3 vHitWorldPoint;
		_float3 vHitNormal;

		// 모델 계층 정보
		class CGameObject* pHitObject;  // 히트된 게임오브젝트
		_float3 vTriangleVertices[3]; // 히트된 삼각형의 3개 정점
						
	} MODEL_PICKING_INFO;
}

#include "Event_Type.h"

#endif // Engine_Struct_h__
