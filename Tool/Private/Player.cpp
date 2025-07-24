#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CGameObject(Prototype)
{
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

    // Player 정면 바라보게 하기?
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

 /*   if (m_pGameInstance->Get_KeyPress(DIK_W))
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

void CPlayer::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CPlayer::Render()
{
    //if (ImGui::IsWindowAppearing())              // 또는 static bool once=true;
    //{
    //    ImGui::SetNextWindowPos({ 100, 100 }, ImGuiCond_Appearing);
    //    ImGui::SetNextWindowSize({ 460, 240 }, ImGuiCond_Appearing); // ← 원하는 픽셀
    //}

    //string str = "Player Transform [" + to_string(Get_ID()) + ']';
    //ImGui::Begin(str.c_str());
    //_float4 vPosition = {};
    //XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));

    //ImGui::InputFloat("X : ", &vPosition.x);
    //ImGui::InputFloat("Y : ", &vPosition.y);
    //ImGui::InputFloat("Z : ", &vPosition.z);
    //ImGui::End();


    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if(FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            CRASH("Bine Materials Failed");
        

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
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

HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
        CRASH("Shader Create Failed");
        return E_FAIL;
    }
        

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO)
        , TEXT("Prototype_Component_Model_Player")
        ,TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
    {
        CRASH("Model Create Failed");
        return E_FAIL;
    }


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
}
