#pragma once
NS_BEGIN(Client)
class CHitFlashEffect final : public CGameObject
{
public:
	enum TEXTURE { DIFFUSE, OPACITY, OTHER, END };


public:
    typedef struct tagHitFlashDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        EFFECTPOSTEX_SHADERPATH eShaderPath = { EFFECTPOSTEX_SHADERPATH::HITFLASH };
    }HITFLASH_DESC;

    typedef struct tagHitFlashEffectActivateDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        _vector vHitDirection = {};
        _vector vHitPosition = {};
        _float  fDisPlayTime = {};
        _float3 vScale = { 1.5f, 1.5f, 1.f };
    }HITFLASHACTIVATE_DESC;

private:
    explicit CHitFlashEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CHitFlashEffect(const CHitFlashEffect& Prototype);
    virtual ~CHitFlashEffect() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

#pragma region 풀링 전용 함수
public:
    virtual void OnActivate(void* pArg) override;
    virtual void OnDeActivate() override;

private:
    // 이펙트 생존 시간. 타이머
    _float m_fDisplayTime = 10.0f;        // 테스트용 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

#pragma endregion


public:
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; }

public:
    void Initialize_Transform();

private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    
#pragma region 쉐이더 전달 변수
    _uint m_iCurFrame = {}; // 16개의 프레임이 존재.
    _uint m_iShaderPath = {};
#pragma endregion

    // 회전 정보
    _bool m_bDirectionCalculated = false; // 방향이 계산되었는지 여부
    _vector m_vHitDirection = {};
    _float3 m_vScale = {};

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::TEXTURE;


private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

public:
    static CHitFlashEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
NS_END

