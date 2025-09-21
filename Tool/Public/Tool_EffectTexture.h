#pragma once

/*
* 1. 이거는 Effect Texture로 제작. =>
* => 생성 방법
* => Prototype 생성
* => Clone 생성 방식 가능 => Texture니까
*/
NS_BEGIN(Tool)
class CTool_EffectTexture final : public CGameObject
{
public:
    /* 클론 시 지정할 Texture들 */
    typedef struct tagToolEffectTexture : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        _vector vHitDirection = {};
        _vector vHitPosition = {};
        _float3 vScale = { 1.f, 1.f, 1.f };
        _float  fDisPlayTime = {};

        _bool useTextureCheckArray[TEXTURE::TEXTURE_END]; // 사용하지 않을 Texture들 지정하기.
        _uint useTextureIndexArray[TEXTURE::TEXTURE_END]; // 사용할 텍스쳐 번호 지정.
        _uint iShaderPath = {}; // 선택할 Shader Path
    }TOOLEFFECT_TEXTURE_DESC;

private:
    explicit CTool_EffectTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CTool_EffectTexture(const CTool_EffectTexture& Prototype);
    virtual ~CTool_EffectTexture() = default;

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
    void Initialize_Transform();

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::TEXTURE;

private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    _uint m_iTextureIndexArray[TEXTURE_END] = {};
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_IsActive = false;

    _vector m_vHitDirection = {};
    _float3 m_vScale = {};
    _float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    _float m_fRotationAngle = 0.0f;      // Z축 회전 각도 (라디안)
    _uint m_iShaderPath = {}; // Shader Path.

    _bool  m_IsDirectionCalculated = false; // 방향이 계산되었는지 여부 => 한번만 계산.
    
    



private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components(TOOLEFFECT_TEXTURE_DESC* pDesc);

public:
    static CTool_EffectTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};
NS_END

