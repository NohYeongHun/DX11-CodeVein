#include "Map.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMap::CMap(const CMap& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CMap::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap::Initialize_Clone(void* pArg)
{
    MAP_DESC* pDesc = static_cast<MAP_DESC*>(pArg);
    
    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    m_pTransformCom->Set_Scale({ 1.5f, 1.f, 1.5f });
    // Player 정면 바라보게 하기?
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));

    return S_OK;
}

void CMap::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMap::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_pNavigationCom)
        m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMap::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CMap::Render()
{
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

#pragma region 기본 렌더링
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        if (FAILED(m_pShaderCom->Begin(0)))
        {
            CRASH("Shader Begin Failed");
            return E_FAIL;
        }


        if (FAILED(m_pModelCom->Render(i)))
        {
            CRASH("Shader Begin Failed");
            return E_FAIL;
        }

    }

#pragma endregion

  

    return S_OK;
}

void CMap::On_Collision_Enter(CGameObject* pOther)
{
}

void CMap::On_Collision_Stay(CGameObject* pOther)
{
}

void CMap::On_Collision_Exit(CGameObject* pOther)
{
}

HRESULT CMap::Ready_Components(MAP_DESC* pDesc)
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    /*if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC)
        , TEXT("Prototype_Component_Model_Player")
        ,TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;*/

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY)
        , pDesc->PrototypeTag
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), nullptr)))
    {

        CRASH("Failed Initialize Navigation Com");
        return E_FAIL;
    }
        

    return S_OK;
}

HRESULT CMap::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap* pInstance = new CMap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
    CMap* pInstance = new CMap(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap::Destroy()
{
    __super::Destroy();
}

void CMap::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pNavigationCom);
}
