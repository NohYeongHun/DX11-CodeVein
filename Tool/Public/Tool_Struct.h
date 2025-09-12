#pragma once

NS_BEGIN(Tool)
enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, OBJECTEDIT, MAPEDIT, MONSTER, MAIN, MAIN2, MAIN3, END };

typedef struct tagBaseStructDesc
{
	LEVEL eLevel = { LEVEL::END };
}BASE_DESC;

typedef struct tagStaticTileDescTEMP_DESC : public BASE_DESC
{
	_float3 vScale = { 1.f, 1.f, 1.f };
	_float3 vRotate = { 0.f, 0.f, 0.f };
	_float3 vPos = { 0.f, 0.f, 0.f };
	_int iIndex = { 0 };
} STATIC_TILE_DESC;


typedef struct tagPlayerStructInfo
{
	_uint iMaxHp;
	_uint iHp;
	_uint iCoin;
	_uint iScore;
}PLAYER_DATA_INFO;

typedef struct tagHierarchyObjectDesc
{
	class CGameObject* pGameObject = { nullptr };
	const char* szName = {}; // 이름.
}HIERARCHY_OBJECT_DSEC;


typedef struct tagModelCreateDesc
{
	const _tchar* pModelTag;
	_float4 vPosition = {}; // 일단은 위치만..
	_float3 vRotate = {};
	_float3 vScale = {};
	_uint iShaderPath = {};
	_bool isColor = {};
	_float4 vTintColor = {};

	_float fFadeOutStartTime = {};
	_float fFadeOutDuration = {};
}MODEL_CREATE_DESC;

typedef struct ParticleInitInfo
{
	_float3 pos;
	_float3 dir;
	_float lifeTime;

	_float3 vCenterPos;
	_float  fRadius;
	_float fExplosionTime;
	_float fTotalLifeTime;


}PARTICLE_INIT_INFO;


// Input 값 (생성 시 받아옴)
typedef struct ParticleTestInfo
{
	_float3 vRange;
	
	_float2 vLifeTime;
	_float3 vDirection;
	_float2 vSize;
	_float2  vLoss;
	_float2  vRandomSpeed;
	_float  fRadius;
	_float  fAlpha;
	
	
}PARTICLE_TEST_INFO;


typedef struct ParticleSwirlInfo
{

	//================================================
	// 기준 좌표계 (어디에 붙일 것인가?)
	//================================================

	// _matrix matTargetWorld;
	// 소용돌이가 생성되고 움직일 기준이 되는 월드 행렬입니다.
	// (예: 칼의 월드 행렬, 캐릭터 손의 월드 행렬 등)
	_matrix matTargetWorld;


	//================================================
	// 로컬 공간에서의 움직임 정의 (어떻게 움직일 것인가?)
	//================================================

	// _float3 vLocalRotationAxis;
	// 'matTargetWorld' 기준의 로컬 회전 축입니다.
	// 칼의 진행 방향(Look)으로 회전시키려면 (0, 0, 1)
	// 칼의 윗 방향(Up)을 축으로 회전시키려면 (0, 1, 0)을 사용합니다.
	_float3 vLocalRotationAxis;

	_float fRadius;       // 로컬 공간에서의 생성 반경
	_float fHeight;       // 로컬 공간에서의 생성 높이 (두께)
	_float2 vRotationSpeed; // 회전 속도
	_float2 vInwardSpeed;   // 중심을 향하는 속도 (양수: 모임, 음수: 퍼짐)


	//================================================
	// 외형 및 수명 (나머지는 동일)
	//================================================
	_float2 vLifeTime;
	_float2 vSize;
	_float fStartAlpha;
	_float fEndAlpha;

	//================================================
   // 스윕(Sweep) 효과를 위한 추가 변수
   //================================================

   // _float fSweepTime;
   // 효과가 한 방향에서 반대 방향으로 완전히 쓸고 지나가는 데 걸리는 시간입니다.
   // 예를 들어 2.0f 이면, 2초 동안 호(arc)가 그려집니다.
	_float fSweepTime;

	// _float fArcWidth;
	// 보여지는 호의 너비(각도)입니다. (단위: 라디안)
	// XM_PI (3.14)는 180도, XM_PIDIV2 (1.57)는 90도에 해당합니다.
	_float fArcWidth;

	// _bool bFade;
	// 호의 양 끝을 부드럽게 만들지(true), 아니면 칼같이 자를지(false) 결정합니다.
	_bool bFade;


}PARTICLE_SWIRL_INFO;


NS_END
