#include "KnightLance.h"
CKnightLance::CKnightLance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CKnightLance::CKnightLance(const CKnightLance& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CKnightLance::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKnightLance::Initialize_Clone(void* pArg)
{
    KNIGHT_LANCE_DESC* pDesc = static_cast<KNIGHT_LANCE_DESC*>(pArg);


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

    if (FAILED(Ready_Effects()))
    {
        CRASH("Failed Ready_Effects");
        return E_FAIL;
    }

    m_vPointUp = _float3(0.f, 0.5f, 0.f);
    m_vPointDown = _float3(0.f, -3.5f, 0.f);

    m_bTrail = true;


    /*m_pTransformCom->Scaling(_float3(0.1f, 0.1f, 0.1f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));*/

    // 기본 false;
    m_pColliderCom->Set_Active(false);
    return S_OK;
}

void CKnightLance::Priority_Update(_float fTimeDelta)
{
    CWeapon::Priority_Update(fTimeDelta);
}

void CKnightLance::Update(_float fTimeDelta)
{
    CWeapon::Update(fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(m_pSocketMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    Finalize_Update(fTimeDelta);
}

void CKnightLance::Late_Update(_float fTimeDelta)
{
    CWeapon::Late_Update(fTimeDelta);


    if (Is_Visible())
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
            return;
    }

    if (m_bTrail)
        m_pTrailWeapon_Effect->Late_Update(fTimeDelta);
}

void CKnightLance::Finalize_Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);


    // Trail Visible이 False더라도 정점의 위치는 계속 업데이트 되어야함 => 그래야 그려질때 안이상함.
    TrailWeapon_Update(SocketMatrix);
    m_pTrailWeapon_Effect->Update(fTimeDelta);

    CWeapon::Finalize_Update(fTimeDelta);
}

HRESULT CKnightLance::Render()
{

#ifdef _DEBUG
    //ImGui_Render();

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

void CKnightLance::On_Collision_Enter(CGameObject* pOther)
{
}

void CKnightLance::On_Collision_Stay(CGameObject* pOther)
{
}

void CKnightLance::On_Collision_Exit(CGameObject* pOther)
{
}

void CKnightLance::OBBCollider_ChangeExtents(_float3 vExtents)
{
    CBounding_OBB::BOUNDING_OBB_DESC* pDesc = static_cast<CBounding_OBB::BOUNDING_OBB_DESC*>(m_pColliderCom->Get_BoundingDesc());

    CBounding_OBB::BOUNDING_OBB_DESC Desc{};
    Desc = *pDesc;
    Desc.vExtents = vExtents;
    m_pColliderCom->Change_BoundingDesc(&Desc);
}

void CKnightLance::TrailWeapon_Update(_matrix WeaponSocketMatrix)
{
    m_pTrailWeapon_Effect->Update_Trail_Point(m_vPointDown, m_vPointUp, WeaponSocketMatrix);
}

HRESULT CKnightLance::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Model_GodChildLance"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CKnightLance::Ready_Colliders()
{
    BOUNDING_BOX box = m_pModelCom->Get_BoundingBox();

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vExtents = _float3(box.vExtents.x * 3.f, box.vExtents.y, box.vExtents.z);
    OBBDesc.vCenter = _float3(0.f, -box.vExtents.y, 0.f); // 중점.

    // 원본 Extents 저장.
    m_vOriginExtents = OBBDesc.vExtents;

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

    /* 생성과 동시에 등록 */
    m_pGameInstance->Add_Collider_To_Manager(m_pColliderCom, ENUM_CLASS(m_eCurLevel));

    return S_OK;
}

HRESULT CKnightLance::Ready_Effects()
{

    CSwordTrail::SWORDTRAIL_DESC Desc{};
    Desc.eCurLevel = m_eCurLevel;
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(1.0f);
    Desc.eDiffuseType = TRAIL_DIFFUSE::THICK_BLOOD;

    //m_pMatrixArray[0] = m_pModelCom->Get_LocalBoneMatrix("TrailStartSocket");
    //m_pMatrixArray[1] = m_pModelCom->Get_LocalBoneMatrix("TrailStartSocket_end");
    //m_pMatrixArray[2] = m_pModelCom->Get_LocalBoneMatrix("TrailEndSocket");
    //m_pMatrixArray[3] = m_pModelCom->Get_LocalBoneMatrix("TrailEndSocket_end");

    


    m_pTrailWeapon_Effect = dynamic_cast<CSwordTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
        ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_GameObject_SwordTrail"), &Desc));

    if (nullptr == m_pTrailWeapon_Effect)
    {
        CRASH("Failed Create TrailWeapon Effect");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CKnightLance::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    //const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    //if (nullptr == pLightDesc)
    //    return E_FAIL;
    //
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
    //    return E_FAIL;
    

    return S_OK;
}

CKnightLance* CKnightLance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKnightLance* pInstance = new CKnightLance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKnightSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKnightLance::Clone(void* pArg)
{
    CKnightLance* pInstance = new CKnightLance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CKnightSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKnightLance::Free()
{
    CWeapon::Free();
    Safe_Release(m_pTrailWeapon_Effect);
}

#pragma region EFFECT
void CKnightLance::Start_Dissolve()
{
    m_fDissolveTime = 0.f;
    m_iShaderPath = static_cast<_uint>(MESH_SHADERPATH::DISSOLVE);
}

void CKnightLance::ReverseStart_Dissolve()
{
    m_fDissolveTime = 0.f;
    m_iShaderPath = static_cast<_uint>(MESH_SHADERPATH::DISSOLVE_REVERSE);
}

void CKnightLance::End_Dissolve()
{
    m_fDissolveTime = 0.f;
    m_iShaderPath = static_cast<_uint>(MESH_SHADERPATH::DEFAULT);
}
#pragma endregion



#ifdef _DEBUG
void CKnightLance::ImGui_Render()
{
    //Edit_Collider(m_pColliderCom, "QueenKnight Lance");

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("QueenKnight Weapon Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Weapon Pos (%.2f, %.2f, %.2f)", m_CombinedWorldMatrix.m[3][0]
    , m_CombinedWorldMatrix.m[3][1], m_CombinedWorldMatrix.m[3][2]);

    static float vPointUp[3] = { m_vPointUp.x, m_vPointUp.y, m_vPointUp.z };
    static float vPointDown[3] = { m_vPointDown.x, m_vPointDown.y, m_vPointDown.z };
    ImGui::InputFloat3("Point Up : ", vPointUp);
    ImGui::InputFloat3("Point Down : ", vPointDown);

    if (ImGui::Button("Apply"))
    {
        m_vPointUp = { vPointUp[0], vPointUp[1], vPointUp[2] };
        m_vPointDown = { vPointDown[0], vPointDown[1], vPointDown[2] };
    }

    //static float vTrailStartPos[3] = { m_pMatrixArray[0]->m[3][0], m_pMatrixArray[0]->m[3][1], m_pMatrixArray[0]->m[3][2] };
    //static float vTrailEndPos[3] = { m_pMatrixArray[2]->m[3][0], m_pMatrixArray[2]->m[3][1], m_pMatrixArray[2]->m[3][2] };
    //
    //ImGui::Text("Weapon Trail End Pos (%.2f, %.2f, %.2f)", vTrailEndPos[0],
    //    vTrailEndPos[1], vTrailEndPos[2]);

    ImGui::End();

    m_pColliderCom->Render();
}
#endif // _DEBUG


