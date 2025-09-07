#pragma once
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle abstract : public CVIBuffer // 인스턴스 형식.
{
public:
	/* 1. Prototype에 생성하는 모든 클론 객체들이 공유할 정보. */
	typedef struct tagVIBufferParticleDesc 
	{
		_uint   iMaxNumInstance;
	}PARTICLE_DESC;

	/*2. 삭제된 로직 대신, 셰이더와 통신하기 위한 약속인 **입력 레이아웃(Input Layout)**을 생성하는 코드를 추가합니다.
         먼저, 셰이더로 보낼 인스턴스 데이터 구조체를 명확히 정의합니다. */




#pragma region 0. 기본 함수들
protected:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& Prototype);
	virtual ~CVIBuffer_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype(const PARTICLE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
	HRESULT Create_Buffer();
#pragma endregion


#pragma region 0. 소유해야하는 멤버 변수들
protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint		  m_iMaxNumInstance = {};
	_uint		  m_iNumIndexPerInstance = {};
	_uint		  m_iInstanceVertexStride = {};

protected:
	D3D11_BUFFER_DESC		m_VBInstanceDesc{};
	void* m_pInstanceVertices = { nullptr };
#pragma endregion



#pragma region 0. 기본 함수들
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
#pragma endregion

};
NS_END

