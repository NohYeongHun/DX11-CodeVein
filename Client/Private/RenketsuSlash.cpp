#include "RenketsuSlash.h"
CRenketsuSlash::CRenketsuSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CRenketsuSlash::CRenketsuSlash(const CRenketsuSlash& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CRenketsuSlash::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype Slash UI");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenketsuSlash::Initialize_Clone(void* pArg)
{
    SLASHEFFECT_DESC* pDesc = static_cast<SLASHEFFECT_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone SlashUI");
        return E_FAIL;
    }
    m_eCurLevel = pDesc->eCurLevel;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    m_iShaderPath = static_cast<_uint>(EFFECTPOSTEX_SHADERPATH::RENKETSU_LINESLASH);


    m_IsActivate = false;
    return S_OK;
}

void CRenketsuSlash::Priority_Update(_float fTimeDelta)
{   
    if (m_IsActivate && m_fDelay > 0.f)
        return;


    if (!m_IsActivate)
        return;

    CGameObject::Priority_Update(fTimeDelta);
}

void CRenketsuSlash::Update(_float fTimeDelta)
{
    if (!m_IsActivate)
        return;


    if (m_IsActivate && m_fDelay > 0.1f)
    {
        m_fDelay -= fTimeDelta;
        return;
    }




    CGameObject::Update(fTimeDelta);

    

    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;
    
    _float fRatio = m_fCurrentTime / m_fDisplayTime;
    
    _float3 vCurrentScale = {};
    XMStoreFloat3(&vCurrentScale, XMLoadFloat3(&m_vStartScale) + (XMLoadFloat3(&m_vScale) - XMLoadFloat3(&m_vStartScale) * fRatio));
    m_pTransformCom->Set_Scale(vCurrentScale);

    // 시간이 지나면 비활성화
    if (m_fCurrentTime >= m_fDisplayTime)
    {
        m_IsActivate = false;
        Reset_Timer();
        
        return;
    }

    
}

void CRenketsuSlash::Late_Update(_float fTimeDelta)
{
    if (m_IsActivate && m_fDelay > 0.f)
        return;

    if (!m_IsActivate)
        return;

    CGameObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
    {
        CRASH("Failed Add_RenderGroup Slash");
        return;
    }


    //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DISTORTION, this)))
    //{
    //    CRASH("Failed Add_RenderGroup Slash");
    //    return;
    //}
        
}

HRESULT CRenketsuSlash::Render()
{
#ifdef _DEBUG
    ImGui_Render();
#endif // _DEBUG

    if (!m_IsActivate)
    {
        // Active, Target도 아닌데 Render되고 있다면 문제가 있음.
        CRASH("Failed Render Slash");
        return S_OK;
    }


    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Bind Shader Resourcaes Failed");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
    {
        CRASH("Ready Shader Begin Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
    {
        CRASH("Ready Bind Buffer Resources Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render()))
    {
        CRASH("Ready Render LockOnUI Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenketsuSlash::Render_Distortion()
{
    if (!m_IsActivate)
    {
        // Active, Target도 아닌데 Render되고 있다면 문제가 있음.
        CRASH("Failed Render Slash");
        return S_OK;
    }


    if (FAILED(Bind_DistortionShaderResources()))
    {
        CRASH("Ready Bind Shader Resources Failed");
        return E_FAIL;
    }

    m_pDistortionTexture->Bind_Shader_Resource(m_pDistortionShaderCom, "g_DistortionTexture", 0);


    if (FAILED(m_pDistortionShaderCom->Begin(static_cast<_uint>(EFFECTPOSTEX_DISTORTIONSHADERPATH::DEFAULT))))
    {
        CRASH("Ready Shader Begin Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
    {
        CRASH("Ready Bind Buffer Resources Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render()))
    {
        CRASH("Ready Render Failed");
        return E_FAIL;
    }


    return S_OK;
}

#pragma region POOLING 전용 함수
void CRenketsuSlash::OnActivate(void* pArg)
{
    // 1. Activate하는데 필요한 값 설정
    SLASHACTIVATE_DESC* pDesc = static_cast<SLASHACTIVATE_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;

    m_vLocalDirection = pDesc->vLocalDirection;

    
    m_fDisplayTime = pDesc->fDisPlayTime;

    // 2. 설정되었을때 Camera에 따른 방향을 재계산할 필요성이 존재.
    m_IsDirectionCalculated = false;
    m_vScale = pDesc->vScale;
    m_vStartScale = pDesc->vStartScale;

    m_pTargetTransform = pDesc->pTargetTransform;
    m_fDelay = pDesc->fCreateDelay;

    // 3. 위치 및 회전계산.
    Initialize_Transform(pDesc);

    m_IsActivate = true;
}

// Deactivate시 필요한 정보가? 딱히 없을듯.
void CRenketsuSlash::OnDeActivate()
{
    m_pGameInstance->Add_GameObject_ToPools(TEXT("RENKETSU_SLASH_EFFECT"), ENUM_CLASS(CRenketsuSlash::EffectType), this);
}
#pragma endregion



/* 한번만 계산합니다. => 쫓아오게할 순 없잖아. */
void CRenketsuSlash::Initialize_Transform(SLASHACTIVATE_DESC* pDesc)
{
    // 이미 계산되었다면 다시 계산하지 않음
    if (m_IsDirectionCalculated)
        return;

    // 0. 위치 지정.
    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vHitPosition);
    // 1. 카메라 위치 가져오기
    _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
    _vector vSlashPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 빌보드 기본 벡터 계산 (카메라를 향하도록)
    _vector vLook = XMVector3Normalize(vSlashPos - vCamPos);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 3. 공격 방향을 빌보드 평면에 투영하여 회전 각도 계산
    _vector vAttackDirection = XMVector3Normalize(XMLoadFloat3(&m_vLocalDirection));
    _float fRightComponent = XMVectorGetX(XMVector3Dot(vAttackDirection, vRight));
    _float fUpComponent = XMVectorGetX(XMVector3Dot(vAttackDirection, vUp));
    _float fRotationAngle = atan2f(-fUpComponent, fRightComponent);


    // 4. Z축 회전된 Right와 Up 벡터 계산
    _float fCos = cosf(fRotationAngle);
    _float fSin = sinf(fRotationAngle);
    _vector vRotatedRight = XMVectorAdd(XMVectorScale(vRight, fCos), XMVectorScale(vUp, -fSin));
    _vector vRotatedUp = XMVectorAdd(XMVectorScale(vRight, fSin), XMVectorScale(vUp, fCos));

    // 5. TransformCom에 상태 설정 (한 번만)
    m_pTransformCom->Set_State(STATE::RIGHT, vRotatedRight);
    m_pTransformCom->Set_State(STATE::UP, vRotatedUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    
    _float fSizeX = m_vStartScale.x;
    _float fSizeY = m_vStartScale.y;
    m_pTransformCom->Set_Scale({ fSizeX, fSizeY, m_vStartScale.z });

    // 7. 계산 완료 플래그 설정
    m_IsDirectionCalculated = true;
}

HRESULT CRenketsuSlash::Bind_ShaderResources()
{


    // 쉐이더에 바인딩
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
    {
        CRASH("Failed Bind World Matrix LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind View Matrix LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind Proj Matrix LockOnUI");
        return E_FAIL;
    }

#pragma region TEXTURE
    if (FAILED(m_pTextureCom[DIFFUSE]->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTextures")))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }


    if (FAILED(m_pTextureCom[OTHER]->Bind_Shader_Resources(m_pShaderCom, "g_OtherTextures")))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom[MASK]->Bind_Shader_Resources(m_pShaderCom, "g_MaskTextures")))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }
#pragma endregion
    // 시간 진행도 계산 (0.0 ~ 1.0)
    _float fTimeRatio = m_fCurrentTime / m_fDisplayTime;

        // 시간에 따른 스케일 증가 (1.0 -> 0.3)
        _float fScale = 0.54f + (fTimeRatio * 1.f);

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeRatio", &fTimeRatio, sizeof(_float))))
        {
            CRASH("Failed Bind TimeRatio");
            return E_FAIL;
        }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fCurrentTime, sizeof(_float))))
    {
        CRASH("Failed Bind TimeRatio");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &fScale, sizeof(_float))))
    {
        CRASH("Failed Bind Scale");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomIntensity", &m_fBloomIntensity, sizeof(_float))))
    {
        CRASH("Failed Bind Scale");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenketsuSlash::Bind_DistortionShaderResources()
{
    // 트레일 정점은 이미 월드 좌표계로 변환된 상태이므로 단위 행렬 사용
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pDistortionShaderCom, "g_WorldMatrix")))
    {
        CRASH("Failed Bind Identity WorldMatrix");
        return E_FAIL;
    }

    if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind View Matrix SwordTrail");
        return E_FAIL;
    }

    if (FAILED(m_pDistortionShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind Proj Matrix SwordTrail");
        return E_FAIL;
    }

    // 시간 진행도 계산 (0.0 ~ 1.0)
    if (FAILED(m_pDistortionShaderCom->Bind_RawValue("g_fTime", &m_fCurrentTime, sizeof(_float))))
    {
        CRASH("Failed Bind Time");
        return E_FAIL;
    }
    return S_OK;
}


HRESULT CRenketsuSlash::Ready_Components()
{
    // 컴포넌트 추가
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxEffectPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
    
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxEffectPosTexDistortion"),
        TEXT("Com_DistortionShader"), reinterpret_cast<CComponent**>(&m_pDistortionShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    {
        CRASH("Failed Load VIBuffer");
        return E_FAIL;
    }

#pragma region 텍스쳐

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_RenketsuDiffuse"),
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[DIFFUSE]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_RenketsuOther"),
        TEXT("Com_OtherTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[OTHER]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_RenketsuMask"),
        TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[MASK]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }


    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashDistortion"),
        TEXT("Com_DistortionTexture"), reinterpret_cast<CComponent**>(&m_pDistortionTexture))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }
#pragma endregion


    return S_OK;
}


CRenketsuSlash* CRenketsuSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenketsuSlash* pInstance = new CRenketsuSlash(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CRenketsuSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRenketsuSlash::Clone(void* pArg)
{
    CRenketsuSlash* pInstance = new CRenketsuSlash(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CRenketsuSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenketsuSlash::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pDistortionShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pDistortionTexture);

    
}

#ifdef _DEBUG
void CRenketsuSlash::ImGui_Render()
{
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window
    ImVec2 windowPos = ImVec2(300.f, 300.f);
    ImVec2 windowSize = ImVec2(300.f, 300.f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    ImGui::Begin("Renketsu Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    _vector vMine = m_pTransformCom->Get_State(STATE::POSITION);
    XMStoreFloat3(&vPos, vMine);
    ImGui::Text("Renketsu Pos: (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);


    ImGui::End();

}
#endif // _DEBUG


