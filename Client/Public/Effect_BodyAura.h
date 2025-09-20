#pragma once

NS_BEGIN(Client)

/* 실제 동작하는 Aura */
class CEffect_BodyAura final : public CPartObject
{
public:
    enum ESTATE { STATE_GROW, STATE_STAY, STATE_DISSOLVE, STATE_END };

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagEffectBodyAuraDesc : public CPartObject::GAMEOBJECT_DESC
    {
        class CGameObject* pOwner = { nullptr };
        MESH_SHADERPATH eShaderPath = { MESH_SHADERPATH::NONE };
    }EFFECTBODYAURA_DESC;

    /* POLL에서 Actviate 상태로 변할때 지정될 값들? */
    typedef struct tagEffectBodyAuraEnterDesc
    {
        LEVEL eCurLevel = { LEVEL::END }; // 현재 레벨.
        _float3 vOffsetPos = {}; // 이동할 위치.
        _float4 vColor = {}; // 추가할 컬러.
        _float3 vStartScale = {}; // 시작 크기
        _float3 vEndScale = {}; // 끝 크기.
        _float fGrowDuration = {}; // 커지는 시간.
        _float fStayDuration = {}; // 유지 시간
        _float fDissolveDuration = {}; // 사라지는 시간
        CTransform* pTargetTransform = { nullptr };
    }EFFECTFBODYAURA_ACTIVATE_DESC;

private:
    explicit CEffect_BodyAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CEffect_BodyAura(const CEffect_BodyAura& Prototype);
    virtual ~CEffect_BodyAura() = default;

#pragma region  기본 함수

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    
#pragma endregion


#pragma region 풀링 전용 함수
public:
    virtual void OnActivate(void* pArg) override;
    virtual void OnDeActivate() override;

private:
    EFFECTFBODYAURA_ACTIVATE_DESC m_ActivateDesc = {};
#pragma endregion

#pragma region 사용하는 컴포넌트
private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    _uint m_iShaderPath = {};

    class CLoad_Model* m_pModelCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    _uint m_iTextureIndexArray[TEXTURE_END] = {};
    
    class CTransform* m_pTargetTransform = { nullptr };


    

#pragma endregion

#pragma region 모양 형태를 잡는데 사용하는 변수들
private:
    _float3 m_vStartScale = {};
    _float m_fShapeTime = {};

    _float3 m_vEndScale = {};

    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

    _float m_fGrowTime = {};

    _float m_fGrowDuration = 0.f;      // 총 성장 시간
    _float m_fStayDuration = 0.f;      // 총 성장 시간
    _float m_fDissolveDuration = 0.f;

    ESTATE m_eState = { ESTATE::STATE_END };
    // --- 성장 애니메이션용 변수 ---
    _bool  m_bIsGrowing = false;   // 애니메이션이 진행 중인지 여부

private:
    void Shape_Control(_float fTimeDelta);
    void Update_Grow(_float fTimeDelta);
    void Update_Stay(_float fTimeDelta);
    void Update_Dissolve(_float fTimeDelta);
#pragma endregion


#pragma region SHADER 변수들
private:
    _float2 m_vTexcoord = {};
    _float m_fTime = {};          // C++에서 계속 증가하는 시간 값을 전달받을 변수
    _float m_fScrollSpeed = 2.f; // 흐르는 속도를 제어할 변수
    _float m_fDissolveTime = 0.f;
    _float m_fDissolveThreshold = {};

	_float4x4 m_CombinedWorldMatrix = {};
#pragma endregion


#pragma region 기본 변수들
public:
    // 타이머 리셋
    void Reset_Timer();
    void Calc_Timer(_float fTimeDelta);

private:
    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_bActive = false;

    // 타이머
    //_float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fDisplayTime = 0.f;        // 테스트용 표시 시간 (초)
    

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;
#pragma endregion

#pragma region HELPER 함수
private:
    


#pragma endregion



#pragma region Clone시 지정할 변수들
public:
    const _float4x4* Get_CombinedWorldMatrix() { return &m_CombinedWorldMatrix; }

private:
    const _float4x4* m_pParentMatrix = { nullptr }; // 장착할 부모 행렬.
    class CGameObject* m_pOwner = { nullptr };

    
#pragma endregion




#pragma region 기본 준비 함수들
private:
    HRESULT Bind_ShaderResources();
    HRESULT Bind_ShaderResources_Distortion();
    HRESULT Ready_Components(EFFECTBODYAURA_DESC* pDesc);
#pragma endregion






#pragma endregion



public:
    static CEffect_BodyAura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG
};
NS_END

