#pragma once

NS_BEGIN(Client)
class CEffectParticle final : public CGameObject
{
public:
    enum PARTICLE_TYPE : _uint
    {
        PARTICLE_TYPE_DEFAULT = 0 ,
        PARTICLE_TYPE_QUEEN_WARP = 1,
        PARTICLE_TYPE_EXPLOSION = 2,
        PARTICLE_TYPE_BOSS_EXPLOSION = 3,
        PARTICLE_TYPE_QUEEN_PARTICLE = 4,
        PARTICLE_TYPE_HIT_PARTCILE = 5,
        PARTICLE_TYPE_PLAYERHIT_PARTCILE = 6,
        PARTICLE_TYPE_TORNADO = 7,
        PARTICLE_TYPE_END
    };

    enum PARTICLE_SHADER
    {
        PARTICLE_SHADER_DEFAULT = 0,
        PARTICLE_SHADER_END,
    };
    
public:
    typedef struct tagEffectParticleEnterDesc
    {
        // 시작 위치
        _vector vStartPos = {};
        PARTICLE_INIT_INFO particleInitInfo;
        class CTransform* pTargetTransform = nullptr; // ★ 추적할 대상의 Transform 추가
        _vector         vObjectDirection = {};
        // Spawn Type
        _bool  IsSpawn = {};
        _float fSpawnInterval = {};
        _uint  iSpawnCount = {};
        
        _float fChaseTime = {};
        PARTICLE_TYPE eParticleType;

    }EFFECTPARTICLE_ENTER_DESC;

public:
    /* 동적으로 버퍼를 생성 해야함. */
    typedef struct tagEffectParticle: public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        // ========= VIBuffer_Point_Instance 용도 ============
        _uint			iNumInstance;
        _float3			vCenter;
        _float3			vRange;
        _float2			vSize;
        _float3			vPivot;
        _float2			vSpeed;
        _float2			vLifeTime;
        _bool			isLoop;

        // ========= 객체 용도 ============
        _vector         vPosition = {};
        _vector         vDirection = {};
       
        _bool           useTextureCheckArray[TEXTURE::TEXTURE_END]; // 사용하지 않을 Texture들 지정하기.
        _uint           useTextureIndexArray[TEXTURE::TEXTURE_END]; // 사용할 텍스쳐 번호 지정.
        _uint           iShaderPath = {};
        _bool           isBillBoard = { false };

        // 대상 객체의 TransformMatrix 필요할 듯.


        // =========== PARTICLE_TYPE ===========
        PARTICLE_TYPE eParticleType;

        _uint iSpawnCount = 100;



    }EFFECT_PARTICLE_DESC;


private:
    explicit CEffectParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CEffectParticle(const CEffectParticle& Prototype);
    virtual ~CEffectParticle() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();


public:
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; }


public:
    // 파티클 생성 함수들
    void CreateDefault_Particle(_float3 vCenterPosition, _float3 vBaseDirection, _float fLifeTime = 3.0f);
    void CreateBurstEffect(_float3 vGatherPoint, _float3 vUpDirection, _float fGatherTime = 1.5f, _float fBurstTime = 2.0f, _float fTotalLifeTime = 5.0f);
    void Create_QueenKnightWarpEffect(const PARTICLE_INIT_INFO particleInitInfo);
    void Create_QueenKnightWarpEffect_Limited(const PARTICLE_INIT_INFO particleInitInfo, _uint iSpawnCount);
    void Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fGatherTime, _float fExplosionTime, _float fTotalLifeTime);
    void Create_ExplosionParticle(_float3 vNomalDir, _float3 vCenterPos, _float fRadius, _float fExplosionTime, _float fTotalLifeTime); // 2D

    void Create_HitParticle(_float3 vCenterPos, _float fRadius, _float fTotalLifeTime);
    void Create_PlayerHitParticle(_float3 vCenterPos, _float fRadius, _float fTotalLifeTime);
    void Create_TornadoParticle(_float3 vCenterPos, _float fRadius, _float fHeight, _float fLifeTime);


#pragma region 
    //void Create_QueenKnightParticle();
#pragma endregion


    // 타이머 기반 파티클 생성 설정
    void Set_SpawnSettings(_float fInterval, _uint iCount, _bool bContinuous = true);
    void Start_ContinuousSpawn();
    void Stop_ContinuousSpawn();

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::PARTICLE;

#pragma region POOLING
public:
    virtual void OnActivate(void* pArg) override;
    virtual void OnDeActivate() override;
#pragma endregion


private:
    // 컴포넌트
    
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    //class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
    class CVIBuffer_PointParticleDir_Instance* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_bActive = false;

    _bool m_isLoop = { false };
    _bool m_isBillBoard = { false };

    _vector m_vHitDirection = {};
    _vector m_vObjectDirection = {};
    _float3 m_vScale = {};
    _float m_fDisplayTime = {};        // 표시 시간 (초)
    _float2 m_vLifeTime = {};
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    _float m_fRotationAngle = 0.0f;      // Z축 회전 각도 (라디안)
    _uint m_iShaderPath = {}; // Shader Path.

    _bool  m_bDirectionCalculated = false; // 방향이 계산되었는지 여부 => 한번만 계산.


    _uint m_iTextureIndexArray[TEXTURE_END] = {};

    _float m_fBloomIntensity = { 2.f };

    PARTICLE_TYPE m_eParticleType = { PARTICLE_TYPE::PARTICLE_TYPE_END };

    // 소유하는 값
    class CTransform* m_pTargetTransform = { nullptr };

    // 타이머 기반 파티클 생성
    _float m_fSpawnTimer = 0.0f;          // 파티클 생성 타이머
    _float m_fSpawnInterval = 0.2f;       // 파티클 생성 간격 (초)
    _uint m_iSpawnCount = 3;              // 한번에 생성할 파티클 개수
    _bool m_bContinuousSpawn = false;     // 연속 생성 여부


    _float m_fEmissiveIntencity = {};
    // 파티클 생성.
    _float m_fChaseTime = {};

private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components(const EFFECT_PARTICLE_DESC* pDesc);
    HRESULT Ready_VIBuffer_Point(const EFFECT_PARTICLE_DESC* pDesc);

public:
    static CEffectParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#pragma region DEBUG 용도

#pragma endregion

#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG

};
NS_END

