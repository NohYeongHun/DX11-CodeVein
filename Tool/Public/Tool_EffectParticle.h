#pragma once
NS_BEGIN(Tool)
class CTool_EffectParticle final : public CGameObject
{
public:
    enum PARTICLE_TYPE : _uint
    {
        PARTICLE_TYPE_DEFAULT = 0 ,
        PARTICLE_TYPE_QUEEN_WARP = 1,
        PARTICLE_TYPE_BOSS_EXPLOSION = 2,
        PARTICLE_TYPE_SWIRL = 3,
        PARTICLE_TYPE_END
    };

    enum PARTICLE_SHADER
    {
        PARTICLE_SHADER_DEFAULT = 0,
        PARTICLE_SHADER_QUEENKNIGHT_WARP = 1,
        PARTICLE_SHADER_END,
    };
    

public:
    /* 동적으로 버퍼를 생성 해야함. */
    typedef struct tagToolEffectParticle: public CGameObject::GAMEOBJECT_DESC
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
        _vector         vParticleDir = {};

        // ========= 객체 용도 ============
        _vector         vPosition = {};
        _vector         vObjectDir = {};
        _bool           useTextureCheckArray[TEXTURE::TEXTURE_END]; // 사용하지 않을 Texture들 지정하기.
        _uint           useTextureIndexArray[TEXTURE::TEXTURE_END]; // 사용할 텍스쳐 번호 지정.
        _uint           iShaderPath = {};
        _bool           isBillBoard = { false };
        
        _bool           isSpawn = {};
        _uint           iSpawnCount = {};
        _float          fSpawnInterval = {};
        
        // ======== 다른 객체 용도 ============
        _float fRadius;
        _float fHeight;
        _float2 vRotationSpeed = {};
        _float2 vInwardSpeed = {};
        _float fStartAlpha;
        _float fEndAlpha;

        // =========== PARTICLE_TYPE ===========
        _float fEmissiveIntencity = {};
        PARTICLE_TYPE eParticleType;
    }TOOLEFFECT_PARTICLE_DESC;


private:
    explicit CTool_EffectParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CTool_EffectParticle(const CTool_EffectTexture& Prototype);
    virtual ~CTool_EffectParticle() = default;

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
    void Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fExplosionTime, _float fTotalLifeTime);
    void Create_SwirlParticle(const ParticleSwirlInfo particleTestInfo);

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::PARTICLE;

private:
    _float3 m_vDir = {};


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
    _float3 m_vScale = {};
    _float m_fDisplayTime = {};        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    _float m_fRotationAngle = 0.0f;      // Z축 회전 각도 (라디안)
    _uint m_iShaderPath = {}; // Shader Path.

    _bool  m_bDirectionCalculated = false; // 방향이 계산되었는지 여부 => 한번만 계산.


    _uint m_iTextureIndexArray[TEXTURE_END] = {};


    PARTICLE_TYPE m_eParticleType = { PARTICLE_TYPE::PARTICLE_TYPE_END };

    _uint m_iSpawnCount = {};
    _float m_fSpawnInterval = {};
    _float m_fSpawnTime = {};
    _bool m_IsSpawn = { false };

    _float m_fEmissiveIntencity = {};
    

private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components(const TOOLEFFECT_PARTICLE_DESC* pDesc);
    HRESULT Ready_VIBuffer_Point(const TOOLEFFECT_PARTICLE_DESC* pDesc);

public:
    static CTool_EffectParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
NS_END

