#pragma once

NS_BEGIN(Engine)
/* 
* 파티클 시스템의 입자들을 관리하는 주체.
*/
class CParticleEmitter final : public CBase
{
public:
	typedef struct tagParticleEmitterDesc
	{
		EMISSION_TYPE eEmissionType = EMITTER_CONTINUOUS;  // 연속 생성? 한 번에 생성?
		EMITTER_SHAPE eEmitterShape = EMITTER_POINT;      // 어떤 모양으로 퍼져나갈까?
		_uint   iEmissionRate = 100;    // 연속 생성 시 1초에 생성할 개수
		_uint   iBurstCount = 50;       // 한 번에 생성 시 터져나올 개수
		_float3 vShapeSize = { 1.f, 1.f, 1.f }; // 방출 형태의 크기 (박스 크기, 구 반지름(x로 사용) 등)
		_float  fConeAngle = 45.f; // 콘(Cone) 형태일 경우 추가 정보
	}EMITTER_DESC;
private:
	CParticleEmitter();
	virtual ~CParticleEmitter() = default;

#pragma region 0. 기본 함수들
public:
	virtual HRESULT Initialize(const EMITTER_DESC* pDesc);
#pragma endregion

#pragma region 1. 고유 함수들
public:
	// CParticleSystem이 풀에서 활성화(OnActivate)될 때마다 호출.
	// 소유자의 정보(DESC, Transform)를 다시 연결하고 타이머 등을 리셋합니다.
	void Reset(const CParticleSystem::PARTICLESYSTEM_CLONE_DESC* pOwnerDesc, class CTransform* pOwnerTransform);

	// CParticleSystem의 Update에서 매 프레임 호출될 핵심 함수.
	// 타이머를 체크해서 생성할 시간이 되면 파티클 데이터를 만들어 out_Particles 벡터에 추가합니다.
	void Emit(_float fTimeDelta, vector<PARTICLE_DATA>& out_Particles);

	// 파티클 생성을 멈추고 싶을 때 호출.
	void Stop();
#pragma endregion


#pragma region  소유할 멤버변수들

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	// 생성된 파티클의 속성을 결정하기 위해, 소유자인 CParticleSystem의 DESC를 가리키는 포인터
	const CParticleSystem::PARTICLESYSTEM_CLONE_DESC* m_pOwnerDesc = { nullptr };

	// 파티클을 어디에 생성할지 알기 위해, 소유자인 CParticleSystem의 Transform을 가리키는 포인터
	class CTransform* m_pOwnerTransform = { nullptr };

	// --- Emitter 자체의 동작을 위한 변수들 ---
	_float                  m_fEmitTimer = { 0.f };     // 파티클 생성 간격 계산을 위한 타이머
	_float                  m_fEmitInterval = { 0.f };  // 1초에 몇 개를 생성할지에 대한 실제 시간 간격
	_uint                   m_iBurstCount = { 0 };      // 한번에 터뜨릴 파티클 개수
	_bool					m_bHasBursted = { false };
	// Emitter의 고유 속성
	EMITTER_DESC            m_tDesc = {};
#pragma endregion


#pragma region 헬퍼 함수들
private:
	void Create_One_Particle(vector<PARTICLE_DATA>& out_Particles);
	_float3 Create_Random_Direction();
	void Generate_Particle_Attribute(_float3& out_vPosition, _float3& out_vDirection);
#pragma endregion




#pragma region 0. 기본 함수들.

public:
	static CParticleEmitter* Create(const EMITTER_DESC* pDesc);
	virtual void Free() override;
#pragma endregion


};
NS_END

