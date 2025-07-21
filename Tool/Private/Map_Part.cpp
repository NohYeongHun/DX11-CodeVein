
CMap_Part::CMap_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMap_Part::CMap_Part(const CMap_Part& Prototype)
    : CGameObject(Prototype)
{
}


const MAP_PART_INFO& CMap_Part::Save_NonAminModel(_fmatrix PreTransformMatrix)
{
    MODEL_INFO modelInfo = m_pModelCom->Save_NonAminModel(PreTransformMatrix, m_strObjTag);


    return {
        modelInfo.strModelTag,
        modelInfo.meshVectorSize,
        modelInfo.meshVector,
        modelInfo.materialVectorSize,
        modelInfo.materialVector
    };
}

HRESULT CMap_Part::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

/* Create Desc와 저장 Desc를 동일하게..*/
HRESULT CMap_Part::Initialize_Clone(void* pArg)
{
    MODEL_CREATE_DESC* pDesc = static_cast<MODEL_CREATE_DESC*>(pArg);
    m_pModelTag = pDesc->pModelTag;
    m_strObjTag = m_pModelTag;

    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Transform(pDesc)))
        return E_FAIL;

    //m_pTransformCom->Scaling({ 1.f, 1.f, 1.f });
    return S_OK;
}

/* Load 용*/
HRESULT CMap_Part::Initialize_Load(void* pArg)
{
    MAP_PART_INFO* pDesc = static_cast<MAP_PART_INFO*>(pArg);

    return S_OK;
}



void CMap_Part::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMap_Part::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    _float fDist = {};

   /* if (m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::LB) &&
        m_pModelCom->Is_Ray_Hit(m_pGameInstance->Get_RayOrigin()
        , m_pGameInstance->Get_RayDir(), &fDist))
    {
        MSG_BOX(TEXT("Map Part Ray Hit!"));
        TOOL_SELECT_OBJECT_DESC Desc{};
        Desc.pSelectedObject = this;
        m_pGameInstance->Publish(EventType::SELECTED_MODEL, &Desc);
    }*/
    
}

void CMap_Part::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CMap_Part::Render()
{
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    
    

    /*string strName = "Map_Part : " + to_string(Get_ID());
    _float4 vPos = {};
    XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    Transform_Print_Imgui(strName.c_str(), reinterpret_cast<_float*>(&vPos));
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vPos));*/

    return S_OK;
}

void CMap_Part::On_Collision_Enter(CGameObject* pOther)
{
}

void CMap_Part::On_Collision_Stay(CGameObject* pOther)
{
}

void CMap_Part::On_Collision_Exit(CGameObject* pOther)
{
}

const _bool CMap_Part::Is_Ray_LocalHit(_float* pOutDist)
{
    // Ray를 Local로 변환.
    m_pGameInstance->Transform_To_LocalSpace(m_pTransformCom->Get_WorldMatrix_Inverse());

    if (m_pModelCom->Is_Ray_Hit(m_pGameInstance->Get_Local_RayOrigin()
        , m_pGameInstance->Get_Local_RayDir(), pOutDist))
        return true;

    return false;
}

HRESULT CMap_Part::Ready_Components(MODEL_CREATE_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO)
        , pDesc->pModelTag
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    

    return S_OK;
}

HRESULT CMap_Part::Ready_Transform(MODEL_CREATE_DESC* pDesc)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPosition));
    m_pTransformCom->Scale(pDesc->vScale);

    if (pDesc->vRotate.x > 0.f) 
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRotate.x));
    else if (pDesc->vRotate.y > 0.f)
        m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRotate.y));
	else if (pDesc->vRotate.z > 0.f)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->vRotate.z));

    return S_OK;
}

HRESULT CMap_Part::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CMap_Part* CMap_Part::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Part* pInstance = new CMap_Part(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMap_Part"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Part::Clone(void* pArg)
{
    CMap_Part* pInstance = new CMap_Part(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Part::Destroy()
{
    __super::Destroy();
}

void CMap_Part::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
