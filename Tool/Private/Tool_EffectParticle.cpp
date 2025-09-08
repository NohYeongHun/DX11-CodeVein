CTool_EffectParticle::CTool_EffectParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTool_EffectParticle::CTool_EffectParticle(const CTool_EffectTexture& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTool_EffectParticle::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Clone CTool_EffectParticle");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CTool_EffectParticle::Initialize_Clone(void* pArg)
{
    TOOLEFFECT_PARTICLE_DESC* pDesc = static_cast<TOOLEFFECT_PARTICLE_DESC*>(pArg);

    pDesc->fSpeedPerSec = 5.f;
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);
    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone CTool_EffectParticle");
        return E_FAIL;
    }

    m_eCurLevel = pDesc->eCurLevel;
   

#pragma region 값 채우기.
    m_fDisplayTime = pDesc->vLifeTime.y; // 최대 시간.
    m_isLoop = pDesc->isLoop;
    m_eParticleType = pDesc->eParticleType;
    m_iShaderPath = pDesc->iShaderPath;
    m_isBillBoard = pDesc->isBillBoard;

    m_fEmissiveIntencity = pDesc->fEmissiveIntencity;
    
    XMStoreFloat3(&m_vDir, pDesc->vObjectDir); // 방향 지정.

    /* 사용할 텍스쳐의 인덱스를 지정해줍니다. */
    for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
        m_iTextureIndexArray[i] = pDesc->useTextureIndexArray[i];


    // 위치 설정. => 생성당시 한번만 설정하면 안될거 같은데?
    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPosition);
#pragma endregion


    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Clone CTool_EffectParticle");
        return E_FAIL;
    }
        
    return S_OK;
}

void CTool_EffectParticle::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CTool_EffectParticle::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);


#pragma region 파티클 타입 에 따른 업데이트

    // 업데이트 이전에 현재 정보 전달.
    
    _vector vDirection = XMVector3Normalize(XMLoadFloat3(&m_vDir));
    m_pTransformCom->Move_Direction(vDirection, fTimeDelta * 1.f); // 지정된 방향으로 날라감.

    //_fvector vPos = m_pTransformCom->Get_State(STATE::POSITION); // 이동 이후 위치 제공.
    //m_pVIBufferCom->Bind_Transform(vPos); // Shader에서 정점들에 WorldMatrix를 이미 곱해주고 있음.
    m_pVIBufferCom->Update(fTimeDelta);
#pragma endregion

    

#pragma region 타이머 업데이트
    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;

    // 시간이 지나면 비활성화
    if (m_fCurrentTime >= m_fDisplayTime)
    {
        // Loop면 다시 시작.
        if (m_isLoop)
        {
            m_fCurrentTime = 0.f;
        }
        else
        {
            m_IsActivate = false;
            Set_Destroy(true);
            return;
        }
    }
#pragma endregion
    
}

void CTool_EffectParticle::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
    {
        CRASH("Failed Add RenderGroup");
        return;
    }

}

HRESULT CTool_EffectParticle::Render()
{
#ifdef _DEBUG
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window

    ImVec2 windowSize = ImVec2(300.f, 300.f);
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = "QueenKnight Effect Debug";
    ImGui::Begin(strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("POS : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    ImGui::End();

#endif // _DEBUG


    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Failed Bind Shader Resources");
        return E_FAIL;
    }
        

    if (FAILED(m_pShaderCom->Begin(m_iShaderPath)))
    {
        CRASH("Failed Shader Begin Failed");
        return E_FAIL;
    }


    if (FAILED(m_pVIBufferCom->Bind_Resources()))
    {
        CRASH("Failed Bind_Resources Failed");
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render()))
    {
        CRASH("Failed Render Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CTool_EffectParticle::Bind_ShaderResources()
{

#pragma region 행렬 바인딩
    // 쉐이더에 바인딩
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
    {
        CRASH("Failed Bind World Matrix");
        return E_FAIL;
    }


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Bind View Matrix");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
    {
        CRASH("Failed Bind Proj Matrix");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
    {
        CRASH("Failed Bind Cam Position");
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &m_fEmissiveIntencity, sizeof(_float))))
    {
        CRASH("Failed Bind Cam Position");
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
                /*if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_Shader_Resources(m_pShaderCom, "g_MaskTextures", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture Mask");
                    return E_FAIL;
                }*/

                if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_Shader_Resources(m_pShaderCom, "g_MaskTextures")))
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

HRESULT CTool_EffectParticle::Ready_Components(const TOOLEFFECT_PARTICLE_DESC* pDesc)
{
    /* 1. VIBuffer Point 생성. */
    if (FAILED(Ready_VIBuffer_Point(pDesc)))
    {
        CRASH("Failed VIBuffer Point");
        return E_FAIL;
    }

    /* 나머지 컴포넌트 채우기. */

#pragma region 1. Shader 초기화
    if (FAILED(Add_Component(ENUM_CLASS(m_eCurLevel), TEXT("Prototype_Component_Shader_VtxInstance_PointDirParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
#pragma endregion

#pragma region 2.TEXTURE 초기화

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

HRESULT CTool_EffectParticle::Ready_VIBuffer_Point(const TOOLEFFECT_PARTICLE_DESC* pDesc)
{
    CVIBuffer_PointParticleDir_Instance::PARTICLEPOINTDIR_INSTANCE_DESC PointDesc{};
    PointDesc.vPivot = pDesc->vPivot;
    PointDesc.vSpeed = pDesc->vSpeed;
    PointDesc.iNumInstance = pDesc->iNumInstance;
    PointDesc.vCenter = pDesc->vCenter;
    PointDesc.vRange = pDesc->vRange;
    PointDesc.vSize = pDesc->vSize;
    PointDesc.vLifeTime = pDesc->vLifeTime;
    PointDesc.isLoop = pDesc->isLoop;
    
    PointDesc.eParticleType = static_cast<CVIBuffer_PointParticleDir_Instance::PARTICLE_TYPE>(pDesc->eParticleType);
    PointDesc.IsSpawn = pDesc->isSpawn;
    PointDesc.fSpawnInterval = pDesc->fSpawnInterval;
    PointDesc.iSpawnCount = pDesc->iSpawnCount;
    
    XMStoreFloat3(&PointDesc.vDir, pDesc->vParticleDir);

    m_pVIBufferCom = CVIBuffer_PointParticleDir_Instance::Create(m_pDevice, m_pContext, &PointDesc);
    if (nullptr == m_pVIBufferCom)
    {
        CRASH("Failed Create VIBuffer PointDir Instance");
        return E_FAIL;
    }

    m_pVIBufferCom->Create_Buffer();

    return S_OK;
}


void CTool_EffectParticle::CreateDefault_Particle(_float3 vCenterPosition, _float3 vBaseDirection, _float fLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->CreateAllParticles(vCenterPosition, vBaseDirection, fLifeTime);
    }
}

void CTool_EffectParticle::CreateBurstEffect(_float3 vGatherPoint, _float3 vUpDirection, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->CreateBurstParticles(vGatherPoint, vUpDirection, fGatherTime, fBurstTime, fTotalLifeTime);
    }
}

void CTool_EffectParticle::Create_QueenKnightWarpEffect(const PARTICLE_INIT_INFO particleInitInfo)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->Create_QueenKnightWarpParticle(particleInitInfo);
    }
}

void CTool_EffectParticle::Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fExplosionTime, _float fTotalLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->Create_BossExplosionParticle(vCenterPos, fRadius, fExplosionTime, fTotalLifeTime);
    }
}

void CTool_EffectParticle::Create_TestParticle(const PARTICLE_TEST_INFO particleTestInfo)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->Create_TestParticle(particleTestInfo);
    }
}

CTool_EffectParticle* CTool_EffectParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_EffectParticle* pInstance = new CTool_EffectParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTool_EffectParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTool_EffectParticle::Clone(void* pArg)
{
    CTool_EffectParticle* pInstance = new CTool_EffectParticle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTool_EffectParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTool_EffectParticle::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);
}
