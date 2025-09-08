#pragma once

NS_BEGIN(Client)

class CVIBuffer_Particle_Instance final : public Engine::CVIBuffer_Instance
{
public:
	typedef struct tagParticleInstanceDesc : Engine::CVIBuffer_Instance::INSTANCE_DESC
	{
		_uint iMaxParticles = 100;
	}PARTICLE_INSTANCE_DESC;

private:
	CVIBuffer_Particle_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& Prototype);
	virtual ~CVIBuffer_Particle_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

public:
	void Update_Buffer(const vector<Engine::PARTICLE_DATA>& particles, _matrix transformMatrix);

private:
	void Create_Buffer();
	
private:
	_uint m_iMaxParticles = 100;
	_uint m_iCurrentParticleCount = 0;

public:
	static CVIBuffer_Particle_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END