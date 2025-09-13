CSlash::CSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSlash::CSlash(const CSlash& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CSlash::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype Slash UI");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSlash::Initialize_Clone(void* pArg)
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

    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);



    return S_OK;
}

void CSlash::Priority_Update(_float fTimeDelta)
{   
    if (!m_IsActivate)
        return;

    CGameObject::Priority_Update(fTimeDelta);
}

void CSlash::Update(_float fTimeDelta)
{

    if (!m_IsActivate)
        return;

    CGameObject::Update(fTimeDelta);

    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;
    
    // 시간이 지나면 비활성화
    if (m_fCurrentTime >= m_fDisplayTime)
    {
        m_IsActivate = false;
        Reset_Timer();
        
        return;
    }

    
}

void CSlash::Late_Update(_float fTimeDelta)
{
    if (!m_IsActivate)
        return;

    CGameObject::Late_Update(fTimeDelta);

    // UI 렌더 그룹에 추가
    //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
    //{
    //    CRASH("Failed Add_RenderGroup Slash");
    //    return;
    //}

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
    {
        CRASH("Failed Add_RenderGroup Slash");
        return;
    }
        
}

HRESULT CSlash::Render()
{
    if (!m_IsActivate)
    {
        // Active, Target도 아닌데 Render되고 있다면 문제가 있음.
        CRASH("Failed Render Slash");
        return S_OK;
    }


    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Ready Bind Shader Resources Failed");
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

#pragma region POOLING 전용 함수
void CSlash::OnActivate(void* pArg)
{
    // 1. Activate하는데 필요한 값 설정
    SLASHACTIVATE_DESC* pDesc = static_cast<SLASHACTIVATE_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;
    m_vHitDirection = pDesc->vHitDirection;

    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vHitPosition);
    m_fDisplayTime = pDesc->fDisPlayTime;
    // 2. 설정되었을때 Camera에 따른 방향을 재계산할 필요성이 존재.
    m_bDirectionCalculated = false;
    m_vScale = pDesc->vScale;
    // 3. 위치 및 회전계산.
    Initialize_Transform();

    m_IsActivate = true;
}

// Deactivate시 필요한 정보가? 딱히 없을듯.
void CSlash::OnDeActivate()
{
    m_pGameInstance->Add_GameObject_ToPools(TEXT("SLASH_EFFECT"), ENUM_CLASS(CSlash::EffectType), this);
}
#pragma endregion



void CSlash::Initialize_Transform()
{
    // 이미 계산되었다면 다시 계산하지 않음
    if (m_bDirectionCalculated)
        return;

    // 1. 카메라 위치 가져오기
    _vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
    _vector vSlashPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 빌보드 기본 벡터 계산 (카메라를 향하도록)
    _vector vLook = XMVector3Normalize(vSlashPos - vCamPos);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 3. 공격 방향을 빌보드 평면에 투영하여 회전 각도 계산
    _vector vAttackDirection = XMVector3Normalize(m_vHitDirection);
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

    // 6. 크기 설정.
    //_float fSizeX = m_vScale.x + static_cast<float>(rand()) / RAND_MAX * 1.5f; // 1.5f ~ 3.f
    //_float fSizeY = m_vScale.y + static_cast<float>(rand()) / RAND_MAX * 0.2f; // 0.2f ~ 0.3f

    //_float fSizeX = m_pGameInstance->Rand(1.5f, 3.f); // 1.5f ~ 3.f
    //_float fSizeY = 0.15f;
    _float fSizeX = m_vScale.x;
    _float fSizeY = m_vScale.y;
    m_pTransformCom->Set_Scale({ fSizeX, fSizeY, m_vScale.z });

    // 7. 계산 완료 플래그 설정
    m_bDirectionCalculated = true;
}

HRESULT CSlash::Bind_ShaderResources()
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
    if (FAILED(m_pTextureCom[DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }


    if (FAILED(m_pTextureCom[OTHER]->Bind_Shader_Resources(m_pShaderCom, "g_OtherTexture")))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }
#pragma endregion

   /* if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 0)))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }


    if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
    {
        CRASH("Failed Bind Texture LockOnUI");
        return E_FAIL;
    }*/

    // 시간 진행도 계산 (0.0 ~ 1.0)
    _float fTimeRatio = m_fCurrentTime / m_fDisplayTime;
    
    // 시간에 따른 스케일 증가 (1.0 -> 0.3)
    _float fScale = 0.54f + (fTimeRatio * 1.f);
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeRatio", &fTimeRatio, sizeof(_float))))
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


HRESULT CSlash::Ready_Components()
{
    // 컴포넌트 추가
    /*if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }*/

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxEffectPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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
    /* Resource */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashEffectDiffuse"),
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[DIFFUSE]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashEffectOther"),
        TEXT("Com_OtherTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[OTHER]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }
#pragma endregion






    //if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashEffectMask"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK]))))
    //{
    //    CRASH("Failed Load Texture");
    //    return E_FAIL;
    //}

    //if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SlashEffectDiffuse"),
    //    TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
    //{
    //    CRASH("Failed Load Texture");
    //    return E_FAIL;
    //}

    return S_OK;
}


CSlash* CSlash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlash* pInstance = new CSlash(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSlash::Clone(void* pArg)
{
    CSlash* pInstance = new CSlash(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlash::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);

    
}

#ifdef _DEBUG
void CSlash::ImGui_Render()
{
}
#endif // _DEBUG


