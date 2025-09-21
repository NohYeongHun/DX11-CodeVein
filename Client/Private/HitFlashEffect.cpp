CHitFlashEffect::CHitFlashEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CHitFlashEffect::CHitFlashEffect(const CHitFlashEffect& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CHitFlashEffect::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype CHitFlashEffect");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHitFlashEffect::Initialize_Clone(void* pArg)
{
    HITFLASH_DESC* pDesc = static_cast<HITFLASH_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone CHitFlashEffect");
        return E_FAIL;
    }
    m_eCurLevel = pDesc->eCurLevel;
    m_iShaderPath = static_cast<_uint>(pDesc->eShaderPath);
    
    // 강제로 HitFlash 패스(1번)로 설정
    m_iShaderPath = 1;

    if (FAILED(Ready_Components()))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }


    return S_OK;
}

void CHitFlashEffect::Priority_Update(_float fTimeDelta)
{
    if (!m_IsActivate)
        return;

    CGameObject::Priority_Update(fTimeDelta);

}

void CHitFlashEffect::Update(_float fTimeDelta)
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

    m_fFrameTime += fTimeDelta;

    if (m_fFrameTime >= fTimeDelta * 2.f)
    {
        m_fFrameTime = 0.f;
        m_iCurFrame = (m_iCurFrame + 1) % 16;
    }

  /*  _float3 vScale = m_pTransformCom->Get_Scale();
    XMStoreFloat3(&vScale, XMLoadFloat3(&vScale) * (1.f + fTimeDelta));
    m_pTransformCom->Set_Scale(vScale);*/
    
}

void CHitFlashEffect::Late_Update(_float fTimeDelta)
{
    if (!m_IsActivate)
        return;

    CGameObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;

}

HRESULT CHitFlashEffect::Render()
{
    if (!m_IsActivate)
    {
        CRASH("Failed Render CHitFlashEffect");
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
        CRASH("Ready Render CHitFlashEffect Failed");
        return E_FAIL;
    }

    return S_OK;
}

#pragma region 풀링 전용 함수.
void CHitFlashEffect::OnActivate(void* pArg)
{
    // 1. Activate하는데 필요한 값 설정
    HITFLASHACTIVATE_DESC* pDesc = static_cast<HITFLASHACTIVATE_DESC*>(pArg);
    m_eCurLevel = pDesc->eCurLevel;
    m_vHitDirection = pDesc->vHitDirection;
    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vHitPosition);
    m_fDisplayTime = pDesc->fDisPlayTime;
    

    // 2. 설정되었을때 Camera에 따른 방향을 재계산할 필요성이 존재.
    m_IsDirectionCalculated = false;
    m_vScale = pDesc->vScale;


    // 3. 위치 및 회전계산.
    Initialize_Transform();
    m_IsActivate = true;
}

void CHitFlashEffect::OnDeActivate()
{
    m_pGameInstance->Add_GameObject_ToPools(TEXT("HITFLASH_EFFECT"), ENUM_CLASS(CHitFlashEffect::EffectType), this);
}
#pragma endregion


/* 타격 위치 계산. */
void CHitFlashEffect::Initialize_Transform()
{
    // 이미 계산되었다면 다시 계산하지 않음
    if (m_IsDirectionCalculated)
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
    _float fSizeX = m_vScale.x + static_cast<float>(rand()) / RAND_MAX * 1.5f; // 1.5f ~ 3.f
    _float fSizeY = m_vScale.y + static_cast<float>(rand()) / RAND_MAX * 0.2f; // 0.2f ~ 0.3f
    m_pTransformCom->Set_Scale({ fSizeX, fSizeY, m_vScale.z });

    // 7. 계산 완료 플래그 설정
    m_IsDirectionCalculated = true;
}

HRESULT CHitFlashEffect::Bind_ShaderResources()
{

#pragma region 기본 행렬 바인딩
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
    {
        CRASH("Failed Bind World Matrix CHitFlashEffect");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind View Matrix CHitFlashEffect");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind Proj Matrix CHitFlashEffect");
        return E_FAIL;
    }

#pragma region 변수 바인딩.

    // 시간 진행도 계산 (0.0 ~ 1.0)
    _float fTimeRatio = m_fCurrentTime / m_fDisplayTime;

    // 계산된 시간 값을 셰이더의 g_fTimeRatio 변수로 전달
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeRatio", &fTimeRatio, sizeof(_float))))
    {
        CRASH("Failed Bind TimeRatio");
        return E_FAIL;
    }

    // Bloom 효과를 위한 밝기 배수 (1.5 이상이면 Bloom 적용)
    _float fBloomIntensity = 2.0f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomIntensity", &fBloomIntensity, sizeof(_float))))
    {
        CRASH("Failed Bind BloomIntensity");
        return E_FAIL;
    }
#pragma endregion


#pragma region TEXTURE 바인딩.
    // 텍스쳐 바인딩
    if (FAILED(m_pTextureCom[DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
    {
        CRASH("Failed Bind Texture");
        return E_FAIL;
    }
        

    if (FAILED(m_pTextureCom[OPACITY]->Bind_Shader_Resource(m_pShaderCom, "g_OpacityTexture", 0)))
    {
        CRASH("Failed Bind Texture");
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom[OTHER]->Bind_Shader_Resources(m_pShaderCom, "g_OtherTexture")))
    {
        CRASH("Failed Bind Texture");
        return E_FAIL;
    }
#pragma endregion




    return S_OK;
}

HRESULT CHitFlashEffect::Ready_Components()
{
    // 컴포넌트 추가
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


    // 1. Diffuse
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HitFlashDiffuse"),
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[DIFFUSE]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }

    // 2. Opacity (Mask)
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HitFlashOpacity"),
        TEXT("Com_OpacityTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[OPACITY]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    } 

    // 3. Other 4개있음.
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HitFlashOther"),
        TEXT("Com_OtherTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[OTHER]))))
    {
        CRASH("Failed Load Texture");
        return E_FAIL;
    }


    return S_OK;
}

CHitFlashEffect* CHitFlashEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHitFlashEffect* pInstance = new CHitFlashEffect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CHitFlashEffect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHitFlashEffect::Clone(void* pArg)
{
    CHitFlashEffect* pInstance = new CHitFlashEffect(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CHitFlashEffect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHitFlashEffect::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);
}
