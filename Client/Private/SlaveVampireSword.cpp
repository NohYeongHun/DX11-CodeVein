CSlaveVampireSword::CSlaveVampireSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CSlaveVampireSword::CSlaveVampireSword(const CSlaveVampireSword& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CSlaveVampireSword::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSlaveVampireSword::Initialize(void* pArg)
{
    SLAVEVAMPIRE_WEAPON_DESC* pDesc = static_cast<SLAVEVAMPIRE_WEAPON_DESC*>(pArg);


    if (FAILED(CWeapon::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    if (FAILED(Ready_Colliders()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    // 기본 false;
    m_pColliderCom->Set_Active(false);
    m_pTransformCom->Add_Rotation(0.5f, 0.5f, 0.5f);

    return S_OK;
}

void CSlaveVampireSword::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);
}

void CSlaveVampireSword::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);

    if (nullptr != m_pSocketMatrix && nullptr != m_pParentMatrix)
    {
        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() *
            XMLoadFloat4x4(m_pSocketMatrix) *
            XMLoadFloat4x4(m_pParentMatrix));
    }
    else
    {
        OutputDebugStringA("SlaveVampireWeapon: Socket or Parent matrix is nullptr!\n");
    }

    CWeapon::Update_Timer(fTimeDelta);
    CWeapon::Finalize_Update(fTimeDelta);
}

void CSlaveVampireSword::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
        return;
#endif // _DEBUG

}

HRESULT CSlaveVampireSword::Render()
{
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

        m_pShaderCom->Begin(m_iShaderPath);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

void CSlaveVampireSword::On_Collision_Enter(CGameObject* pOther)
{
}

void CSlaveVampireSword::On_Collision_Stay(CGameObject* pOther)
{
}

void CSlaveVampireSword::On_Collision_Exit(CGameObject* pOther)
{
}

HRESULT CSlaveVampireSword::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Model_SlaveVampireGreatSword"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    // Dissolve Texture
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve"),
        TEXT("Com_Dissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTexture), nullptr)))
    {
        CRASH("Failed Load DissolveTexture");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSlaveVampireSword::Ready_Colliders()
{
    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(box.vExtents.x, box.vExtents.y, box.vExtents.z);
    OBBDesc.vCenter = _float3(0.f, -box.vExtents.y, 0.f); // 중점.


    OBBDesc.vRotation = _float3(
        XMConvertToRadians(-95.f),  // Pitch: 아래쪽으로 기울어짐
        XMConvertToRadians(45.f),   // Yaw: 오른쪽으로 회전
        XMConvertToRadians(0.f)     // Roll: 회전 없음
    );

    OBBDesc.pOwner = this;
    OBBDesc.eCollisionType = CCollider::COLLISION_TRIGGER;
    OBBDesc.eMyLayer = CCollider::MONSTER_WEAPON;
    OBBDesc.eTargetLayer = CCollider::PLAYER | CCollider::PLAYER_WEAPON |
        CCollider::PLAYER_SKILL | CCollider::STATIC_OBJECT;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Collider_OBB")
        , TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
    {
        CRASH("Failed Clone Collider OBB");
        return E_FAIL;
    }

    if (nullptr == m_pColliderCom)
    {
        OutputDebugStringA("SlaveVampireWeapon: m_pColliderCom is nullptr after creation!\n");
        return E_FAIL;
    }

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));
    OutputDebugStringA("SlaveVampireWeapon: Collider created and registered successfully\n");

    return S_OK;
}

HRESULT CSlaveVampireSword::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
    {
        CRASH("Failed BindWorld Matrix");
        return E_FAIL;
    }
        

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed BindView Matrix");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed BindProj Matrix");
        return E_FAIL;
    }

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
    {
        CRASH("Failed BindLight");
        return E_FAIL;
    }


    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
    {
        CRASH("Failed BindLight");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
    {
        CRASH("Failed BindLight");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
    {
        CRASH("Failed BindLight");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
    {
        CRASH("Failed BindLight");
        return E_FAIL;
    }

    if (FAILED(m_pDissolveTexture->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0)))
    {

        return E_FAIL;
    }
        


    _float fDissolveTime = {};
    if (m_bDissolve)
        fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxDissolveTime);
    else if (m_bReverseDissolve)
        fDissolveTime = normalize(m_fCurDissolveTime, 0.f, m_fMaxReverseDissolveTime);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
    {
        CRASH("Failed Dissolve Texture");
        return E_FAIL;
    }

    return S_OK;
}

CSlaveVampireSword* CSlaveVampireSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlaveVampireSword* pInstance = new CSlaveVampireSword(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSlaveVampireSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSlaveVampireSword::Clone(void* pArg)
{
    CSlaveVampireSword* pInstance = new CSlaveVampireSword(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSlaveVampireSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlaveVampireSword::Free()
{
    CWeapon::Free();
}

#ifdef _DEBUG
void CSlaveVampireSword::ImGui_Render()
{
    //Edit_Collider(m_pColliderCom, "SlaveVampireTag");
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window

    ImVec2 windowSize = ImVec2(300.f, 300.f);
    ImVec2 windowPos = ImVec2(io.DisplaySize.x - windowSize.x, windowSize.y);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = "VampireGreat Sword";
    ImGui::Begin(strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};

    static _float vRotation[3] = { 0.f, 0.f, 0.f };
    ImGui::InputFloat3("Rotation", vRotation);

    if (ImGui::Button("Apply"))
    {
        m_pTransformCom->Add_Rotation(vRotation[0], vRotation[1], vRotation[2]);
    }

    /* 정보 표시. */
    ImGui::Text("Rotation : (%.2f, %.2f, %.2f)", vRotation[0], vRotation[1], vRotation[2]);
    ImGui::Separator();

    ImGui::End();


}
#endif // _DEBUG
