#pragma once

NS_BEGIN(Client)
class CSwordWind final : public CPartObject
{
public:
    enum ESTATE { STATE_GROW, STATE_STAY, STATE_DECREASE, STATE_END };

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagBloodPillarADesc : public CPartObject::GAMEOBJECT_DESC
    {
        class CGameObject* pOwner = { nullptr };
        LEVEL eCurLevel = {}; // 현재 레벨
        MESH_SHADERPATH eShaderPath = { MESH_SHADERPATH::NONE };
        _uint iTextureIndexArray[TEXTURE::TEXTURE_END] = {};
        _float fDisplayTime = {};
        _float3 vStartScale = {}; // 초기 Scale.
    }SWORDWIND_DESC;

    /* POLL에서 Actviate 상태로 변할때 지정될 값들? */
    typedef struct tagPillarAEnterDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        const _float4x4* pParentMatrix = { nullptr }; // 장착할 부모 행렬.
        _float3 vColor = {}; // 추가할 컬러.
        _float fEmissiveIntensity = {}; // Emissive 강도
        _float3 vStartPos = {}; // 시작 포지션.
        _float3 vStartScale = {}; // 시작 스케일;
        _float fGrowDuration = {}; // 모든 성장이 일어나는데 걸리는 총 시간.
        _float fStayDuration = {}; // 유지 시간.
        _float fDecreaseDuration = {}; // 모든 감소가 일어나는데 걸리는 총 시간.
        _float3 vStartRotation = {}; // 시작 회전.
        _float3 vRotationAxis = {}; // 회전 축.
        _float fRotationSpeed = {}; // 회전 속도


        _float fDissolveThreshold = {}; // 디졸브 임계값;
        _float fCreateTime = {}; // 생성 시간.
		_uint iMeshIndex = {}; // 몇번째 메시인지.
        
        CTransform* pTargetTransform = { nullptr };
    }SWORDWIND_ACTIVATE_DESC;

private:
    explicit CSwordWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CSwordWind(const CSwordWind& Prototype);
    virtual ~CSwordWind() = default;

#pragma region  기본 함수

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Distortion() override;
    
#pragma endregion


#pragma region 풀링 전용 함수
public:
    virtual void OnActivate(void* pArg) override;
    virtual void OnDeActivate() override;

private:
    SWORDWIND_ACTIVATE_DESC m_ActivateDesc = {};
#pragma endregion

#pragma region 사용하는 컴포넌트
private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    class CShader* m_pDistortionShaderCom = { nullptr };
    _uint m_iShaderPath = {};


    

    class CLoad_Model* m_pModelCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    _uint m_iTextureIndexArray[TEXTURE_END] = {};
    
    class CTransform* m_pTargetTransform = { nullptr };


    

#pragma endregion

#pragma region 회전 형태를 잡는데 사용하는 변수들
private:
    _float m_fRotationSpeed = {};
    _float3 m_vStartScale = {};
#pragma endregion


#pragma region SHADER 변수들
private:
    _float2 m_vTexcoord = {};
    _float m_fTime = {}; // C++에서 계속 증가하는 시간 값을 전달받을 변수
    _float m_fCreateTime = {};
    _float m_fScrollSpeed = 2.f; // 흐르는 속도를 제어할 변수
    _float m_fDissolveTime = 0.f;

    _float m_fDissolveThreshold = {};

	_float4x4 m_CombinedWorldMatrix = {};
    _vector     m_InitialQuaternion = XMQuaternionIdentity(); // 초기 X축 -50도 회전 저장
    _float      m_fAccumulatedRotation = 0.f;
#pragma endregion


#pragma region 기본 변수들
public:
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; }
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
    ESTATE m_eState = { ESTATE::STATE_END };
    // --- 성장 애니메이션용 변수 ---
    _bool  m_bIsGrowing = false;   // 애니메이션이 진행 중인지 여부
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

    _float m_fGrowTime = {};
    
    _float m_fGrowDuration = 0.f;      // 총 성장 시간
    _float m_fStayDuration = 0.f;      // 총 성장 시간
    _float m_fDecreaseDuration = 0.f;

    _float3 m_vRotation = {};
    _float3 m_vRotationAxis = {};

private:
    void Shape_Control(_float fTimeDelta);
    void Update_Grow(_float fTimeDelta);
    void Update_Stay(_float fTimeDelta);
    void Update_Decrease(_float fTimeDelta); // x,z만 감소.
    
    void RotateTurn_ToAxis(_float fTimeDelta);
    
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
    HRESULT Ready_Components(SWORDWIND_DESC* pDesc);
#pragma endregion






#pragma endregion



public:
    static CSwordWind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG
};
NS_END

