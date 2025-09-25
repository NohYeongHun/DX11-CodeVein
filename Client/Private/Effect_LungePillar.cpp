#pragma region 기본 함수들
CEffect_LungePillar::CEffect_LungePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CEffect_LungePillar::CEffect_LungePillar(const CEffect_LungePillar& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CEffect_LungePillar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_LungePillar::Initialize_Clone(void* pArg)
{
    EFFECT_LUNGEPILLARDESC* pDesc = static_cast<EFFECT_LUNGEPILLARDESC*>(pArg);
    // 기본 Transform 값 처리.
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->fSpeedPerSec = 10.f;


    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
        CRASH("Failed Clone Transform Info");

    Ready_Components(pDesc);
    Ready_PartObjects();

    
    return S_OK;
}

void CEffect_LungePillar::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CEffect_LungePillar::Update(_float fTimeDelta)
{
    CContainerObject::Update(fTimeDelta);

    if (m_pColliderCom)
    {
        _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        m_pColliderCom->Update(WorldMatrix);
    }

    // 생명 주기 관리
    Calc_Timer(fTimeDelta);
}

void CEffect_LungePillar::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    // Collider Rendering
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
    {
        CRASH("Failed Add DebugComponent");
        return;
    }
#endif // _DEBUG

}

HRESULT CEffect_LungePillar::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
    //m_pColliderCom->Render();
#endif // DEBUG

    return S_OK;
}

#pragma endregion


#pragma region 풀링 준비
void CEffect_LungePillar::OnActivate(void* pArg)
{
    LUNGEPILLAR_ACTIVATE_DESC* pDesc = static_cast<LUNGEPILLAR_ACTIVATE_DESC*>(pArg);
    ASSERT_CRASH(pDesc);

    /* 값 채워주기 */
    m_eCurLevel = pDesc->eCurLevel;
    m_ActivateDesc = *pDesc;
    m_fGrowDuration = pDesc->fGrowDuration;
    m_fStayDuration = pDesc->fStayDuration;
    m_fDecreaseDuration = pDesc->fDecreaseDuration;
    m_fDuration = m_fGrowDuration + m_fStayDuration + m_fDecreaseDuration + 0.1f; 
    m_fTargetRadius = pDesc->fTargetRadius;
    Reset_Timer();

    m_pTransformCom->Set_State(STATE::POSITION, m_ActivateDesc.vStartPos);

    /* 하위 객체들 Activate 실행 */
    CLunge_Pillar::LUNGEPILLAR_ACTIVATE_DESC PillarDesc{};
    PillarDesc.vStartPos = { 0.f, 0.f, 0.f }; // 최종 위치는 어차피 곱해진다. => 서서히 조절.
    PillarDesc.fTargetRadius = m_fTargetRadius;
    PillarDesc.fDecreaseTargetRadius = 0.1f; // 감소
    PillarDesc.fTargetHeight = 4.f;
    PillarDesc.fGrowDuration = m_fGrowDuration;
    PillarDesc.fStayDuration = m_fStayDuration;
    PillarDesc.fDecreaseDuration = m_fDecreaseDuration;
    m_pLungePillar->OnActivate(&PillarDesc);


    /* Effect 파티클 활성화 */
    
    //m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));
}

// 풀에 다시 넣지 말고 삭제해버리자. 
// => Pooling에 충분할정도로 많이 넣어두고 풀에 안넣고 오브젝트 매니저에서 삭제 객체에 넣기.
void CEffect_LungePillar::OnDeActivate()
{
    m_IsDestroy = true;
}

void CEffect_LungePillar::Calc_Timer(_float fTimeDelta)
{
    if (m_fCurrentTime < m_fDuration)
        m_fCurrentTime += fTimeDelta;

    /* 그냥 지우자 ~ Pooling 많이 만들고. */
    if (m_fCurrentTime >= m_fDuration * 0.8f && m_pColliderCom->Is_Active() == true)
        m_pColliderCom->Set_Active(false);

    if (m_fCurrentTime >= m_fDuration)
    {
        m_IsDestroy = true; 
    }
       
    
}

#pragma endregion



#pragma region 기본 준비 함수들

HRESULT CEffect_LungePillar::Ready_Components(EFFECT_LUNGEPILLARDESC* pDesc)
{

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(m_fTargetRadius * 0.7f, m_fTargetRadius, m_fTargetRadius * 0.7f);
    OBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    OBBDesc.vCenter = _float3(0.f, m_fTargetRadius + 0.7f, 0.0f); // 중점.

    OBBDesc.pOwner = this;
    OBBDesc.eCollisionType = CCollider::COLLISION_TRIGGER;
    OBBDesc.eMyLayer = CCollider::MONSTER_SKILL;
    OBBDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON |
        CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_OBB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
    {
        CRASH("Failed Clone Collider OBB");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffect_LungePillar::Ready_PartObjects()
{

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CLunge_Pillar::LUNGEPILLAR_DESC PillarDesc{};
    PillarDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PillarDesc.pOwner = this;
    PillarDesc.eCurLevel = m_eCurLevel;
    PillarDesc.eShaderPath = MESH_SHADERPATH::LUNGE_PILLAR;
    PillarDesc.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0; // 사용할 인덱스.
    // AddRef로 참조값 1 증가.
    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_Pillar"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LungePillar")
        , reinterpret_cast<CPartObject**>(&m_pLungePillar), &PillarDesc)))
    {
        CRASH("Failed Create Pillar");
        return E_FAIL;
    }

    return S_OK;
}

#pragma endregion


#ifdef _DEBUG
void CEffect_LungePillar::ImGui_Render()
{

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Effect Pillar Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    ImGui::Text("Pillar Pos %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);

    ImGui::End();
}
#endif // _DEBUG





CEffect_LungePillar* CEffect_LungePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_LungePillar* pInstance = new CEffect_LungePillar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_LungePillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_LungePillar::Clone(void* pArg)
{
    CEffect_LungePillar* pInstance = new CEffect_LungePillar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_LungePillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_LungePillar::Free()
{
    CContainerObject::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pLungePillar);
    
}
