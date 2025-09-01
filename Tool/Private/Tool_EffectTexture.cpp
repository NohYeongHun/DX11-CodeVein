CTool_EffectTexture::CTool_EffectTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTool_EffectTexture::CTool_EffectTexture(const CTool_EffectTexture& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTool_EffectTexture::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Initialize Prototype Slash UI");
        return E_FAIL;
    }



    return S_OK;
}

HRESULT CTool_EffectTexture::Initialize_Clone(void* pArg)
{
    TOOLEFFECT_TEXTURE_DESC* pDesc = static_cast<TOOLEFFECT_TEXTURE_DESC*>(pArg);

    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone ToolEffectTexture");
        return E_FAIL;
    }

#pragma region 값 채우기
    m_eCurLevel = pDesc->eCurLevel;
    m_vHitDirection = pDesc->vHitDirection;
    m_vScale = pDesc->vScale;
    m_fDisplayTime = pDesc->fDisPlayTime;
    m_iShaderPath = pDesc->iShaderPath;
    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vHitPosition); // 처음에 위치 설정.

    /* 사용할 텍스쳐의 인덱스를 지정해줍니다. */
    for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
        m_iTextureIndexArray[i] = pDesc->useTextureIndexArray[i];
#pragma endregion


    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready_Components");
        return E_FAIL;
    }

    Initialize_Transform();
    m_IsActivate = true;

    return S_OK;
}

void CTool_EffectTexture::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CTool_EffectTexture::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);

    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;

    // 시간이 지나면 비활성화

    if (m_fCurrentTime >= m_fDisplayTime)
    {
        m_IsActivate = false;
        Set_Destroy(true);
        return;
    }

}

void CTool_EffectTexture::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);

    // UI 렌더 그룹에 추가
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CTool_EffectTexture::Render()
{
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
        CRASH("Ready Render Effect Texture Failed");
        return E_FAIL;
    }

    return S_OK;
}


void CTool_EffectTexture::Initialize_Transform()
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
    m_pTransformCom->Set_Scale({ m_vScale.x, m_vScale.y, m_vScale.z });

    // 7. 계산 완료 플래그 설정
    m_bDirectionCalculated = true;
}

HRESULT CTool_EffectTexture::Bind_ShaderResources()
{
#pragma region 행렬 바인딩
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
#pragma endregion

#pragma region Texture 바인딩
    // 동적으로 생성 되어있으므로(없을 수도 있음.)
    for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
    {
        /* nullptr이 아닌 경우에만 바인딩. */
        if (m_pTextureCom[i] != nullptr)
        {
            switch (i)
            {
            case TEXTURE::TEXTURE_DIFFUSE:
            {
                if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Diffuse Texture ");
                    return E_FAIL;
                }
            }
                break;
            case TEXTURE::TEXTURE_GRADIENT:
            {
                if (FAILED(m_pTextureCom[TEXTURE_GRADIENT]->Bind_Shader_Resource(m_pShaderCom, "g_GradientTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture ");
                    return E_FAIL;
                }
            }
                break;
            case TEXTURE::TEXTURE_GRADIENT_ALPHA:
            {
                if (FAILED(m_pTextureCom[TEXTURE_GRADIENT_ALPHA]->Bind_Shader_Resource(m_pShaderCom, "g_GradientAlphaTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture Alpha");
                    return E_FAIL;
                }
            }
                break;
            case TEXTURE::TEXTURE_MASK:
            {
                if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture Mask");
                    return E_FAIL;
                }
            }
                break;
            }
        }
    }
#pragma endregion

#pragma region 변수 바인딩.

    // 시간 진행도 계산 (0.0 ~ 1.0) => g_fTimeRatio
    _float fTimeRatio = m_fCurrentTime / m_fDisplayTime;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeRatio", &fTimeRatio, sizeof(_float))))
    {
        CRASH("Failed Bind TimeRatio");
        return E_FAIL;
    }

    // 크기 바인딩.
    _float fScaleRatio = 1.0f - (fTimeRatio * 0.7f);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fScaleRatio", &fScaleRatio, sizeof(_float))))
    {
        CRASH("Failed Bind Scale");
        return E_FAIL;
    }

#pragma endregion



    return S_OK;
}

HRESULT CTool_EffectTexture::Ready_Components(TOOLEFFECT_TEXTURE_DESC* pDesc)
{
#pragma region Shader 초기화
    if (FAILED(Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
#pragma endregion

#pragma region VIBuffer 초기화.
    if (FAILED(Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    {
        CRASH("Failed Load VIBuffer");
        return E_FAIL;
    }
#pragma endregion

#pragma region TEXTURE 초기화

    for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
    {
        if (pDesc->useTextureCheckArray[i])
        {
            _wstring strTextureTag = {};
            switch (i)
            {
            case TEXTURE::TEXTURE_DIFFUSE:
                {
                    strTextureTag = TEXT("Com_DiffuseTexture");
                    break;
                }
            case TEXTURE::TEXTURE_GRADIENT:
                {
                    strTextureTag = TEXT("Com_GradientTexture");
                    break;
                }
            case TEXTURE::TEXTURE_GRADIENT_ALPHA:
                {
                    strTextureTag = TEXT("Com_GradientAlphaTexture");
                    break;
                }
            case TEXTURE::TEXTURE_MASK:
                {
                    strTextureTag = TEXT("Com_MaskTexture");
                    break;
                }
            default:
                break;
            }
            
            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurLevel), Effect_TexturePrototypes[i].prototypeName
                , strTextureTag, reinterpret_cast<CComponent**>(&m_pTextureCom[i]))))
            {
                CRASH("Failed LoadTexture");
                return E_FAIL;
            }
        }
    }
#pragma endregion

    return S_OK;
}

CTool_EffectTexture* CTool_EffectTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_EffectTexture* pInstance = new CTool_EffectTexture(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTool_EffectTexture"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTool_EffectTexture::Clone(void* pArg)
{
    CTool_EffectTexture* pInstance = new CTool_EffectTexture(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTool_EffectTexture"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTool_EffectTexture::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);
}
