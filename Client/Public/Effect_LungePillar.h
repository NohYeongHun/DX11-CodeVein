#pragma once

NS_BEGIN(Client)
class CEffect_LungePillar final : public CContainerObject
{

public:
    /* 클론시 전달할 정보들 */
    typedef struct tagEffectLungePillarDesc : public CContainerObject::GAMEOBJECT_DESC
    {
    }EFFECT_LUNGEPILLARDESC;

    typedef struct tagLungePillarActivateDesc
    {
        LEVEL eCurLevel = { LEVEL::END };
        _vector vStartPos = {}; // 시작 포지션.
        _float fGrowDuration = {};
        _float fStayDuration = {};
        _float fDecreaseDuration = {};
        _float fTargetRadius = { 2.f };

    }LUNGEPILLAR_ACTIVATE_DESC;



private:
    explicit CEffect_LungePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CEffect_LungePillar(const CEffect_LungePillar& Prototype);
    virtual ~CEffect_LungePillar() = default;

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


#pragma region ATTACK POWER

public:
    // 데미지 
    const _float Get_AttackPower() { return m_fAttackPower; }
#pragma endregion



#pragma endregion

#pragma region 생명주기와 관련된 멤버변수
private:
    _float m_fGrowDuration = {};
    _float m_fStayDuration = {};
    _float m_fDecreaseDuration = {};
#pragma endregion



#pragma region 사용하는 컴포넌트
private:
    class CCollider* m_pColliderCom = { nullptr };

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
    _float m_fAttackPower = {};
    _float m_fTargetRadius = { 6.f };
    LUNGEPILLAR_ACTIVATE_DESC m_ActivateDesc = {};


    // PartObject들
private:
    class CLunge_Pillar* m_pLungePillar = { nullptr };



public:
    static const EFFECTTYPE EffectType = EFFECTTYPE::MESH;

#pragma endregion



#pragma region 기본 준비 함수들
private:
    HRESULT Ready_Components(EFFECT_LUNGEPILLARDESC* pDesc);
    HRESULT Ready_PartObjects();
#pragma endregion


#pragma region IMGUI
#ifdef _DEBUG
private:
    void ImGui_Render();
#endif // _DEBUG



#pragma endregion



public:
    static CEffect_LungePillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};
NS_END

