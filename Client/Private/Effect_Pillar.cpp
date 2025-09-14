#pragma region 기본 함수들
CEffect_Pillar::CEffect_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CEffect_Pillar::CEffect_Pillar(const CEffect_Pillar& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CEffect_Pillar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Pillar::Initialize_Clone(void* pArg)
{
    EFFECT_PILLARDESC* pDesc = static_cast<EFFECT_PILLARDESC*>(pArg);
    // 기본 Transform 값 처리.
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->fSpeedPerSec = 10.f;


    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
        CRASH("Failed Clone Transform Info");

    Ready_Components(pDesc);

    Ready_PartObjects();

    
    return S_OK;
}

void CEffect_Pillar::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CEffect_Pillar::Update(_float fTimeDelta)
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

void CEffect_Pillar::Late_Update(_float fTimeDelta)
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

HRESULT CEffect_Pillar::Render()
{
#ifdef _DEBUG
    ImGui_Render();
    m_pColliderCom->Render();
#endif // DEBUG

    return S_OK;
}

#pragma endregion


#pragma region 풀링 준비
void CEffect_Pillar::OnActivate(void* pArg)
{
    PILLAR_ACTIVATE_DESC* pDesc = static_cast<PILLAR_ACTIVATE_DESC*>(pArg);
    ASSERT_CRASH(pDesc);

    /* 값 채워주기 */
    m_eCurLevel = pDesc->eCurLevel; // Move GameObjects To Pools에서 현재 레벨이 결정된다.
    m_ActivateDesc = *pDesc;
    m_fDuration = m_ActivateDesc.fDuration;
    m_fAttackPower = pDesc->fAttackPower; // 데미지
    Reset_Timer();


    m_pTransformCom->Set_State(STATE::POSITION, m_ActivateDesc.vStartPos);

    /* 하위 객체들 Actviate 실행. */

    CBlood_PillarA::PILLARA_ACTIVATE_DESC PillarADesc{};
    PillarADesc.eCurLevel = pDesc->eCurLevel;
    PillarADesc.vColor = { 1.f, 0.f, 0.f };
    PillarADesc.fEmissiveIntensity = 0.5f;
    PillarADesc.vStartPos = { 0.f, 0.f, 0.f }; // 최종 위치는 어차피 곱해진다. => 서서히 조절.
    PillarADesc.fTargetRadius = 6.f;
    PillarADesc.fDecreaseTargetRadius = 2.f;
    PillarADesc.fTargetHeight = 5.f;
    PillarADesc.fGrowDuration = 1.f;
    PillarADesc.fStayDuration = 0.2f;
    PillarADesc.fDecreaseDuration = 0.8f;
    m_pBloodPillarA->OnActivate(&PillarADesc);
    

    CBlood_PillarB::PILLARB_ACTIVATE_DESC PillarBDesc{};
    PillarBDesc.eCurLevel = pDesc->eCurLevel;
    PillarBDesc.vColor = { 1.f, 0.f, 0.f };
    PillarBDesc.fEmissiveIntensity = 0.5f;
    PillarBDesc.vStartPos = { 0.f, 0.f, 0.f }; // 최종 위치는 어차피 곱해진다. => 서서히 조절.
    PillarBDesc.fTargetRadius = 6.f;
    PillarBDesc.fDecreaseTargetRadius = 2.f;
    PillarBDesc.fTargetHeight = 5.f;
    PillarBDesc.fGrowDuration = 1.f;
    PillarBDesc.fStayDuration = 0.2f;
    PillarBDesc.fDecreaseDuration = 0.8f;
    m_pBloodPillarB->OnActivate(&PillarBDesc);


    m_pColliderCom->Set_Active(true);

    /* Pool이 활성화될 때 등록. 
    * 원래는 Pool이 빠질때 빼는 로직도 생성해야하나 => Destroy로 해결.
    */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));
}

// 풀에 다시 넣지 말고 삭제해버리자. 
// => Pooling에 충분할정도로 많이 넣어두고 풀에 안넣고 오브젝트 매니저에서 삭제 객체에 넣기.
void CEffect_Pillar::OnDeActivate()
{
    m_IsDestroy = true;
}

void CEffect_Pillar::Calc_Timer(_float fTimeDelta)
{
    if (m_fCurrentTime < m_fDuration)
        m_fCurrentTime += fTimeDelta;

    /* 그냥 지우자 ~ Pooling 많이 만들고. */
    if (m_fCurrentTime >= m_fDuration)
    {
        m_IsDestroy = true; 
    }
       
    
}

#pragma endregion



#pragma region 기본 준비 함수들

HRESULT CEffect_Pillar::Ready_Components(EFFECT_PILLARDESC* pDesc)
{

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(5.f, 5.f, 5.f);
    OBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    OBBDesc.vCenter = _float3(0.f, 4.f, 0.0f); // 중점.

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

HRESULT CEffect_Pillar::Ready_PartObjects()
{

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CBlood_PillarA::BLOODPILLAR_DESC PillarA{};
    PillarA.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PillarA.pOwner = this;
    PillarA.eCurLevel = m_eCurLevel;
    PillarA.eShaderPath = MESH_SHADERPATH::BLOOD_PILLARA;
    PillarA.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0; // 사용할 인덱스.
    // AddRef로 참조값 1 증가.
    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_PillarA"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodPillarA")
        , reinterpret_cast<CPartObject**>(&m_pBloodPillarA), &PillarA)))
    {
        CRASH("Failed Create PillarA");
        return E_FAIL;
    }

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CBlood_PillarB::BLOODPILLAR_DESC PillarB{};
    PillarB.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PillarB.pOwner = this;
    PillarB.eCurLevel = m_eCurLevel;
    PillarB.eShaderPath = MESH_SHADERPATH::BLOOD_PILLARB;
    PillarB.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0; // 사용할 인덱스.

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_PillarB"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodPillarB")
        , reinterpret_cast<CPartObject**>(&m_pBloodPillarB), &PillarB)))
    {
        CRASH("Failed Create PillarB");
        return E_FAIL;
    }

    /////* Clone시 지정될 값들 => 초기 설정 값들.*/
    //CBlood_PillarC::BLOODPILLAR_DESC PillarC{};
    //PillarC.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    //PillarC.pOwner = this;
    //PillarC.eCurLevel = m_eCurLevel;
    //PillarC.eShaderPath = MESH_SHADERPATH::BLOOD_PILLARC;
    //PillarC.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0; // 사용할 인덱스

    //if (FAILED(CContainerObject::Add_PartObject(TEXT("Com_PillarC"),
    //    ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BloodPillarC")
    //    , reinterpret_cast<CPartObject**>(&m_pBloodPillarC), &PillarC)))
    //{
    //    CRASH("Failed Create PillarC");
    //    return E_FAIL;
    //}

    return S_OK;
}

#pragma endregion


#ifdef _DEBUG
void CEffect_Pillar::ImGui_Render()
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





CEffect_Pillar* CEffect_Pillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Pillar* pInstance = new CEffect_Pillar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Pillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Pillar::Clone(void* pArg)
{
    CEffect_Pillar* pInstance = new CEffect_Pillar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_Pillar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Pillar::Free()
{
    CContainerObject::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pBloodPillarA);
    Safe_Release(m_pBloodPillarB);
    //Safe_Release(m_pBloodPillarC);
    
}
