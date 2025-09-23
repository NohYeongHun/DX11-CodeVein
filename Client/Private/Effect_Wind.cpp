#pragma region 기본 함수들
CEffect_Wind::CEffect_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CEffect_Wind::CEffect_Wind(const CEffect_Wind& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CEffect_Wind::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Wind::Initialize_Clone(void* pArg)
{
    EFFECTWIND_DESC* pDesc = static_cast<EFFECTWIND_DESC*>(pArg);
    // 기본 Transform 값 처리.
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->fSpeedPerSec = 10.f;


    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
        CRASH("Failed Clone Transform Info");

    Ready_Components(pDesc);

    Ready_PartObjects();

    
    return S_OK;
}

void CEffect_Wind::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CEffect_Wind::Update(_float fTimeDelta)
{
    CContainerObject::Update(fTimeDelta);

    // 생명 주기 관리
    Calc_Timer(fTimeDelta);
}

void CEffect_Wind::Late_Update(_float fTimeDelta)
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

HRESULT CEffect_Wind::Render()
{
#ifdef _DEBUG
    //ImGui_Render();
#endif // DEBUG

    return S_OK;
}

#pragma endregion


#pragma region 풀링 준비
void CEffect_Wind::OnActivate(void* pArg)
{
    EFFECTWIND_ACTIVATE_DESC* pDesc = static_cast<EFFECTWIND_ACTIVATE_DESC*>(pArg);
    ASSERT_CRASH(pDesc);

    /* 값 채워주기 */
    m_eCurLevel = pDesc->eCurLevel;
    m_ActivateDesc = *pDesc;
    m_fDuration = m_ActivateDesc.fDuration;
    m_vStartPos = pDesc->vStartPos;
    Reset_Timer();

    CSwordWind::SWORDWIND_ACTIVATE_DESC ActivateDesc{};
    ActivateDesc.eCurLevel = m_eCurLevel;

    // ⭐ 검풍 효과의 생명주기 및 크기 설정
    ActivateDesc.fMoveDuration = 0.3f;     // 나타나며 커지는 시간
    ActivateDesc.fStayDuration = 0.2f;     // 유지 시간
    ActivateDesc.fDecreaseDuration = 0.4f; // 사라지는 시간
    ActivateDesc.vStartScale = { 17.0f, 17.0f, 1.0f }; // 최종 목표 크기
    ActivateDesc.vStartPos = pDesc->vStartPos;

    // ⭐ 단 하나의 검풍만 활성화
    if (!m_vecSwordWinds.empty())
    {
        m_vecSwordWinds[0]->OnActivate(&ActivateDesc);
    }


}

// 풀에 다시 넣지 말고 삭제해버리자. 
// => Pooling에 충분할정도로 많이 넣어두고 풀에 안넣고 오브젝트 매니저에서 삭제 객체에 넣기.
void CEffect_Wind::OnDeActivate()
{
    m_IsDestroy = true;
}

void CEffect_Wind::Calc_Timer(_float fTimeDelta)
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

HRESULT CEffect_Wind::Ready_Components(EFFECTWIND_DESC* pDesc)
{

    return S_OK;
}

HRESULT CEffect_Wind::Ready_PartObjects()
{

    /* Clone시 지정될 값들 => 초기 설정 값들.*/
    CSwordWind::SWORDWIND_DESC WindDesc{};
    WindDesc.pOwner = this;
    WindDesc.eCurLevel = m_eCurLevel;
    WindDesc.eShaderPath = MESH_SHADERPATH::SWORD_WIND;

    // 텍스처 인덱스 설정
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_DIFFUSE] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_OTHER] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_NOISE] = 0;
    WindDesc.iTextureIndexArray[TEXTURE::TEXTURE_SWIRL] = 0;

    _wstring strComTag = TEXT("Com_SwordWind");
    m_vecSwordWinds.resize(4); // 4개의 검풍

    for (_uint i = 0; i < m_vecSwordWinds.size(); ++i)
    {
        // Com Tag 다르게 설정
        _wstring strUniqueTag = strComTag + to_wstring(i);

        WindDesc.fDisplayTime = m_fDuration;
        
        // 기본 크기는 OnActivate에서 설정
        _float3 vScale = { 1.0f, 1.0f, 1.0f };
        
        
        XMStoreFloat3(&WindDesc.vStartScale, XMLoadFloat3(&vScale));

        if (FAILED(CContainerObject::Add_PartObject(strUniqueTag,
            ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SwordWind"),
            reinterpret_cast<CPartObject**>(&m_vecSwordWinds[i]), &WindDesc)))
        {
            CRASH("Failed Create SwordWind");
            return E_FAIL;
        }
    }

    return S_OK;
}

#pragma endregion


#ifdef _DEBUG
void CEffect_Wind::ImGui_Render()
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





CEffect_Wind* CEffect_Wind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Wind* pInstance = new CEffect_Wind(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Wind"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Wind::Clone(void* pArg)
{
    CEffect_Wind* pInstance = new CEffect_Wind(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_Wind"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Wind::Free()
{
    CContainerObject::Free();
	for (auto& pSwordWind : m_vecSwordWinds)
		Safe_Release(pSwordWind);

    //Safe_Release(m_pBloodPillarA);
    //Safe_Release(m_pBloodPillarB);
    //Safe_Release(m_pBloodPillarC);
    
}
