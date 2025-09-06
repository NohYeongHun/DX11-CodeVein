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
    if (FAILED(CGameObject::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap::Initialize_Clone(void* pArg)
{
    MAP_DESC* pDesc = static_cast<MAP_DESC*>(pArg);
    
    m_eCurLevel = pDesc->eCurLevel;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Init GameObject");
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }
        

    /* 2배 기준으로 NaviMesh 깔았음. */
    m_pTransformCom->Set_Scale(pDesc->vScale);
    // Player 정면 바라보게 하기?
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));

    return S_OK;
}

void CMap::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CMap::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);

    if (m_pNavigationCom)
    {
        _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
        _vector vScale, vRotation, vTranslate;
        XMMatrixDecompose(&vScale, &vRotation, &vTranslate, WorldMatrix);

        _vector vUnitScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
        _matrix ModifiedWorldMatrix = XMMatrixAffineTransformation(vUnitScale, XMVectorZero(), vRotation, vTranslate);

        m_pNavigationCom->Update(ModifiedWorldMatrix);
    }
        
}

void CMap::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pNavigationCom)))
    {
        CRASH("Failed Load Navigation Component");
        return;
    }
#endif // _DEBUG

}

HRESULT CMap::Render()
{
    if (FAILED(Ready_Render_Resources()))
        return E_FAIL;

#pragma region 기본 렌더링
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
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
    {
        CRASH("Failed Crash Shader Component");
        return E_FAIL;
    }
        

    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel)
        , pDesc->PrototypeTag
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Navigation"),
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
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
    CGameObject::Destroy();
}

void CMap::Free()
{
    CGameObject::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pNavigationCom);
}
