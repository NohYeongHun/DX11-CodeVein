#pragma once
NS_BEGIN(Client)
/* 몬스터가 소유하고 있다가 출력 되게? */
class CSlash final : public CGameObject
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
        _vector vHitDirection = {};
        _vector vHitPosition = {};
        _float  fDisPlayTime = {};
        _float3 vScale = { 1.5f, 0.2f, 1.f };
        _float3 vTargetScale = {};
        _float fCreateDelay = { 0.f };
        _bool  bIsScaleChange = { false };
        EFFECTPOSTEX_SHADERPATH eShaderPath = { EFFECTPOSTEX_SHADERPATH::MONSTER_LINESLASH };
    }SLASHACTIVATE_DESC;

private:
    explicit CSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CSlash(const CSlash& Prototype);
    virtual ~CSlash() = default;

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
#pragma endregion


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
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    // LockOn 관련
    CGameObject* m_pTarget = { nullptr };
    _bool m_IsActive = false;

    _bool m_IsScaleChange = { false };
    
    // 타이머
    _float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간
    _float m_fCreateDelay = {}; // 생성 대기시간.
    // 회전 정보
    _float m_fRotationAngle = 0.0f; // Z축 회전 각도 (라디안)
    _bool m_IsDirectionCalculated = false; // 방향이 계산되었는지 여부
    _vector m_vHitDirection = {};
    _float3 m_vScale = {};

    _float3 m_vTargetScale = {};
    _uint m_iShaderPath = {};

    _float m_fBloomIntensity = 5.f;


private:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

public:
    static CSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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

