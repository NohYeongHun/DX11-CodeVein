#pragma once

NS_BEGIN(Engine)
/* 실체가 있는 클래스. */
class ENGINE_DLL CParticleSystem final : public CEffect
{
public:
	enum PARTICLE_TYPE : _uint
	{
		PARTICLE_TYPE_DEFAULT = 0,
		PARTICLE_TYPE_QUEEN_WARP = 1,
		PARTICLE_TYPE_BOSS_EXPLOSION = 2,
		PARTICLE_TYPE_END
	};

	enum TEXTURE_TYPE : _uint
	{
		TEXTURE_DIFFUSE = 0
		, TEXTURE_GRADIENT
		, TEXTURE_GRADIENT_ALPHA
		, TEXTURE_MASK, TEXTURE_NOISE
		, TEXTURE_END
	};

public:
	/* 클론시에 한번만 사용하고 계속 들고 있는 정보*/
	typedef struct tagParticleSystemCloneDesc : public CEffect::EFFECTCLONE_DESC
	{
		_wstring strPoolTag = {};

		// Ready_Components에 필요한 모든 정적 정보
		_uint iComponentPrototypeLevel;
		_wstring strShaderPrototypeTag;
		_wstring strTexturePrototypeTag[TEXTURE_END];
		/* 사용할 Shader 정보 */
		_uint iShaderPath = {};

		/* 사용할 Texture 정보 */
		
		_bool   useTextureCheckArray[TEXTURE_END]; // 사용하지 않을 Texture들 지정하기.
		_uint   useTextureIndexArray[TEXTURE_END]; // 사용할 텍스쳐 번호 지정.


		/* 기본적인 Particle 정점 구조 정보.*/
		_float2 vLifeTime; // x Min, y Max => 전체 시스템의 수명 시간.
		_float2 vSpeed;    // x Min, y Max
		_float2 vSizeStart;     // x Min, y Max
		_float2 vSizeEnd;     // x Min, y Max
		_float4 vColorStart;
		_float4 vColorEnd;
		_uint   iEmitCount = 100; // 기본 100개 
		_bool isLoop;

		/* Enitter 정보 => 추후 프리셋으로 정의. */
		EMISSION_TYPE eEmissionType = EMITTER_CONTINUOUS;  // 연속 생성? 한 번에 생성?
		EMITTER_SHAPE eEmitterShape = EMITTER_POINT;      // 어떤 모양으로 퍼져나갈까?
		_uint   iEmissionRate = 100;    // 연속 생성 시 1초에 생성할 개수
		_uint   iBurstCount = 50;       // 한 번에 생성 시 터져나올 개수
		_float3 vShapeSize = { 1.f, 1.f, 1.f }; // 방출 형태의 크기 (박스 크기, 구 반지름(x로 사용) 등)
		_float  fConeAngle = 45.f;
	} PARTICLESYSTEM_CLONE_DESC;

	// CPU에서 관리하는 데이터들.
	// Activate 시 정의되는 데이터들.
	typedef struct tagParticleSystemActivateDesc : public CEffect::EFFECTACITVATE_DESC
	{

	}PARTICLESYSTEM_ACTIVATE_DESC;
public:
	

#pragma region 0. 기본 함수들

private:
	explicit CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CParticleSystem(const CParticleSystem& Prototype);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion

#pragma region 파티클 업데이트 로직

private:
	void Update_Particles_Logic(_float fTimeDelta);
#pragma endregion





#pragma region 1. POOLING 고려
public:
	virtual void OnMoved_ToObjectManager() override; // 객체마다 다르게 정의해야함.
	virtual void OnActivate(void* pArg) override; // 활성화 상태일때 처리할 로직
	virtual void OnDeActivate() override;		  // 비활성화 상태일때 처리할 로직

public:
	void Calc_Timer(_float fTimeDelta);
	void Reset_Timer() { m_fLifeTime = 0.f;  }
#pragma endregion


#pragma region 소유해야 하는 멤버 변수들
private:
	class CVIBuffer_Particle_Instance* m_pVIBufferCom = { nullptr }; // VIBuffer
	class CParticleEmitter* m_pParticleEmitter = { nullptr }; // Emitter VIBuffer 생성.
	class CShader* m_pShaderCom = { nullptr }; // Shader Component 필요;
	_uint m_iShaderPath = {};

	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr }; // Texture Component 필요;
	_uint m_iTextureIndexArray[TEXTURE_TYPE::TEXTURE_END] = {};

	_float m_fLifeTime = {};          // 시스템 전체의 수명 타이머
	_float m_fLifeMaxTime = {};
	PARTICLESYSTEM_ACTIVATE_DESC m_tDesc = {}; // 파티클 정의 내용.

	PARTICLESYSTEM_CLONE_DESC m_tLayOutDesc = {}; // 파티클 정의 내용.
	vector<PARTICLE_DATA> m_Particles = {};

	const _wstring m_PoolTag = {};
#pragma endregion

#pragma region 0. 기본 함수들.
private:
	HRESULT Ready_Components(PARTICLESYSTEM_CLONE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static CParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion


};
NS_END

