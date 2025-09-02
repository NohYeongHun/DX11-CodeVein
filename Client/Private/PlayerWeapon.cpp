CPlayerWeapon::CPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon{ pDevice, pContext }
{

}

CPlayerWeapon::CPlayerWeapon(const CPlayerWeapon& Prototype)
    : CWeapon( Prototype )
{

}

HRESULT CPlayerWeapon::Initialize_Prototype()
{
    m_strObjTag = TEXT("PlayerWeapon");
    return S_OK;
}

HRESULT CPlayerWeapon::Initialize_Clone(void* pArg)
{
    PLAYER_WEAPON_DESC* pDesc = static_cast<PLAYER_WEAPON_DESC*>(pArg);
    //m_pParentState = pDesc->pState;
    if (FAILED(CWeapon::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone CWeapon");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders()))
    {
        CRASH("Failed Ready_Colliders");
        return E_FAIL;
    }

    if (FAILED(Ready_Effects()))
    {
        CRASH("Failed Ready_Effects");
        return E_FAIL;
    }

    // * m_vPointUp, m_vPointDown으로 하드코딩 하는게 아니라 Socket 월드 위치 가져오기.

        
    m_vPointUp = _float3(0.f, 0.f, 0.f);
    m_vPointDown = _float3(0.f, 0.f, 1.85f);

    m_pWeaponTrailStart_SocketMatrix = m_pModelCom->Get_BoneMatrix("TrailStartSocket");
    m_pWeaponTrailEnd_SocketMatrix = m_pModelCom->Get_BoneMatrix("TrailEndSocket");

    //m_vPointUp = _float3(0.0f, 0.0f, 0.f);
    //m_vPointDown = _float3(0.0f, 0.0f, 0.f);

    m_pTransformCom->Add_Rotation(XMConvertToRadians(0.f)
        , XMConvertToRadians(180.f)
        , XMConvertToRadians(0.f));
    //m_pTransformCom->Scaling(_float3(0.1f, 0.1f, 0.1f));
    //m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));

    return S_OK;
}

void CPlayerWeapon::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);

}

/* Weapon은 CombinedWorldMatrix를 Collider에 전달 해주어야합니다. => 부모 트랜스폼이 있으므로 */
void CPlayerWeapon::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);
    // 1. 가장 먼저 Combined 행렬 초기화
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(m_pSocketMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    


    // 가장 마지막 부근에 Collider Push
    Finalize_Update(fTimeDelta);
}

void CPlayerWeapon::Finalize_Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);


    // Trail Visible이 False더라도 정점의 위치는 계속 업데이트 되어야함 => 그래야 그려질때 안이상함.
    TrailWeapon_Update(SocketMatrix);
    m_pTrailWeapon_Effect->Update(fTimeDelta);
    


    CWeapon::Finalize_Update(fTimeDelta);
}

void CPlayerWeapon::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);

    // Trail이 켜질때만 넣기.
    if (m_bTrail)
        m_pTrailWeapon_Effect->Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CPlayerWeapon::Render()
{

#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Player Weapon Debug", nullptr, ImGuiWindowFlags_NoCollapse);


    static float vPointUp[3] = { m_vPointUp.x, m_vPointUp.y, m_vPointUp.z };
    static float vPointDown[3] = { m_vPointDown.x, m_vPointDown.y, m_vPointDown.z };
    ImGui::InputFloat3("Point Up : ", vPointUp);
    ImGui::InputFloat3("Point Down : ", vPointDown);

    if(ImGui::Button("Apply"))
    {
        m_vPointUp = { vPointUp[0], vPointUp[1], vPointUp[2] };
        m_vPointDown = { vPointDown[0], vPointDown[1], vPointDown[2] };
    }


    


    ImGui::End();


    //Edit_Collider(m_pColliderCom, "OBB");
    m_pColliderCom->Render();
#endif // _DEBUG

    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }
        

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }



    return S_OK;
}


#pragma region 1. 무기는 충돌에 대한 상태제어를 할 수 있어야한다.=> 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
void CPlayerWeapon::On_Collision_Enter(CGameObject* pOther)
{
    CWeapon::On_Collision_Enter(pOther);
        
    //OutputDebugWstring(pOther->Get_ObjectTag() + TEXT("충돌"));
    //CWeapon::Deactivate_Collider();
}

void CPlayerWeapon::On_Collision_Stay(CGameObject* pOther)
{

}

void CPlayerWeapon::On_Collision_Exit(CGameObject* pOther)
{
    CWeapon::On_Collision_Exit(pOther);
}

void CPlayerWeapon::Activate_ColliderFrame(_float fDuration)
{
    CWeapon::Activate_ColliderFrame(fDuration);
}

void CPlayerWeapon::Activate_Collider()
{
    CWeapon::Activate_Collider();
}

void CPlayerWeapon::Deactivate_Collider()
{
    CWeapon::Deactivate_Collider();
}

void CPlayerWeapon::Update_ColliderFrame(_float fTimeDelta)
{
    CWeapon::Update_ColliderFrame(fTimeDelta);
}


#pragma endregion

void CPlayerWeapon::TrailWeapon_Update(_matrix WeaponSocketMatrix)
{
    m_pTrailWeapon_Effect->Update_Trail_Point(m_vPointDown, m_vPointUp, WeaponSocketMatrix);
}

HRESULT CPlayerWeapon::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Sword"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    

    return S_OK;
}

HRESULT CPlayerWeapon::Ready_Colliders()
{

    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    OBBDesc.vRotation = _float3(1.5f, 0.f, 0.f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.7f); // 중점.

    OBBDesc.pOwner = this;
    OBBDesc.eCollisionType = CCollider::COLLISION_TRIGGER;
    OBBDesc.eMyLayer = CCollider::PLAYER_WEAPON;
    OBBDesc.eTargetLayer = CCollider::MONSTER | CCollider::MONSTER_WEAPON |
        CCollider::MONSTER_SKILL | CCollider::STATIC_OBJECT;



    //BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();
    //CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{};
    //AABBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    //AABBDesc.vCenter = _float3(0.f, 0.f, 0.f); // 중점.
    //AABBDesc.pOwner = this;

    if (FAILED(CWeapon::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_OBB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
    {
        CRASH("Failed Clone Collider OBB");
        return E_FAIL;
    }

    m_pColliderCom->Set_Active(false);



    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));

    return S_OK;
}

HRESULT CPlayerWeapon::Ready_Effects()
{
    CSwordTrail::SWORDTRAIL_DESC Desc{};
    Desc.eCurLevel = m_eCurLevel;
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(1.0f);
    

;    m_pTrailWeapon_Effect = dynamic_cast<CSwordTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
        ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_GameObject_SwordTrail"), &Desc));

    if (nullptr == m_pTrailWeapon_Effect)
    {
        CRASH("Failed Create TrailWeapon Effect");
        return E_FAIL;
    }
        
    return S_OK;
}

HRESULT CPlayerWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CPlayerWeapon* CPlayerWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerWeapon::Clone(void* pArg)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerWeapon::Free()
{
    CWeapon::Free();
    Safe_Release(m_pTrailWeapon_Effect);
}
