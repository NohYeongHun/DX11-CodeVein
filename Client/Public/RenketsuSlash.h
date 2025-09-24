#pragma once
NS_BEGIN(Client)
/* 몬스터가 소유하고 있다가 출력 되게? */
class CRenketsuSlash final : public CGameObject
{
public:
    enum TEXTURE { DIFFUSE, MASK, OTHER, TEXTURE_END };

public:
    typedef struct tagSlashEffectDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        EFFECTPOSTEX_SHADERPATH eShaderPath = { EFFECTPOSTEX_SHADERPATH::NONE };
        //POSTEX_SHADERPATH eShaderPath = {POSTEX_SHADERPATH::NONE };
    }SLASHEFFECT_DESC;

    typedef struct tagSlashActivateDesc
    {
        LEVEL eCurLevel = {LEVEL::END};
        class CTransform* pTargetTransform = { nullptr };
        _float3 vLocalDirection = {}; // 카메라 방향으로 어떻게 회전할건지? (수평 고정?)
        _vector vHitPosition = {};
        _float  fDisPlayTime = {};
        _float3 vScale = { 3.f, 0.2f, 1.f };
        _float3 vStartScale = {0.5f, 2.f, 1.f};
        _float fCreateDelay = {};
    }SLASHACTIVATE_DESC;

private:
    explicit CRenketsuSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CRenketsuSlash(const CRenketsuSlash& Prototype);
    virtual ~CRenketsuSlash() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    virtual HRESULT Render_Distortion();

#pragma region 풀링 전용 함수
public:
    virtual void OnActivate(void* pArg) override;
    virtual void OnDeActivate() override;
#pragma endregion


public:
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; }

public:
    void Initialize_Transform(SLASHACTIVATE_DESC* pDesc);

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::TEXTURE;

private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CShader* m_pDistortionShaderCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    class CTexture* m_pDistortionTexture = { nullptr };
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    // LockOn 관련
    class CTransform* m_pTargetTransform = { nullptr };
    _bool m_IsActive = false;

    _float m_fDelay = {};

    
    // 타이머
    _float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    // 회전 정보
    _float m_fRotationAngle = 0.0f; // Z축 회전 각도 (라디안)
    _bool m_IsDirectionCalculated = false; // 방향이 계산되었는지 여부
    _float3 m_vScale = {};
    _float3 m_vStartScale = {};
    _float3 m_vLocalDirection = {}; // 회전 방향.
    _uint m_iShaderPath = {};

    _float m_fBloomIntensity = 5.f;


private:
    HRESULT Bind_ShaderResources();
    HRESULT Bind_DistortionShaderResources();
    HRESULT Ready_Components();

public:
    static CRenketsuSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#pragma region DEBUG
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG



#pragma endregion

};
NS_END

