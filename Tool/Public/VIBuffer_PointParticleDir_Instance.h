#pragma once

NS_BEGIN(Tool)

// 생성된 파티클에 추가적으로 부여할 정보들.
typedef struct ParticleVertexInfo
{
	_float3 pos;
	_float3 dir;
	_float lifeTime;
	_float3 initialPos;     // 초기 위치 (모이는 지점)
	_float3 burstDir;       // 터질 때의 방향
	_float fBurstTime;      // 터지는 시간
	_float fRandomSpeed;    // 랜덤 스피드.
	
	// TestParticle에 사용할 정보.
	_float3 vParticleDir;
	_float3 vStartPos;
	_float fLifeTime;
	_float fRadius;
	_float fRandomStartSpeed; // 시작 스피드
	_float fLoss; // 감쇠값.
	_float fSize;
	_float fAlpha;

}PARTICLE_VERTEX_INFO;

typedef struct ParticleVertexTestInfo
{
	_float3 vInitialPos; // 시작 위치
	_float  fLifeTime;
}PARTICLE_VERTEX_TESTINFO;


class CVIBuffer_PointParticleDir_Instance final : public Engine::CVIBuffer_Instance
{
public:
	enum class PARTICLE_TYPE : _uint
	{
		PARTICLE_TYPE_DEFAULT = 0,
		PARTICLE_TYPE_QUEENKNGIHT_WARP = 1,
		PARTICLE_TYPE_BOSS_EXPLOSION = 2,
		PARTICLE_TYPE_TEST = 3,
		PARTICLE_TYPE_END,
	};

public:
	typedef struct tagParticlePointDirInstanceDesc : Engine::CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_float3 vDir; // 어떤 방향을 기준으로 움직일 것인지를 제공받음.
		_bool isLoop;
		PARTICLE_TYPE eParticleType;

		_bool IsSpawn = { false };
		_uint iSpawnCount = {};
		_float fSpawnInterval = {};

	}PARTICLEPOINTDIR_INSTANCE_DESC;

#pragma region 기본 함수들
private:
	CVIBuffer_PointParticleDir_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_PointParticleDir_Instance(const CVIBuffer_PointParticleDir_Instance& Prototype);
	virtual ~CVIBuffer_PointParticleDir_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
	void Create_Buffer();
#pragma endregion

#pragma region 0. 부모로 부터 실시간으로 받아올 정보들.
public:
	void Bind_Transform(_fvector vPos);

private:
	_float4 m_vParentPos = {};
#pragma endregion


#pragma region 1. PARTICLE_TYPE에 따른 다른 형태의 파티클 업데이트 설정.
public:
	void Update(_float fTimeDelta);
	void Default_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta);
	void QueenKnightWarp_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta);
	void BossExplosion_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta);
	void TestParticle_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta);

public:
	void CreateAllParticles(_float3 vCenterPos, _float3 vBaseDir, _float fLifeTime = 3.0f);
	void CreateBurstParticles(_float3 vGatherPoint, _float3 vUpDir, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime);
	void Create_QueenKnightWarpParticle(const PARTICLE_INIT_INFO particleInitInfo);
	void Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fExplosionTime, _float fTotalLifeTime);
	void Create_TestParticle(const PARTICLE_TEST_INFO particleInitInfo);


	/* 스폰 전용*/


#pragma endregion



#pragma region POOLING
	
	_uint Get_LiveParticleCount() const { return static_cast<_uint>(m_LiveParticleIndices.size()); }
	_uint Get_DeadParticleCount() const { return static_cast<_uint>(m_DeadParticleIndices.size()); }

private:
	queue<_uint> m_DeadParticleIndices;
	list<_uint> m_LiveParticleIndices;
	queue<pair<_uint, PARTICLE_VERTEX_INFO>> m_ReadyparticleIndices;
#pragma endregion



private:
	_float3	m_vPivot = {};
	_float3 m_vDir = {}; // 방향성.
	_float3 m_vRange = {}; // 파티클 범위
	_float2 m_vLifeTime = {}; // 파티클 생명시간 범위 (min, max)
	_bool	m_isLoop = {};
	PARTICLE_TYPE m_eParticleType = { PARTICLE_TYPE::PARTICLE_TYPE_END };

	_uint m_iSpawnCount = {};
	_bool m_isSpawn = {};
	_float m_fSpawnInterval = {};
	_float m_fSpawnTime = {};
	_float2 m_vSpeed = {};

#ifdef _DEBUG
	_float m_fDebugTime = { 0.f };
#endif // _DEBUG

	



public:
	static CVIBuffer_PointParticleDir_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

