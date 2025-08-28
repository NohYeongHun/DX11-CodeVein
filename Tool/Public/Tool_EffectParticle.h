#pragma once
#include "GameObject.h"

NS_BEGIN(Tool)
class CTool_EffectParticle final : public CGameObject
{

public:
    /* 동적으로 버퍼를 생성 해야함. */
    typedef struct tagToolEffectParticle: public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        _uint			iNumInstance;
        _float3			vCenter;
        _float3			vRange;
        _float2			vSize;
        _float3			vPivot;
        _float2			vSpeed;
        _float2			vLifeTime;
        _bool			isLoop;
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
    static const EFFECTTYPE EffectType = EFFECTTYPE::PARTICLE;

private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_bActive = false;

    _vector m_vHitDirection = {};
    _float3 m_vScale = {};
    _float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    _float m_fRotationAngle = 0.0f;      // Z축 회전 각도 (라디안)
    _uint m_iShaderPath = {}; // Shader Path.

    _bool  m_bDirectionCalculated = false; // 방향이 계산되었는지 여부 => 한번만 계산.





private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

public:
    static CTool_EffectParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
NS_END

