#pragma once

NS_BEGIN(Client)
class CSwordWind final : public CPartObject
{
public:
    enum ESTATE { STATE_STAY, STATE_MOVE,  STATE_DECREASE, STATE_END };

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagSwordWindDesc : public CPartObject::GAMEOBJECT_DESC
    {
        class CGameObject* pOwner = { nullptr };
        
        LEVEL eCurLevel = {}; // 현재 레벨
        MESH_SHADERPATH eShaderPath = { MESH_SHADERPATH::NONE };
        _uint iTextureIndexArray[TEXTURE::TEXTURE_END] = {};
        _float fDisplayTime = {};
        _float3 vStartScale = {}; // 초기 Scale.
    }SWORDWIND_DESC;

    /* POLL에서 Actviate 상태로 변할때 지정될 값들? */
    typedef struct tagSwordWindActivateDesc
    {
        LEVEL eCurLevel = { LEVEL::END };

        _float3 vColor = {}; // 추가할 컬러.
        _float fEmissiveIntensity = {}; // Emissive 강도
        _float3 vStartPos = {};
        _float3 vMoveDirection = {}; 
        _float3 vStartScale = {};

        _float fMoveDuration = {}; // 모든 성장이 일어나는데 걸리는 총 시간.
        _float fStayDuration = {}; // 유지 시간.
        _float fDecreaseDuration = {}; // 모든 감소가 일어나는데 걸리는 총 시간.


        _float fDissolveThreshold = {}; // 디졸브 임계값;
        _float fCreateTime = {}; // 생성 시간.
		_uint iMeshIndex = {}; // 몇번째 메시인지.
        
        const _float4x4* pSocketMatrix = { nullptr };
        class CTransform* pTargetTransform = { nullptr };
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
    class CTexture* m_pDistortionTexture = { nullptr };
    _uint m_iTextureIndexArray[TEXTURE_END] = {};
    
    class CTransform* m_pTargetTransform = { nullptr };
    const _float4x4* m_pSocketMatrix = { nullptr };


    

#pragma endregion

#pragma region 회전 형태를 잡는데 사용하는 변수들
private:
    _float m_fRotationSpeed = {};
    _float3 m_vStartScale = {};
    _float3 m_vTargetScale = {};
    _float3 m_vMoveDirection = {}; // 이동 방향이자 회전축.
    _float3 m_vStartPos = {};
    
    _vector m_vTargetPos = {};

    // ... 기존 다른 변수들 ...
    _float m_fRotationAngle = { 0.f };    // ⭐ 2. 현재 회전 각도 변수 추가!
    _float m_fStartAngle = { 0.f };       // ⭐ 3. 시작 각도를 저장할 변수 추가!
    _vector m_vPrevPos = {};              // ⭐ 4. 이전 프레임 위치 저장 변수 추가!
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
    _bool m_IsActive = false;

    // 타이머
    //_float m_fDisplayTime = 1.0f;        // 표시 시간 (초)
    _float m_fDisplayTime = {};        // 테스트용 표시 시간 (초)
    _float m_fDuration = {};
    

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;
#pragma endregion

#pragma region HELPER 함수
private:
    ESTATE m_eState = { ESTATE::STATE_END };
    // --- 성장 애니메이션용 변수 ---
    _bool  m_IsGrowing = false;   // 애니메이션이 진행 중인지 여부
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

    _float m_fMoveTime = {};
    
    _float m_fStayDuration = 0.f;      // 대기 시간
    _float m_fMoveDuration = 0.f;      // 이동 시간.
    _float m_fDecreaseDuration = 0.f;

    _float3 m_vRotation = {};
    _float3 m_vRotationAxis = {};

private:

    void Update_Stay(_float fTimeDelta);
    void Update_RotateMove(_float fTimeDelta);
    
    void Update_Decrease(_float fTimeDelta); // x,z만 감소.
    
    
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

