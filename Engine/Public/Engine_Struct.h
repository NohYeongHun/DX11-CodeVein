#ifndef Engine_Struct_h__
#define Engine_Struct_h__



namespace Engine
{

	typedef struct tagFaceIndices
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES;

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

	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "PSIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOINT;

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

#pragma region PARTICLE INSTANCE STRUCTS
	typedef struct tagVertexInstanceMesh
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;
	}VTXINSTANCE_MESH;

	typedef struct tagVertexInstanceParticle
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;

		XMFLOAT2	vLifeTime;
	}VTXINSTANCE_PARTICLE;


	typedef struct tagVertexInstanceDirectionParticle
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;
		XMFLOAT2	vLifeTime;
		XMFLOAT3	vDir;
		float		fDirSpeed;
	}VTXINSTANCEDIR_PARTICLE;

	typedef struct tagVertexInstancePointDirectionParticle
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;
		XMFLOAT2	vLifeTime;
		XMFLOAT3	vDir;
		float		fDirSpeed;
		unsigned int iMaskTextureIndex;
	}VTXINSTANCEPOINTDIR_PARTICLE;

	typedef struct tagVertexParticle
	{
		static const unsigned int	iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}VTXPARTICLE;

	typedef struct tagVertexPointParticle
	{
		static const unsigned int	iNumElements = { 6 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}VTXPOINTPARTICLE;


	typedef struct tagVertexPointDirParticle
	{
		static const unsigned int	iNumElements = { 9 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, 84, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXINDEX", 0, DXGI_FORMAT_R32_UINT, 1, 88, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
	}VTXPOINTDIRPARTICLE;
#pragma endregion

	

	/* KEYFRAME에 대한 정보. */
	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;

		float		fTrackPosition;
	}KEYFRAME;


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

	typedef struct tagTriggerMonsterDesc
	{
		_float3  vTriggerPos;      // 트리거 발동 위치.
		_float   fRadius;          // 트리거 반지름 범위.
		_wstring strTriggerLayer; // 어떤 Layer에서 객체를 꺼낼건지?
		_wstring strObjectLayer;  // 어떤 Layer로 추가해줄 것인지?
		_uint    iCount;	      // 얼마나 꺼낼건지?
		_int     iFlag = -1;      // 0 : 초기, 1 : 실행 완료, -1 : 오류
	}TRIGGER_MONSTER_DESC;


	// GPU 버퍼 인스턴싱에 사용되는 데이터
	typedef struct tagParticleData
	{
		// 필수 데이터
		_float3  vPosition;      // 현재 파티클의 3D 월드 위치
		_float3  vDir;			 // 방향 구별.
		_float   fSpeed;		 // 속도 구별
		_float2  vLife;          // 남은 수명 (0이 되면 파티클 소멸)

		// 시각적 표현을 위한 데이터
		_float  fLifeRatio;	    // 현재 수명 Ratio
		_float4 vColor;         // 현재 색상 (RGBA)
		_float  fCurrentSize;   // 현재 프레임의 실제 크기 (보간된 값)
		_float  fStartSize;     // 이 파티클이 태어날 때 정해진 시작 크기
		_float  fEndSize;       // 이 파티클이 죽을 때 도달할 끝 크기
	} PARTICLE_DATA;
}

#include "Event_Type.h"

#endif // Engine_Struct_h__
