#include "Effect_Wind.h"
#pragma region 기본 함수들
CEffect_WindCircle::CEffect_WindCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CEffect_WindCircle::CEffect_WindCircle(const CEffect_WindCircle& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CEffect_WindCircle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_WindCircle::Initialize_Clone(void* pArg)
{
    EFFECTWIND_CIRCLE_DESC* pDesc = static_cast<EFFECTWIND_CIRCLE_DESC*>(pArg);
    // 기본 Transform 값 처리.
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->fSpeedPerSec = 10.f;


    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
        CRASH("Failed Clone Transform Info");

    Ready_Components(pDesc);

    Ready_PartObjects();

    
    return S_OK;
}

void CEffect_WindCircle::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CEffect_WindCircle::Update(_float fTimeDelta)
{
    CContainerObject::Update(fTimeDelta);

    // 생명 주기 관리
    Calc_Timer(fTimeDelta);

    Effect_TriggerCheck(fTimeDelta);
    
}

void CEffect_WindCircle::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

    //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
    //    return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC_UI, this)))
        return;

#ifdef _DEBUG
    // Collider Rendering
    
#endif // _DEBUG

}

HRESULT CEffect_WindCircle::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
#endif // DEBUG

    return S_OK;
}

#pragma endregion


#pragma region 풀링 준비
void CEffect_WindCircle::OnActivate(void* pArg)
{
    EFFECTWIND_CIRCLE_ACTIVATE_DESC* pDesc = static_cast<EFFECTWIND_CIRCLE_ACTIVATE_DESC*>(pArg);
    ASSERT_CRASH(pDesc);

    /* 값 채워주기 */
    m_eCurLevel = pDesc->eCurLevel;
    m_ActivateDesc = *pDesc;
    m_fDisplayTime = pDesc->fDisplayTime;
    m_vStartPos = pDesc->vStartPos;
    m_vStartScale = pDesc->vStartScale;
    m_vTargetScale = pDesc->vTargetScale;
    m_pTargetWorldMatrix = pDesc->pWorldMatrix; // 한번만따라오는데
    m_pSocketMatrix = pDesc->pSocketMatrix;
    
    Reset_Timer();

    
    m_iWindCount = pDesc->iWindCount;
    m_fCreateDelay = pDesc->fCreateDelay;
   
 

    _wstring strComTag = {};
    for (_uint i = 0; i < m_iWindCount; ++i)
    {
        strComTag = TEXT("Com_SwordWind") + to_wstring(i);
        m_EffectTrigger.emplace_back(EFFECTTRIGGER{ m_fCreateDelay * (i + 1), false, strComTag});
    }
    


}

// 풀에 다시 넣지 말고 삭제해버리자. 
// => Pooling에 충분할정도로 많이 넣어두고 풀에 안넣고 오브젝트 매니저에서 삭제 객체에 넣기.
void CEffect_WindCircle::OnDeActivate()
{
    m_IsDestroy = true;
}

void CEffect_WindCircle::Initialize_EffectTrigger(const _wstring& strTag)
{
    CSwordWindCircle::SWORDWIND_CIRCLE_ACTIVATE_DESC ActivateDesc{};
    ActivateDesc.eCurLevel = m_eCurLevel;

    // ⭐ 검풍 효과의 생명주기 및 크기 설정
    //ActivateDesc.fStayDuration = 0.0f;     // 유지 시간
    //ActivateDesc.fMoveDuration = 0.7f;     // 나타나며 커지는 시간
    //ActivateDesc.fDecreaseDuration = 0.2f; // 사라지는 시간

    ActivateDesc.fDisplayTime = m_fDisplayTime;
    ActivateDesc.vStartPos = m_vStartPos;

    // 바인딩할 소켓과 Transform이 필요함. => 매프레임 업데이트
    ActivateDesc.pSocketMatrix = m_pSocketMatrix;
    ActivateDesc.pTargetWorldMatrix = m_pTargetWorldMatrix;
    ActivateDesc.vStartScale = m_vStartScale;
    ActivateDesc.vTargetScale = m_vTargetScale;

    if (m_iCurrentWind < m_vecSwordWinds.size())
    {
        m_vecSwordWinds[m_iCurrentWind]->OnActivate(&ActivateDesc);
        m_iCurrentWind++;
    }

}

void CEffect_WindCircle::Effect_TriggerCheck(_float fTimeDelta)
{
    for (auto& Trigger : m_EffectTrigger)
    {
        if (Trigger.fTriggerTime <= m_fCurrentTime && Trigger.bIsTriggered == false)
        {
            // 트리거 발동
            Trigger.bIsTriggered = true;
            // 트리거에 필요한 정보를 채워서 OnActivate 호출
            Initialize_EffectTrigger(Trigger.strPartObjectTag);
        }
    }
}

void CEffect_WindCircle::Calc_Timer(_float fTimeDelta)
{
    if (m_fCurrentTime < m_fDisplayTime)
        m_fCurrentTime += fTimeDelta;

    /* 그냥 지우자 ~ Pooling 많이 만들고. */
    if (m_fCurrentTime >= m_fDisplayTime)
    {
        m_IsDestroy = true; 
    }
       
    
}

#pragma endregion



#pragma region 기본 준비 함수들

HRESULT CEffect_WindCircle::Ready_Components(EFFECTWIND_CIRCLE_DESC* pDesc)
{

    return S_OK;
}

HRESULT CEffect_WindCircle::Ready_PartObjects()
{

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CSwordWindCircle::SWORDWIND_CIRCLE_DESC WindDesc{};
    WindDesc.pOwner = this;
    WindDesc.eCurLevel = m_eCurLevel;
    WindDesc.eShaderPath = MESH_SHADERPATH::SWORD_CIRCLE;

    // 텍스처 인덱스 설정
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_OTHER] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_NOISE] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_SWIRL] = 0;

    _wstring strComTag = TEXT("Com_SwordWind");
    m_vecSwordWinds.resize(5); // 여러개의.. 검풍 최대 20개?

    for (_uint i = 0; i < m_vecSwordWinds.size(); ++i)
    {
        // Com Tag 다르게 설정
        _wstring strUniqueTag = strComTag + to_wstring(i);

        WindDesc.fDisplayTime = m_fDisplayTime;
        
        // 기본 크기는 OnActivate에서 설정
        _float3 vScale = { 1.0f, 1.0f, 1.0f };
        
        
        XMStoreFloat3(&WindDesc.vStartScale, XMLoadFloat3(&vScale));

        if (FAILED(CContainerObject::Add_PartObject(strUniqueTag,
            ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SwordWindCircle"),
            reinterpret_cast<CPartObject**>(&m_vecSwordWinds[i]), &WindDesc)))
        {
            CRASH("Failed Create SwordWindCircle");
            return E_FAIL;
        }
    }

    return S_OK;
}

#pragma endregion


#ifdef _DEBUG
void CEffect_WindCircle::ImGui_Render()
{

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Effect Wind Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    ImGui::Text("Effect Wind Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

    ImGui::End();
}
#endif // _DEBUG





CEffect_WindCircle* CEffect_WindCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_WindCircle* pInstance = new CEffect_WindCircle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_WindCircle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_WindCircle::Clone(void* pArg)
{
    CEffect_WindCircle* pInstance = new CEffect_WindCircle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_WindCircle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_WindCircle::Free()
{
    CContainerObject::Free();
	for (auto& pSwordWind : m_vecSwordWinds)
		Safe_Release(pSwordWind);
    
}
