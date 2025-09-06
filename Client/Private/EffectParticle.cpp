CEffectParticle::CEffectParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffectParticle::CEffectParticle(const CEffectParticle& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffectParticle::Initialize_Prototype()
{
    if (FAILED(CGameObject::Initialize_Prototype()))
    {
        CRASH("Failed Clone CEffectParticle");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CEffectParticle::Initialize_Clone(void* pArg)
{
    EFFECT_PARTICLE_DESC* pDesc = static_cast<EFFECT_PARTICLE_DESC*>(pArg);
    if (FAILED(CGameObject::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone CEffectParticle");
        return E_FAIL;
    }

    m_eCurLevel = pDesc->eCurLevel;
   

#pragma region 값 채우기.
    m_fDisplayTime = pDesc->vLifeTime.y;
    m_isLoop = pDesc->isLoop;
    m_eParticleType = pDesc->eParticleType;
    m_iShaderPath = pDesc->iShaderPath;
    m_isBillBoard = pDesc->isBillBoard;

    /* 사용할 텍스쳐의 인덱스를 지정해줍니다. */
    for (_uint i = 0; i < TEXTURE::TEXTURE_END; ++i)
        m_iTextureIndexArray[i] = pDesc->useTextureIndexArray[i];


    // 위치 설정.
    // m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPosition);
#pragma endregion


    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Clone CEffectParticle");
        return E_FAIL;
    }
        
    return S_OK;
}

void CEffectParticle::Priority_Update(_float fTimeDelta)
{
    CGameObject::Priority_Update(fTimeDelta);
}

void CEffectParticle::Update(_float fTimeDelta)
{
    CGameObject::Update(fTimeDelta);


#pragma region 파티클 타입 에 따른 업데이트

    m_pVIBufferCom->Update(fTimeDelta);
#pragma endregion


#pragma region 타이머 업데이트
    // 타이머 업데이트
    m_fCurrentTime += fTimeDelta;

    // 활성 파티클 수 확인
    _uint liveParticleCount = m_pVIBufferCom->Get_LiveParticleCount();
    
    // 모든 파티클이 죽었거나 시간이 지나면 비활성화
    if (liveParticleCount == 0 || m_fCurrentTime >= m_fDisplayTime)
    {
        // Loop면 다시 시작.
        if (m_isLoop)
        {
            m_fCurrentTime = 0.f;
        }
        else
        {
            m_IsActivate = false;
            Reset_Timer(); // 타이머 초기화.
            return;
        }
    }
#pragma endregion
  
    
}

void CEffectParticle::Late_Update(_float fTimeDelta)
{
    CGameObject::Late_Update(fTimeDelta);

    // 활성화된 상태에서만 렌더링
    if (m_IsActivate)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        {
            CRASH("Failed Add RenderGroup");
            return;
        }
    }
}

HRESULT CEffectParticle::Render()
{
#ifdef _DEBUG
    ImGui_Render();
#endif // _DEBUG

    HRESULT hr;
    hr = Bind_ShaderResources();
    if (FAILED(hr))
    {
        CRASH("Failed Bind Shader Resources");
        return E_FAIL;
    }
    
    hr = m_pShaderCom->Begin(m_iShaderPath);
    //hr = m_pShaderCom->Begin(m_iShaderPath);
    if (FAILED(hr))
    {
        CRASH("Failed Shader Begin Failed");
        return E_FAIL;
    }

    hr = m_pVIBufferCom->Bind_Resources();
    if (FAILED(hr))
    {
        CRASH("Failed Bind_Resources Failed");
        return E_FAIL;
    }

    hr = m_pVIBufferCom->Render();
    if (FAILED(hr))
    {
        CRASH("Failed Render Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffectParticle::Bind_ShaderResources()
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

HRESULT CEffectParticle::Ready_Components(const EFFECT_PARTICLE_DESC* pDesc)
{

    /* 1. VIBuffer Point 생성. */
    if (FAILED(Ready_VIBuffer_Point(pDesc)))
    {
        CRASH("Failed VIBuffer Point");
        return E_FAIL;
    }

    /* 나머지 컴포넌트 채우기. */

#pragma region 1. Shader 초기화
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointDirParticle"),
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

            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), Effect_TexturePrototypes[i].prototypeName
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

HRESULT CEffectParticle::Ready_VIBuffer_Point(const EFFECT_PARTICLE_DESC* pDesc)
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

    m_fDisplayTime = PointDesc.vLifeTime.y + 1.0f;  // 개별 파티클보다 1초 더 길게
    
    XMStoreFloat3(&PointDesc.vDir, pDesc->vDirection);

    m_pVIBufferCom = CVIBuffer_PointParticleDir_Instance::Create(m_pDevice, m_pContext, &PointDesc);
    if (nullptr == m_pVIBufferCom)
    {
        CRASH("Failed Create VIBuffer PointDir Instance");
        return E_FAIL;
    }

    m_pVIBufferCom->Create_Buffer();

    return S_OK;
}


void CEffectParticle::CreateDefault_Particle(_float3 vCenterPosition, _float3 vBaseDirection, _float fLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->CreateAllParticles(vCenterPosition, vBaseDirection, fLifeTime);
    }
}

void CEffectParticle::CreateBurstEffect(_float3 vGatherPoint, _float3 vUpDirection, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->CreateBurstParticles(vGatherPoint, vUpDirection, fGatherTime, fBurstTime, fTotalLifeTime);
    }
}

void CEffectParticle::Create_QueenKnightWarpEffect(const PARTICLE_INIT_INFO particleInitInfo)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->Create_QueenKnightWarpParticle(particleInitInfo);
    }
}

void CEffectParticle::Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fGatherTime, _float fExplosionTime, _float fTotalLifeTime)
{
    if (m_pVIBufferCom)
    {
        m_pVIBufferCom->Create_BossExplosionParticle(vCenterPos, fRadius, fGatherTime, fExplosionTime, fTotalLifeTime);
    }
}


#pragma region POOLING에서 꺼내질때 필요한 작업 정의 
void CEffectParticle::OnActivate(void* pArg)
{
    EFFECTPARTICLE_ENTER_DESC* pDesc = static_cast<EFFECTPARTICLE_ENTER_DESC*>(pArg);

    // 1. 위치 지정. => WorldMatrix용
    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vStartPos);

    // 2. 활성화 상태로 설정
    m_IsActivate = true;
    Reset_Timer(); // 타이머 초기화
    
    const PARTICLE_INIT_INFO& particleInit = pDesc->particleInitInfo;

    // 2. 타입에 맞는 Create 함수 호출
    switch (m_eParticleType)
    {
    case PARTICLE_TYPE_DEFAULT:
        m_pVIBufferCom->CreateAllParticles(particleInit.pos, particleInit.dir, particleInit.lifeTime);
        break;
    case PARTICLE_TYPE_QUEEN_WARP:
        m_pVIBufferCom->Create_QueenKnightWarpParticle(particleInit);
        break;

    case PARTICLE_TYPE_BOSS_EXPLOSION:
        m_pVIBufferCom->Create_BossExplosionParticle(particleInit.pos, particleInit.fRadius, particleInit.fGatherTime
            , particleInit.fExplositionTime, particleInit.lifeTime);
        break;

    }

}

void CEffectParticle::OnDeActivate()
{
    // Activate할때 적용함.
    m_pGameInstance->Add_GameObject_ToPools(TEXT("QUEENKNIGHT_WARP"), ENUM_CLASS(CHitFlashEffect::EffectType), this);
}

#pragma endregion

CEffectParticle* CEffectParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectParticle* pInstance = new CEffectParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffectParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectParticle::Clone(void* pArg)
{
    CEffectParticle* pInstance = new CEffectParticle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffectParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffectParticle::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);
    Safe_Release(m_pVIBufferCom);
}

#ifdef _DEBUG
void CEffectParticle::ImGui_Render()
{
    ImGuiIO& io = ImGui::GetIO();

    // 기존 Player Debug Window
    ImVec2 windowSize = ImVec2(300.f, 00.f);
    ImVec2 windowPos = ImVec2(0.f, 0.f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    string strDebug = "QueenKnight Effect Debug";
    ImGui::Begin(strDebug.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    _float3 vPos = {};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("POS : (%.2f, %.2f, %.2f)", vPos.x, vPos.y, vPos.z);

    ImGui::Text("IsActivate : %s", m_IsActivate ? "TRUE" : "FALSE");
    ImGui::Text("ParticleType : %d", m_eParticleType);
    ImGui::Text("ShaderPath : %d", m_iShaderPath);
    ImGui::Text("CurrentTime : %.2f / %.2f", m_fCurrentTime, m_fDisplayTime);

    if (m_pVIBufferCom)
    {
        ImGui::Text("VIBuffer : Valid");
        // 파티클 인스턴스 수 확인 (VIBuffer 클래스에 getter가 있다면)
        ImGui::Text("Active Particles: %d", m_pVIBufferCom->Get_LiveParticleCount());


    }
    else
    {
        ImGui::Text("VIBuffer : NULL");
    }

    ImGui::End();
}
#endif // _DEBUG
