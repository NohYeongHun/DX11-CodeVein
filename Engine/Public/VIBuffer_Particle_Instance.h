#pragma once
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Instance final : public CVIBuffer_Particle
{
public:
	/* 1. Prototype에 생성하는 모든 클론 객체들이 공유할 정보. */
	typedef struct tagVIBufferParticleInstanceDesc : CVIBuffer_Particle::PARTICLE_DESC
	{
		
	}PARTICLE_INSTANCE_DESC;

	/*2. 삭제된 로직 대신, 셰이더와 통신하기 위한 약속인 **입력 레이아웃(Input Layout)**을 생성하는 코드를 추가합니다.
		 먼저, 셰이더로 보낼 인스턴스 데이터 구조체를 명확히 정의합니다. */


#pragma region 0. 기본 함수들
private:
	CVIBuffer_Particle_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& Prototype);
	virtual ~CVIBuffer_Particle_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const PARTICLE_INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
#pragma endregion


#pragma region 1. 전용 함수들
public:
	void Update_Buffer(const vector<PARTICLE_DATA>& particles, // System으로부터 직접 받은 데이터
		const _matrix& finalTransformMatrix);
#pragma endregion


#pragma region 0. 소유해야하는 멤버 변수들
private:
	_uint m_iNumRenderInstance = {};
#pragma endregion


#pragma region 0. 기본 함수들
public:
	
	static CVIBuffer_Particle_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext
		, const PARTICLE_INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion
};
NS_END

