#include "ToolMap_Part.h"

CToolMap_Part::CToolMap_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CToolMap_Part::CToolMap_Part(const CToolMap_Part& Prototype)
    : CGameObject(Prototype)
{
}


//const MAP_PART_INFO& CToolMap_Part::Save_NonAminModel(_fmatrix PreTransformMatrix)
//{
//    MODEL_INFO modelInfo = m_pModelCom->Save_NonAminModel(PreTransformMatrix, m_strModelTag);
//    
//
//    return {
//        modelInfo.strModelTag,
//        modelInfo.meshVectorSize,
//        modelInfo.meshVector,
//        modelInfo.materialVectorSize,
//        modelInfo.materialVector
//    };
//}

HRESULT CToolMap_Part::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

/* Create Desc와 저장 Desc를 동일하게..*/
HRESULT CToolMap_Part::Initialize_Clone(void* pArg)
{
    MAP_PART_DESC* pDesc = static_cast<MAP_PART_DESC*>(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (pDesc->eArgType == ARG_TYPE::CREATE)
    {
        MODEL_CREATE_DESC* pCreateDesc = static_cast<MODEL_CREATE_DESC*>(pDesc->pData);
        if(FAILED(Initialize_Craete(pCreateDesc)))
            return E_FAIL;
    }
    else if (pDesc->eArgType == ARG_TYPE::MODEL_LOAD)
    {
        MAP_PART_INFO* pInfoDesc = static_cast<MAP_PART_INFO*>(pDesc->pData);
        if (FAILED(Initialize_Load(pInfoDesc)))
            return E_FAIL;
    }
    

    m_pTransformCom->Set_ScaleX(1.5f);
    m_pTransformCom->Set_ScaleZ(1.5f);
    return S_OK;
}

HRESULT CToolMap_Part::Initialize_Craete(MODEL_CREATE_DESC* pDesc)
{
    m_strModelTag = pDesc->pModelTag;
    m_strObjTag = m_strModelTag.c_str();


    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO)
        , pDesc->pModelTag
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    if (FAILED(Ready_Transform(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolMap_Part::Initialize_Load(MAP_PART_INFO* pDesc)
{
    m_strModelTag = pDesc->strModelTag;
    m_strObjTag = m_strModelTag.c_str();

    if (FAILED(__super::Initialize_Clone(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO)
        , pDesc->strModelTag.c_str()
        , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    /* Transform 정보도 저장해서 추가.*/
    if (FAILED(Ready_Transform(pDesc)))
        return E_FAIL;

    return S_OK;
}




void CToolMap_Part::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CToolMap_Part::Update(_float fTimeDelta)
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

void CToolMap_Part::Late_Update(_float fTimeDelta)
{
    
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CToolMap_Part::Render()
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

/* 메시 피킹용 반환 위치를 PickingPoint에 담아줍니다.  */
_bool CToolMap_Part::Picking(_float3* PickingPoint)
{
    // 현재 RayDir을 Local Pos로 변환
    m_pGameInstance->Transform_To_LocalSpace(m_pTransformCom->Get_WorldMatrix_Inverse());

    // 반환시 PickingPoint에 월드로 전달.
    if (m_pModelCom->Picking(PickingPoint))
    {
        _vector vWorldPickPoint = XMVector3TransformCoord(XMLoadFloat3(PickingPoint), m_pTransformCom->Get_WorldMatrix());
        XMStoreFloat3(PickingPoint, vWorldPickPoint);
        return true;
    }

    return false;
}

void CToolMap_Part::On_Collision_Enter(CGameObject* pOther)
{
}

void CToolMap_Part::On_Collision_Stay(CGameObject* pOther)
{
}

void CToolMap_Part::On_Collision_Exit(CGameObject* pOther)
{
}

const _bool CToolMap_Part::Is_Ray_LocalHit(_float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist)
{
    // 현재 Ray를 Local로 변환.
    m_pGameInstance->Transform_To_LocalSpace(m_pTransformCom->Get_WorldMatrix_Inverse());

    // Local로 변환된 시점의 Ray를 이용해서 RayHit 확인.
    if (m_pModelCom->Is_Ray_Hit(m_pGameInstance->Get_Local_RayOrigin()
        , m_pGameInstance->Get_Local_RayDir(), pOutLocalPos, pOutLocalNormal, pOutDist))
        return true;

    return false;
}

const _bool CToolMap_Part::Is_Ray_LocalHit(MODEL_PICKING_INFO* pPickingInfo, _float* pOutDist)
{
    m_pGameInstance->Transform_To_LocalSpace(m_pTransformCom->Get_WorldMatrix_Inverse());
    // Local로 변환된 시점의 Ray를 이용해서 RayHit 확인.
    if (m_pModelCom->Is_Ray_Hit(m_pGameInstance->Get_Local_RayOrigin()
        , m_pGameInstance->Get_Local_RayDir(), pPickingInfo, pOutDist))
        return true;

    return false;
}

HRESULT CToolMap_Part::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    return S_OK;
}


HRESULT CToolMap_Part::Ready_Transform(MODEL_CREATE_DESC* pDesc)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPosition));
    m_pTransformCom->Scale(pDesc->vScale);

    if (pDesc->vRotate.x > 0.f)
        m_pTransformCom->Add_Rotation(XMConvertToRadians(pDesc->vScale.x), 0.f, 0.f);
    else if (pDesc->vRotate.y > 0.f)
        m_pTransformCom->Add_Rotation(0.f, XMConvertToRadians(pDesc->vScale.y), 0.f);
    else if (pDesc->vRotate.z > 0.f)
        m_pTransformCom->Add_Rotation(0.f, 0.f, XMConvertToRadians(pDesc->vScale.z));

    return S_OK;
}

HRESULT CToolMap_Part::Ready_Transform(MAP_PART_INFO* pDesc)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&(pDesc->transformInfo.vPosition)));
    m_pTransformCom->Scale(pDesc->transformInfo.vScale);

   /* m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPosition));
    m_pTransformCom->Scale(pDesc->vScale);

    if (pDesc->vRotate.x > 0.f)
        m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRotate.x));
    else if (pDesc->vRotate.y > 0.f)
        m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRotate.y));
    else if (pDesc->vRotate.z > 0.f)
        m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->vRotate.z));*/

    return S_OK;
}

HRESULT CToolMap_Part::Ready_Render_Resources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CToolMap_Part* CToolMap_Part::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CToolMap_Part* pInstance = new CToolMap_Part(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CToolMap_Part"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CToolMap_Part::Clone(void* pArg)
{
    CToolMap_Part* pInstance = new CToolMap_Part(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CToolMap_Part::Destroy()
{
    __super::Destroy();
}

void CToolMap_Part::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
