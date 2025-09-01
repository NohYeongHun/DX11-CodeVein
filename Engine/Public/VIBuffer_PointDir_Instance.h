#pragma once

NS_BEGIN(Engine)
struct ParticleVertexInfo
{
	_float3 pos;
	_float3 dir;
	_float lifeTime;
	_float3 initialPos;     // 초기 위치 (모이는 지점)
	_float3 burstDir;       // 터질 때의 방향
	_float fBurstTime;      // 터지는 시간
};


class ENGINE_DLL CVIBuffer_PointDir_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagPointDirInstanceDesc : CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_float3 vDir; // 어떤 방향을 기준으로 움직일 것인지를 제공받음.
		_bool isLoop;
	}POINTDIR_INSTANCE_DESC;

private:
	CVIBuffer_PointDir_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_PointDir_Instance(const CVIBuffer_PointDir_Instance& Prototype);
	virtual ~CVIBuffer_PointDir_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
	void Update(_float fTimeDelta);

public:
	void Create_Buffer();


#pragma region POOLING
public:
	void PrepareParticle(_float3 vPos, _float3 vDir, _float fLifeTime);
	void CreateAllParticles(_float3 vCenterPos, _float3 vBaseDir, _float fLifeTime = 3.0f);
	void CreateBurstParticles(_float3 vGatherPoint, _float3 vUpDir, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime);
	_uint Get_LiveParticleCount() const { return static_cast<_uint>(m_LiveParticleIndices.size()); }
	_uint Get_DeadParticleCount() const { return static_cast<_uint>(m_DeadParticleIndices.size()); }

private:
	queue<_uint> m_DeadParticleIndices;
	list<_uint> m_LiveParticleIndices;
	queue<pair<_uint, ParticleVertexInfo>> m_ReadyparticleIndices;
#pragma endregion



private:
	_float3	m_vPivot = {};
	_float3 m_vDir = {}; // 방향성.
	_bool	m_isLoop = {};



	

public:
	static CVIBuffer_PointDir_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

