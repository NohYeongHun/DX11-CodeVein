#include "Effect_PlayerSkill.h"
#pragma region 기본 함수들
CEffect_PlayerSkill::CEffect_PlayerSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CEffect_PlayerSkill::CEffect_PlayerSkill(const CEffect_PlayerSkill& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CEffect_PlayerSkill::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_PlayerSkill::Initialize_Clone(void* pArg)
{
    EFFECT_PLAYERSKILL_DESC* pDesc = static_cast<EFFECT_PLAYERSKILL_DESC*>(pArg);
    // 기본 Transform 값 처리.
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->fSpeedPerSec = 10.f;
    

    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
        CRASH("Failed Clone Transform Info");

    Ready_Components(pDesc);
    Ready_PartObjects();

    
    return S_OK;
}

void CEffect_PlayerSkill::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CEffect_PlayerSkill::Update(_float fTimeDelta)
{
    CContainerObject::Update(fTimeDelta);

	//Event 사용
    Effect_TriggerCheck(fTimeDelta);
    
    

    // 생명 주기 관리
    Calc_Timer(fTimeDelta);
}

void CEffect_PlayerSkill::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

#ifdef _DEBUG
    
#endif // _DEBUG

}

HRESULT CEffect_PlayerSkill::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
#endif // DEBUG

    return S_OK;
}

#pragma endregion


#pragma region 풀링에서 꺼냈을 때. 준비
void CEffect_PlayerSkill::OnActivate(void* pArg)
{
    EFFECT_PLAYERSKILL_ACTIVATE_DESC* pDesc = static_cast<EFFECT_PLAYERSKILL_ACTIVATE_DESC*>(pArg);
    ASSERT_CRASH(pDesc);

    Reset_Timer();

    /* 값 채워주기 */
    m_ActivateDesc = *pDesc;
    m_eCurLevel = pDesc->eCurLevel;
    m_pTargetTransform = pDesc->pTargetTransform; // 전달할 트랜스폼.
    m_pParentMatrix = pDesc->pParentMatrix;
    m_fDuration = m_ActivateDesc.fDuration;  // 총 수명
    m_vStartPos = pDesc->vStartPos;
    m_vScaleMultiple = pDesc->vScaleMultiple;
    /* 이거를 PlayerSkill의 Update 때 지정하는것도 괜찮아 보임 
    *  특정 시간에 저거 특정 시간에 저거 등등.
    */

    

    /* Effect Trigger 호출. */
    m_EffectTrigger.emplace_back(
        EFFECTTRIGGER{ 0.0f, false, TEXT("Com_BloodFloorAura") }
    );

    m_EffectTrigger.emplace_back(
        EFFECTTRIGGER{ 0.1f, false, TEXT("Com_BloodBodyAura") }
    );

}


    // 풀에 다시 넣지 말고 삭제해버리자. 
// => Pooling에 충분할정도로 많이 넣어두고 풀에 안넣고 오브젝트 매니저에서 삭제 객체에 넣기.
void CEffect_PlayerSkill::OnDeActivate()
{
    m_IsDestroy = true;
}

void CEffect_PlayerSkill::Effect_TriggerCheck(_float fTimeDelta)
{
    for (auto& Trigger : m_EffectTrigger)
    {
		if (Trigger.fTriggerTime <= m_fTime && Trigger.bIsTriggered == false)
		{
			// 트리거 발동
			Trigger.bIsTriggered = true;
            // 트리거에 필요한 정보를 채워서 OnActivate 호출
            Initialize_EffectTrigger(Trigger.strPartObjectTag);
		}
    }
}

void CEffect_PlayerSkill::Initialize_EffectTrigger(const _wstring& strTag)
{
    if (strTag == TEXT("Com_BloodFloorAura"))
    {
        CEffect_FloorAura::EFFECTFLOORAURA_ACTIVATE_DESC FloorAuraDesc{};
        FloorAuraDesc.eCurLevel = m_eCurLevel;
        FloorAuraDesc.vOffsetPos = m_vStartPos;
        FloorAuraDesc.vColor = { 0.f, 0.f, 0.f, 1.f };
        FloorAuraDesc.vStartScale = { 0.01f, 1.f, 0.01f };
        _float3 vEndSacle = { 0.7f, 0.7f, 0.7f };
        XMStoreFloat3(&FloorAuraDesc.vEndScale, XMLoadFloat3(&vEndSacle) * XMLoadFloat3(&m_vScaleMultiple));


        FloorAuraDesc.fGrowDuration = 0.2f;
        FloorAuraDesc.fStayDuration = 0.4f;
        FloorAuraDesc.fDissolveDuration = 0.4f;


        FloorAuraDesc.pTargetTransform = m_pTargetTransform;
        m_pFloorAura->OnActivate(&FloorAuraDesc);
    }
    else if (strTag == TEXT("Com_BloodBodyAura"))
    {
        CEffect_BodyAura::EFFECTFBODYAURA_ACTIVATE_DESC BodyAuraDesc{};
        BodyAuraDesc.eCurLevel = m_eCurLevel;
        BodyAuraDesc.vOffsetPos = m_vStartPos;
        BodyAuraDesc.vColor = { 0.f, 0.f, 0.f, 1.f };
        _float3 vStartScale = { 4.5f, 2.5f, 4.5f };
        XMStoreFloat3(&BodyAuraDesc.vStartScale, XMLoadFloat3(&vStartScale) * XMLoadFloat3(&m_vScaleMultiple));
        //BodyAuraDesc.vStartScale = { 8.f, 6.f, 8.f };
        BodyAuraDesc.fStayDuration = 0.7f;
        BodyAuraDesc.fDissolveDuration = 0.3f;
        BodyAuraDesc.vStartRotation = { 0.f, 0.f, 0.f };

        BodyAuraDesc.pTargetTransform = m_pTargetTransform;
        m_pBodyAura->OnActivate(&BodyAuraDesc);
    }

    
}




void CEffect_PlayerSkill::Calc_Timer(_float fTimeDelta)
{
    if (m_fTime < m_fDuration)
        m_fTime += fTimeDelta;

    /* 그냥 지우자 ~ Pooling 많이 만들고. */
    if (m_fTime >= m_fDuration)
    {
        m_IsDestroy = true; 
    }
       
    
}

#pragma endregion



#pragma region 기본 준비 함수들

HRESULT CEffect_PlayerSkill::Ready_Components(EFFECT_PLAYERSKILL_DESC* pDesc)
{

    return S_OK;
}

HRESULT CEffect_PlayerSkill::Ready_PartObjects()
{

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CEffect_FloorAura::EFFECTFLOORAURA_DESC BloodFloorAuraDesc{};
    _wstring strComTag = TEXT("Com_BloodFloorAura");
    BloodFloorAuraDesc.pOwner = this;
    BloodFloorAuraDesc.eShaderPath = MESH_SHADERPATH::BLOOD_CIRCLE;
    //BloodAuraDesc.eCurLevel = m_eCurLevel; => Pooling에 넣는 시점에는 지정되지 않음. => 풀링에서 빼낼때 지정.
    if (FAILED(CContainerObject::Add_PartObject(strComTag,
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodFloorAura")
        , reinterpret_cast<CPartObject**>(&m_pFloorAura), &BloodFloorAuraDesc)))
    {
        CRASH("Failed Create BloodAura");
        return E_FAIL;
    }

    CEffect_BodyAura::EFFECTBODYAURA_DESC BloodBodyAuraDesc{};
    strComTag = TEXT("Com_BloodBodyAura");
    BloodBodyAuraDesc.pOwner = this;
    BloodBodyAuraDesc.eShaderPath = MESH_SHADERPATH::BLOOD_AURA;
    //BloodAuraDesc.eCurLevel = m_eCurLevel; => Pooling에 넣는 시점에는 지정되지 않음. => 풀링에서 빼낼때 지정.
    if (FAILED(CContainerObject::Add_PartObject(strComTag,
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodBodyAura")
        , reinterpret_cast<CPartObject**>(&m_pBodyAura), &BloodBodyAuraDesc)))
    {
        CRASH("Failed Create BloodAura");
        return E_FAIL;
    }

    strComTag = TEXT("Com_BloodBodyAura2");
    BloodBodyAuraDesc.pOwner = this;
    BloodBodyAuraDesc.eShaderPath = MESH_SHADERPATH::BLOOD_AURA;
    //BloodAuraDesc.eCurLevel = m_eCurLevel; => Pooling에 넣는 시점에는 지정되지 않음. => 풀링에서 빼낼때 지정.
    if (FAILED(CContainerObject::Add_PartObject(strComTag,
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodBodyAura")
        , reinterpret_cast<CPartObject**>(&m_pBodyAura_Second), &BloodBodyAuraDesc)))
    {
        CRASH("Failed Create BloodAura");
        return E_FAIL;
    }
   

    return S_OK;
}

#pragma endregion


#ifdef _DEBUG
void CEffect_PlayerSkill::ImGui_Render()
{

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Effect PlayerAura Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    ImGui::Text("Effect PlayerAura Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

    ImGui::End();
}
#endif // _DEBUG


CEffect_PlayerSkill* CEffect_PlayerSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_PlayerSkill* pInstance = new CEffect_PlayerSkill(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_PlayerSkill"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_PlayerSkill::Clone(void* pArg)
{
    CEffect_PlayerSkill* pInstance = new CEffect_PlayerSkill(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_PlayerSkill"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_PlayerSkill::Free()
{
    CContainerObject::Free();
    Safe_Release(m_pFloorAura);
    Safe_Release(m_pBodyAura);
    Safe_Release(m_pBodyAura_Second);

   
    
}
