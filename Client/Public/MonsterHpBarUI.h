#pragma once

NS_BEGIN(Client)
class CMonsterHpBar final : public CGameObject
{
public:
    typedef struct tagMonsterHpBarDesc : public CGameObject::GAMEOBJECT_DESC
    {
        LEVEL eCurLevel = { LEVEL::END };
        POSTEX_SHADERPATH eShaderPath = {};
        _float fX = 0.f;
        _float fY = 0.f;
        _float fSizeX = 64.f;
        _float fSizeY = 64.f;
        _float2 vOffset = {};
        _float3 vScale = {};
        _float fMaxHp = {};
    }MONSTERHPUI_DESC;

private:
    explicit CMonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMonsterHpBar(const CMonsterHpBar& Prototype);
    virtual ~CMonsterHpBar() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    // LockOn 대상 설정/해제

    // UI 활성화/비활성화
    void Set_Active(_bool bActive) { m_IsActive = bActive; }
    _bool Is_Active() const { return m_IsActive; }

#pragma region 위치 구하기.

public:
    void Calculate_Screen_Position(_fvector vMonsterPos);
    void Increase_Hp(_float fHp, _float fTime);
    void Decrease_HpUI(_float fHp, _float fTime);
    void Time_Calc(_float fTimeDelta);

private:
    _float4x4 m_EquipWorld = {};
    _float2 m_vOffset = {};
    _float3 m_vScale = {};
    _float m_fSizeX = {};
    _float m_fSizeY = {};

    _float m_fRightRatio = {};
    _float m_fLeftRatio = {};
    _float m_fHp = {};
    _float m_fMaxHp = {};

    _bool m_IsIncrease = {false };
    _bool m_IsDecrease = {false };
#pragma endregion



private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CTexture* m_pTextureCom = { nullptr };
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    LEVEL m_eCurLevel = { LEVEL::END };
    _uint m_iShaderPath = {};
    _bool m_IsActive = false;

    // 애니메이션
    _float m_fAnimationTime = 0.0f;
    _float m_fRotationSpeed = 2.0f;
    _float m_fScaleSpeed = 1.5f;

private:
    HRESULT Bind_ShaderResources();

public:
    static CMonsterHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG

};
NS_END

