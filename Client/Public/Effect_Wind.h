#pragma once

NS_BEGIN(Client)
class CEffect_Wind final : public CContainerObject
{

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagEffectWindDesc : public CContainerObject::GAMEOBJECT_DESC
    {
    }EFFECTWIND_DESC;

    typedef struct tagPillarActivateDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        _float3 vStartPos = {}; // 시작 포지션.
        _float fDuration = {}; // 모든 성장이 일어나는데 걸리는 총 시간.
        _float3 vStartRotation = {}; // 시작 회전.
		_float3 vRotationAxis = {}; // 회전 축.
        const _float4x4* pSocketMatrix = { nullptr };
		class CTransform* pTargetTransform = { nullptr };
    }EFFECTWIND_ACTIVATE_DESC;

private:
    struct SwordWindEvent
    {
        _bool bIsActive = { false };
        _float fWindEventTime = {};
    };

private:
    explicit CEffect_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CEffect_Wind(const CEffect_Wind& Prototype);
    virtual ~CEffect_Wind() = default;

#pragma region  기본 함수

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




#pragma endregion



#pragma region 사용하는 컴포넌트

#pragma endregion

#pragma region 사용하는 멤버 변수
public:
    // 타이머 리셋
    void Reset_Timer() { m_fCurrentTime = 0.0f; }
    void Calc_Timer(_float fTimeDelta);


private:
    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_IsActive = false;

    _float m_fCurrentTime = {};
    _float m_fDuration = {};
    EFFECTWIND_ACTIVATE_DESC m_ActivateDesc = {};
    _float3 m_vStartRotation = {};
    _float3 m_vRotationAxis = {};
    

    // PartObject들
private:
    vector<class CSwordWind*> m_vecSwordWinds;
	

    vector<SwordWindEvent> m_vecSwordWindEvents;
    const _float4x4* m_pSocketMatrix = { nullptr };
    class CTransform* m_pTargetTransform = { nullptr };
    _float3 m_vStartPos = {};
    



public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;

#pragma endregion



#pragma region 기본 준비 함수들
private:
    HRESULT Ready_Components(EFFECTWIND_DESC* pDesc);
    HRESULT Ready_PartObjects();
#pragma endregion


#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG



#pragma endregion



public:
    static CEffect_Wind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};
NS_END

