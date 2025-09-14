#pragma once

NS_BEGIN(Client)
class CBlood_PillarC final : public CPartObject
{
public:
    /* 클론시 전달할 정보들 */
    typedef struct tagBloodPillarADesc : public CPartObject::GAMEOBJECT_DESC
    {
        
        const _float4x4* pParentMatrix = { nullptr }; // 장착할 부모 행렬.
        class CGameObject* pOwner = { nullptr };
        LEVEL eCurLevel = {}; // 현재 레벨
        MESH_SHADERPATH eShaderPath = { MESH_SHADERPATH::NONE };
        _float fDisplayTime = {};
        _uint iTextureIndexArray[TEXTURE::TEXTURE_END] = {};
    }BLOODPILLAR_DESC;

    /* POLL에서 Actviate 상태로 변할때 지정될 값들? */
    typedef struct tagPillarCEnterDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        _float3 vColor = {}; // 추가할 컬러.
        _float fEmissiveIntensity = {}; // Emissive 강도
        _float3 vStartPos = {}; // 시작 포지션.
        _float fStartRadius = {}; // 시작 반지름.
        _float fTargetRadius = {}; // 최종 반지름
        _float fTargetHeight = {}; // 최종 높이.
        _float fDuration = {}; // 모든 성장이 일어나는데 걸리는 총 시간.
    }PILLARC_ACTIVATE_DESC;

private:
    explicit CBlood_PillarC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CBlood_PillarC(const CBlood_PillarC& Prototype);
    virtual ~CBlood_PillarC() = default;

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
    PILLARC_ACTIVATE_DESC m_ActivateDesc = {};
#pragma endregion

#pragma region 사용하는 컴포넌트
private:
    // 컴포넌트
    class CShader* m_pShaderCom = { nullptr };
    _uint m_iShaderPath = {};
    class CLoad_Model* m_pModelCom = { nullptr };
    class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
    _uint m_iTextureIndexArray[TEXTURE_END] = {};

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
    _float m_fDisplayTime = 10.0f;        // 테스트용 표시 시간 (초)
    _float m_fCurrentTime = 0.0f;        // 현재 경과 시간

public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;
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
    HRESULT Ready_Components(BLOODPILLAR_DESC* pDesc);
#pragma endregion






#pragma endregion



public:
    static CBlood_PillarC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG
};
NS_END

