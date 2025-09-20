#pragma once

NS_BEGIN(Client)
class CEffect_PlayerSkill final : public CContainerObject
{

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagEffectPlayerSkillDesc : public CContainerObject::GAMEOBJECT_DESC
    {
    }EFFECT_PLAYERSKILL_DESC;

    typedef struct tagPillarActivateDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        _float3 vStartPos = {}; // 시작 포지션.
        _float fDuration = {};  // 총 객체 수명.
        const _float4x4* pParentMatrix = { nullptr };
		class CTransform* pTargetTransform = { nullptr };
    }EFFECT_PLAYERSKILL_ACTIVATE_DESC;

private:
	struct EFFECTTRIGGER
    {
		_float fTriggerTime = { 0.f }; // 몇 초에 발동할 것인가.
        _bool bIsTriggered = { false };
		const _wstring strPartObjectTag = { L"" }; // 어떤 PartObject를 활성화 시킬 것인가.
    };

private:
    explicit CEffect_PlayerSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CEffect_PlayerSkill(const CEffect_PlayerSkill& Prototype);
    virtual ~CEffect_PlayerSkill() = default;

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

#pragma region 특정 실행 함수.
private:
    void Effect_TriggerCheck(_float fTimeDelta);
    void Initialize_EffectTrigger(const _wstring& strTag);

private:
	vector<EFFECTTRIGGER> m_EffectTrigger = {}; // 트리거

    
#pragma endregion







#pragma endregion



#pragma region 사용하는 컴포넌트

#pragma endregion

#pragma region 사용하는 멤버 변수
public:
    // 타이머 리셋
    void Reset_Timer() { m_fTime = 0.0f; }
    void Calc_Timer(_float fTimeDelta);


private:
    LEVEL m_eCurLevel = { LEVEL::END };
    _bool m_bActive = false;

    _float m_fTime = {};     // 현재 진행 시간
    _float m_fDuration = {}; // 총 진행 시간
    EFFECT_PLAYERSKILL_ACTIVATE_DESC m_ActivateDesc = {};

    // PartObject들
private:
    class CEffect_FloorAura* m_pFloorAura = { nullptr };
    class CEffect_BodyAura* m_pBodyAura = { nullptr };
    class CEffect_BodyAura* m_pBodyAura_Second = { nullptr };
    const _float4x4* m_pParentMatrix = { nullptr };
    class CTransform* m_pTargetTransform = { nullptr };
    _float3 m_vStartPos = {};
    



public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;

#pragma endregion



#pragma region 기본 준비 함수들
private:
    HRESULT Ready_Components(EFFECT_PLAYERSKILL_DESC* pDesc);
    HRESULT Ready_PartObjects();
#pragma endregion


#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG



#pragma endregion



public:
    static CEffect_PlayerSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};
NS_END

