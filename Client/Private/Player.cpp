#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CGameObject(Prototype)
{
}

void CPlayer::Move_By_Camera_Direction_8Way(DIR eDir, _float fTimeDelta, _float fSpeed)
{
    CCamera* pCamera = m_pGameInstance->Get_MainCamera();
    if (!pCamera)
    {
        CRASH("Camera Not Found");
        return;
    }

    _vector vLook = pCamera->Get_LookVector();  // Y 제거 포함된 형태여야 함
    _vector vRight = pCamera->Get_RightVector();

    vLook = XMVectorSetY(vLook, 0.f);
    vRight = XMVectorSetY(vRight, 0.f);
    vLook = XMVector3Normalize(vLook);
    vRight = XMVector3Normalize(vRight);

    _vector vMoveDir = XMVectorZero();

    switch (eDir)
    {
        case DIR::U:   vMoveDir = vLook; break;
        case DIR::D:   vMoveDir = -vLook; break;
        case DIR::L:   vMoveDir = -vRight; break;
        case DIR::R:   vMoveDir = vRight; break;
        case DIR::LU:  vMoveDir = XMVector3Normalize(vLook - vRight); break;
        case DIR::LD:  vMoveDir = XMVector3Normalize(-vLook - vRight); break;
        case DIR::RU:  vMoveDir = XMVector3Normalize(vLook + vRight); break;
        case DIR::RD:  vMoveDir = XMVector3Normalize(-vLook + vRight); break;
        default: return;
    }

    vMoveDir = XMVectorSetY(vMoveDir, 0.f);

    if (XMVector3Equal(vMoveDir, XMVectorZero()))
        return;

    vMoveDir = XMVector3Normalize(vMoveDir);

    // 2. 이동 방향 기준 Yaw 회전 계산
    _float x = XMVectorGetX(vMoveDir);
    _float z = XMVectorGetZ(vMoveDir);
    _float fYaw = atan2f(x, z); // <- 회전 각도

    // 3. 회전 쿼터니언 적용
    _vector qRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw);
    m_pTransformCom->Set_Quaternion(qRot);

    // 4. 이동 적용
    m_pTransformCom->Move_Direction(vMoveDir, fTimeDelta * 0.5f);
}


HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
    
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Fsm()))
        return E_FAIL;

    _float3 vPos = { 0.f, 10.f, 0.f };
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

    //m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(180.f));

    // Player 정면 바라보게 하기?
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));

    //m_pModelCom->Set_Animation(6, true);
    //m_pModelCom->Set_Animation(38, true);
    
    

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);



    /*if (m_pGameInstance->Get_KeyPress(DIK_W))
   {
       m_pTransformCom->Go_Straight(fTimeDelta);
   }

   if (m_pGameInstance->Get_KeyPress(DIK_S))
   {
       m_pTransformCom->Go_Backward(fTimeDelta);
   }

   if (m_pGameInstance->Get_KeyPress(DIK_A))
   {
       m_pTransformCom->Go_Left(fTimeDelta);
   }

   if (m_pGameInstance->Get_KeyPress(DIK_D))
   {
       m_pTransformCom->Go_Right(fTimeDelta);
   }*/

}

void CPlayer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (nullptr != m_pFsmCom)
        m_pFsmCom->Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        int a = 0;
    }

    
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 350.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    // Cond_Once: 최초 한 번만 위치 적용 → 이후 드래그 가능
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);
    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);
    ImGui::End();
#endif // _DEBUG



    if (FAILED(Ready_Render_Resources()))
    {
        CRASH("Ready Render Resource Failed");
        return E_FAIL;
    }
        

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            CRASH("Ready Bine Materials Failed");

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            CRASH("Ready Bone Matrices Failed");
            

        if (FAILED(m_pShaderCom->Begin(0)))
            CRASH("Ready Shader Begin Failed");

        if (FAILED(m_pModelCom->Render(i)))
            CRASH("Ready Render Failed");
    }
    

    return S_OK;
}

void CPlayer::On_Collision_Enter(CGameObject* pOther)
{
}

void CPlayer::On_Collision_Stay(CGameObject* pOther)
{
}

void CPlayer::On_Collision_Exit(CGameObject* pOther)
{
}

#pragma region 플레이어 상태 함수들
void CPlayer::Change_Animation(_uint iAnimationIndex, _bool isLoop)
{
    m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
    if (!isLoop)
        CRASH("현재 루프가 안됌");
        
}

#pragma endregion



HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Model_Player")
        ,TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Ready_Fsm()
{
    CFsm::FSM_DESC Desc{};
    Desc.pOwner = this;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Fsm")
        , TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), &Desc)))
        return E_FAIL;


    CPlayerState::PLAYER_STATE_DESC PlayerDesc{};
    PlayerDesc.pFsm = m_pFsmCom;
    PlayerDesc.pOwner = this;

    m_pFsmCom->Add_State(CPlayer_IdleState::Create(PLAYER_STATE::IDLE, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_WalkState::Create(PLAYER_STATE::WALK, &PlayerDesc));
    m_pFsmCom->Add_State(CPlayer_RunState::Create(PLAYER_STATE::RUN, &PlayerDesc));

    CPlayer_IdleState::IDLE_ENTER_DESC enter{};
    enter.iAnimation_Index = 17;

    m_pFsmCom->Change_State(PLAYER_STATE::IDLE, &enter);
    return S_OK;
}

HRESULT CPlayer::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
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

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Destroy()
{
    __super::Destroy();
}

void CPlayer::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pFsmCom);
}
