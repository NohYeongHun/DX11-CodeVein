#pragma once

NS_BEGIN(Engine)
struct ParticleVertexInfo
{
	_float3 pos;
	_float3 dir;
	_float lifeTime;
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

