#include "Sky.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
   
}

CSky::CSky(const CSky& Prototype)
    : CGameObject( Prototype )
{
}

HRESULT CSky::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSky::Initialize_Clone(void* pArg)
{

    SKY_DESC* pDesc = static_cast<SKY_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
		CRASH("Failed Init GameObject")
        return E_FAIL;
    }
        

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }
        

    //m_pTransformCom->Set_Scale({ 5.f, 5.f, 5.f });

    return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{

}

void CSky::Update(_float fTimeDelta)
{
    // 이게 한프레임 이전의 CamPosition을 가지고 와서.
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

}

void CSky::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this)))
        return;
}

HRESULT CSky::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Failed Bind Shader Resources");
        return E_FAIL;
    }
        

    if (FAILED(m_pShaderCom->Begin(0)))
    {
        CRASH("Failed Begin Shader Resources");
        return E_FAIL;
    }
    
    if (FAILED(m_pVIBufferCom->Bind_Resources()))
    {
        CRASH("Failed Bind_Resources");
        return E_FAIL;
    }

   
    if (FAILED(m_pVIBufferCom->Render()))
    {
        CRASH("Failed VIBuffer Render");
        return E_FAIL;
    }

#pragma region MODEL
   /* _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        if (FAILED(m_pShaderCom->Begin(1)))
        {
            CRASH("Shader Begin Failed");
            return E_FAIL;
        }


        if (FAILED(m_pModelCom->Render(i)))
        {
            CRASH("Shader Begin Failed");
            return E_FAIL;
        }

    }*/
#pragma endregion


  


    

    return S_OK;
}

HRESULT CSky::Ready_Components(SKY_DESC* pDesc)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    {
		CRASH("Failed Add Shader Component");
        return E_FAIL;
    }
        

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
    {
        CRASH("Failed Add Com_VIBuffer");
        return E_FAIL;
    }
     

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Texture_Sky"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    //{

    //    CRASH("Failed Add Sky Texture");
    //    return E_FAIL;
    //}

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), pDesc->PrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    {

        CRASH("Failed Add Sky Texture");
        return E_FAIL;
    }
        


#pragma region MODEL
   /* if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    return E_FAIL;


    CLoad_Model::LOADMODEL_DESC Desc{};
    Desc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY)
        , TEXT("Prototype_Component_Model_SkySphere")
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &Desc)))
    {
        CRASH("Failed Load SkySphere");
        return E_FAIL;
    }*/

#pragma endregion

        



    return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
   

#pragma region MODEL
    /*if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
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
        return E_FAIL;*/
#pragma endregion

   

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSky* pInstance = new CSky(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSky"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
    CSky* pInstance = new CSky(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CSky"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSky::Free()
{
    CGameObject::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pModelCom);
}
