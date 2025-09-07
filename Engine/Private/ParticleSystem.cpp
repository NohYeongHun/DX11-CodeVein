#pragma region 0. 기본 함수들
CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect{ pDevice, pContext }
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& Prototype)
    : CEffect(Prototype)
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
    if (FAILED(CEffect::Initialize_Prototype()))
    {
        CRASH("Failed Initialize_Prototype");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CParticleSystem::Initialize_Clone(void* pArg)
{
    PARTICLESYSTEM_CLONE_DESC* pDesc = static_cast<PARTICLESYSTEM_CLONE_DESC*>(pArg);
    // 여기서 부모의 트랜스폼 컴포넌트 주소를 받아옵니다. => 부모의 월드 좌표가 필요하므로?
    if (FAILED(CEffect::Initialize_Clone(pDesc)))
    {
        CRASH("Failed Clone Particle System");
        return E_FAIL;
    }

    m_tLayOutDesc = *pDesc;
    // 최대 Display 시간 설정.
    m_fLifeMaxTime = pDesc->vLifeTime.y; 

    if (FAILED(Ready_Components(pDesc)))
    {
        CRASH("Failed Ready Components");
        return E_FAIL;
    }

   

    return S_OK;
}

void CParticleSystem::Priority_Update(_float fTimeDelta)
{
    CEffect::Priority_Update(fTimeDelta);
}

void CParticleSystem::Update(_float fTimeDelta)
{
    CEffect::Update(fTimeDelta);

    // 1. VIBuffer에 데이터를 갱신해주는 로직.
    Update_Particles_Logic(fTimeDelta);

    // 2. 시간이 다되면 Pool에 재입장
    Calc_Timer(fTimeDelta);
}

void CParticleSystem::Late_Update(_float fTimeDelta)
{
    CEffect::Late_Update(fTimeDelta);

    if (m_eAttachType == EAttachType::PARENT)
        XMStoreFloat4x4(&m_CombinedWorldMatrix
            , m_pTransformCom->Get_WorldMatrix() * m_pOwnerTransform->Get_WorldMatrix());
    else
        XMStoreFloat4x4(&m_CombinedWorldMatrix,m_pTransformCom->Get_WorldMatrix());

    if (m_IsActivate)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        {
            CRASH("Failed Add RenderGroup");
            return;
        }
    }
}

HRESULT CParticleSystem::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        CRASH("Failed Bind ShaderResources ");
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

#pragma endregion


#pragma region 파티클 업데이트 로직
void CParticleSystem::Update_Particles_Logic(_float fTimeDelta)
{
    // --- 1. 생성 (Emission) ---
    // Emitter에게 이번 프레임에 생성할 파티클을 m_Particles 벡터에 추가해달라고 요청합니다.
    if (nullptr != m_pParticleEmitter)
    {
        m_pParticleEmitter->Emit(fTimeDelta, m_Particles);
    }

    // --- 2. 갱신 (Simulation) ---
    for (auto& particle : m_Particles)
    {
        // 2-1. 수명 갱신 (현재 살아온 시간)
        particle.vLife.x += fTimeDelta;

        // 2-2. 위치 갱신 (기본 물리)
        XMStoreFloat3(&particle.vPosition
            , (XMLoadFloat3(&particle.vPosition) + XMLoadFloat3(&particle.vDir) * particle.fSpeed * fTimeDelta));

        // 2-3. 속성 보간(Interpolation) - 시간에 따른 변화
        // 파티클이 전체 수명(vLife.y) 중 얼마나 살았는지 비율(0.0 ~ 1.0)을 계산합니다.
        particle.fLifeRatio = particle.vLife.x / particle.vLife.y;
        if (particle.fLifeRatio > 1.f) particle.fLifeRatio = 1.f; // 1을 넘지 않도록 Clamp

        // 이 비율을 사용해 시작값과 끝값 사이를 보간(Lerp)합니다.
        // 예시: 색상 보간 (시작 색상에서 끝 색상으로 자연스럽게 변함)
        // DirectX::XMVectorLerp 함수나 직접 구현한 Lerp 함수를 사용합니다.
        // particle.vColor = Lerp(m_tDesc.vColorStart, m_tDesc.vColorEnd, lifeRatio);

        // 예시: 크기 보간 (시작 크기에서 끝 크기로 자연스럽게 변함)
        // particle.fSize = m_tDesc.vSize.x + (m_tDesc.vSize.y - m_tDesc.vSize.x) * lifeRatio;
    }

    // --- 3. 제거 (Removal) ---
    // 수명이 다 된 파티클을 효율적으로 제거합니다. (Erase-Remove Idiom)
    // C++에서 벡터의 원소를 순회하며 제거할 때 가장 표준적이고 빠른 방법입니다.
    m_Particles.erase(
        std::remove_if(m_Particles.begin(), m_Particles.end(),
            [](const PARTICLE_DATA& p) {
                // vLife.x (현재 수명)가 vLife.y (최대 수명)를 넘으면 true를 반환 -> 제거 대상
                return p.vLife.x >= p.vLife.y;
            }),
        m_Particles.end()
    );

    // --- 4. 최종 VIBuffer 업데이트 ---
    if (!m_Particles.empty() && nullptr != m_pVIBufferCom)
    {
        _matrix finalTransformMatrix = m_pTransformCom->Get_WorldMatrix();

        // 만약 주인을 따라다니는 타입이라면, 주인의 월드 행렬을 추가로 곱해준다.
        if (m_eAttachType == EAttachType::PARENT && nullptr != m_pOwnerTransform)
        {
            finalTransformMatrix *= m_pOwnerTransform->Get_WorldMatrix();
        }

        // VIBuffer에게 파티클 데이터와 '최종 변환 행렬'을 함께 넘겨준다.
        m_pVIBufferCom->Update_Buffer(m_Particles, finalTransformMatrix);
    }
}
#pragma endregion


#pragma region 1. 풀링 고려
void CParticleSystem::OnMoved_ToObjectManager()
{
}

void CParticleSystem::OnActivate(void* pArg)
{
    PARTICLESYSTEM_ACTIVATE_DESC* pParticleDesc = static_cast<PARTICLESYSTEM_ACTIVATE_DESC*>(pArg);
    if (nullptr == pParticleDesc)
    {
        CRASH("Failed OnActivate");
    }

    CEffect::OnActivate(pParticleDesc);

    m_tDesc = *pParticleDesc;
    
    // 0. 전체 소멸 시간 지정
    m_fLifeTime = m_tLayOutDesc.vLifeTime.y;

    // 1. '레시피(DESC)'를 다시 적용하여 재설정 (가장 중요)
    m_eAttachType = pParticleDesc->eAttachType;
    m_pOwnerTransform = pParticleDesc->pOwnerTransform;

    // 2. 파티클 데이터 초기화.
    m_Particles.clear();

     // 3. Emitter 초기화
    // Emitter는 내가 이미 가지고 있는 '레시피 북'(m_tDesc)을 참조하여 리셋된다.
    if (m_pParticleEmitter != nullptr)
    {
        m_pParticleEmitter->Reset(&m_tLayOutDesc, m_pOwnerTransform);
    }

    m_IsActivate = true;
}

// Obj Manager의 Layer에서 Activate가 false면 자동 호출.
void CParticleSystem::OnDeActivate()
{
    CEffect::OnDeActivate();
    m_pGameInstance->Add_GameObject_ToPools(m_PoolTag, ENUM_CLASS(EFFECTTYPE::PARTICLE), this);
}

void CParticleSystem::Calc_Timer(_float fTimeDelta)
{
    if (m_fLifeTime <= 0.f)
    {
        m_IsActivate = false;
        Reset_Timer();
    }
    else
    {
        m_fLifeTime -= fTimeDelta;
    }
}
#pragma endregion

#pragma region 0. 기본 함수들
HRESULT CParticleSystem::Ready_Components(PARTICLESYSTEM_CLONE_DESC* pDesc)
{


#pragma region 0. Emitter 정의

    // 프리셋 등을 통해 Emitter DESC를 정의
    CParticleEmitter::EMITTER_DESC tEmitterDesc = {};
    tEmitterDesc.eEmissionType = pDesc->eEmissionType;
    tEmitterDesc.eEmitterShape = pDesc->eEmitterShape;
    tEmitterDesc.iEmissionRate = pDesc->iEmissionRate;
    tEmitterDesc.iBurstCount = pDesc->iBurstCount;
    tEmitterDesc.vShapeSize = pDesc->vShapeSize;
    tEmitterDesc.fConeAngle = pDesc->fConeAngle;


    m_pParticleEmitter = CParticleEmitter::Create(&tEmitterDesc);
    if (nullptr == m_pParticleEmitter)
    {
        CRASH("Failed Particle Emitter");
        return E_FAIL;
    }

#pragma endregion

#pragma region SHADER 초기화
    if (FAILED(Add_Component(pDesc->iComponentPrototypeLevel, pDesc->strShaderPrototypeTag,
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        CRASH("Failed Load Shader");
        return E_FAIL;
    }
#pragma endregion

#pragma region TEXTURE 초기화
    for (_uint i = 0; i < TEXTURE_END; ++i)
    {
        // Texture들을 사용한다면?
        if (pDesc->useTextureCheckArray[i])
        {
            _wstring strTextureTag = {};
            switch (i)
            {
            case TEXTURE_DIFFUSE:
            {
                strTextureTag = TEXT("Com_DiffuseTexture");
                break;
            }
            case TEXTURE_GRADIENT:
            {
                strTextureTag = TEXT("Com_GradientTexture");
                break;
            }
            case TEXTURE_GRADIENT_ALPHA:
            {
                strTextureTag = TEXT("Com_GradientAlphaTexture");
                break;
            }
            case TEXTURE_MASK:
            {
                strTextureTag = TEXT("Com_MaskTexture");
                break;
            }
            default:
                break;
            }

            if (FAILED(CGameObject::Add_Component(pDesc->iComponentPrototypeLevel
                , pDesc->strTexturePrototypeTag[i], strTextureTag
                , reinterpret_cast<CComponent**>(&m_pTextureCom[i]))))
            {
                CRASH("Failed LoadTexture");
                return E_FAIL;
            }
        }

    }
#pragma endregion



    return S_OK;
}

HRESULT CParticleSystem::Bind_ShaderResources()
{

#pragma region 행렬 바인딩
    // 쉐이더에 바인딩
    // 지정하기 전에 WorldMatrix를 명확하게 초기화해주어야 합니다.
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
    for (_uint i = 0; i < TEXTURE_END; ++i)
    {
        /* nullptr이 아닌 경우에만 바인딩. */
        if (m_pTextureCom[i] != nullptr)
        {
            switch (i)
            {
            case TEXTURE_DIFFUSE:
            {
                if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Diffuse Texture ");
                    return E_FAIL;
                }
            }
            break;
            case TEXTURE_GRADIENT:
            {
                if (FAILED(m_pTextureCom[TEXTURE_GRADIENT]->Bind_Shader_Resource(m_pShaderCom, "g_GradientTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture ");
                    return E_FAIL;
                }
            }
            break;
            case TEXTURE_GRADIENT_ALPHA:
            {
                if (FAILED(m_pTextureCom[TEXTURE_GRADIENT_ALPHA]->Bind_Shader_Resource(m_pShaderCom, "g_GradientAlphaTexture", m_iTextureIndexArray[i])))
                {
                    CRASH("Failed Bind Texture Gradient Texture Alpha");
                    return E_FAIL;
                }
            }
            break;
            case TEXTURE_MASK:
            {
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
    _float fTimeRatio = m_fLifeTime / m_fLifeMaxTime;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeRatio", &fTimeRatio, sizeof(_float))))
    {
        CRASH("Failed Bind TimeRatio");
        return E_FAIL;
    }

    //// 크기 바인딩.
    //_float fScaleRatio = 1.0f - (fTimeRatio * 0.7f);
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_fScaleRatio", &fScaleRatio, sizeof(_float))))
    //{
    //    CRASH("Failed Bind Scale");
    //    return E_FAIL;
    //}
#pragma endregion

    return S_OK;
}



CParticleSystem* CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleSystem* pInstance = new CParticleSystem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CParticleSystem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleSystem::Clone(void* pArg)
{
    CParticleSystem* pInstance = new CParticleSystem(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticleSystem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticleSystem::Free()
{
    CEffect::Free();
    Safe_Release(m_pParticleEmitter);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);

    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);

}

#pragma endregion




