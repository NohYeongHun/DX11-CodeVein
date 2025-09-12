CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance {CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

}

HRESULT CRenderer::Initialize()
{

    // Render Target 초기화
    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    

#pragma region 후처리 쉐이딩 Render Target 추가.
    /* For.Target_Diffuse */
    // Clear Color 가 알파 1.f라서 discard가 되어지지 않습니다. => 후처리 시에는 Diffuse의 알파 Clear 값을 0으로
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
    {
        CRASH("Failed Add RenderTarget Diffuse");
        return E_FAIL;
    }

    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget Normal");
        return E_FAIL;
    }

    /* For.Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget Depth");
        return E_FAIL;
    }
        

    /* For.Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget Shade");
        return E_FAIL;
    }

    /* For.Target_Specular */
    //if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
    //{
    //    CRASH("Failed Add RenderTarget Specular");
    //    return E_FAIL;
    //}

      /* For.Target_LightDepth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

#pragma region BLOOM 용도 렌더 타겟들
    /* For.Target_BrightPass */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BrightPass"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget BrightPass");
        return E_FAIL;
    }

    /* For.Target_BloomBlur1 */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlur1"), ViewportDesc.Width / 4, ViewportDesc.Height / 4, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget BloomBlur1");
        return E_FAIL;
    }

    /* For.Target_BloomBlur2 */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlur2"), ViewportDesc.Width / 4, ViewportDesc.Height / 4, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget BloomBlur2");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SceneColor"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
    {
        CRASH("Failed Add RenderTarget Screen Color");
        return E_FAIL;
    }
#pragma endregion
        

    /* For.MRT_GameObjects : 게임 오브젝트들의 정보를 저장받기위한 타겟들 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;


       /* For.MRT_LightAcc : 빛들의 연산 결과를 누적한다. */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
    //    return E_FAIL;


      /* For.MRT_Shadow : 광원기준으로 보여지는 장면을 그려준다.  */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
        return E_FAIL;

#pragma region BLOOM 데이터

    /* For.MRT_BrightPass : Bloom용 밝은 부분 추출 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BrightPass"), TEXT("Target_BrightPass"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlur1"), TEXT("Target_BloomBlur1"))))
        return E_FAIL;

    /* For.MRT_BloomBlur1 : Bloom용 첫 번째 블러 패스 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlur2"), TEXT("Target_BloomBlur2"))))
        return E_FAIL;

    /* For.MRT_BloomBlur2 : Bloom용 두 번째 블러 패스 */
    

    /* For.MRT_FinalScene : 최종 장면 렌더링 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_FinalScene"), TEXT("Target_SceneColor"))))
        return E_FAIL;
#pragma endregion


#pragma endregion


    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#pragma region DEBUGING 용도 Render Target 화면 추가
#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f)))
    {
        CRASH("Faield Target Diffuse Ready");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
    {
        CRASH("Faield Target Normal Ready");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
    {
        CRASH("Faield Target Shade Ready");
        return E_FAIL;
    }

    //if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.0f, 450.0f, 300.f, 300.f)))
    //{ 
    //    CRASH("Faield Target_Specular Ready");
    //    return E_FAIL;
    //}

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), ViewportDesc.Width - 150.0f, 150.0f, 300.f, 300.f)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BrightPass"), ViewportDesc.Width - 150.0f, 450.0f, 300.f, 300.f)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlur1"), ViewportDesc.Width - 150.0f, 450.0f, 300.f, 300.f)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlur2"), ViewportDesc.Width - 150.0f, 750.0f, 300.f, 300.f)))
    {
        return E_FAIL;
    }
        

    //if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), ViewportDesc.Width - 150.0f, 150.0f, 300.f, 300.f)))
    //    return E_FAIL;
        
#endif
#pragma endregion



    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

    Safe_AddRef(pRenderObject);
    
    return S_OK;
}

// Renderer.cpp
HRESULT CRenderer::Draw()
{
    // 0. Priority 렌더링 (스카이박스 등) - 백버퍼에 직접

    // 1. G-Buffer 생성 (불투명 객체)
    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    // 2. 조명 계산
    if (FAILED(Render_Lights()))
        return E_FAIL;

    // 3. G-Buffer 합성 및 스카이박스 렌더링으로 최종 씬 완성
    if (FAILED(Render_Combined()))
        return E_FAIL;
  

    if (FAILED(Render_Priority()))
        return E_FAIL;


    // 4. 모든 반투명 객체(HitFlashEffect 포함)를 최종 씬 위에 렌더링
    if (FAILED(Render_Blend()))
        return E_FAIL;



#pragma region BLOOM
    // 5. 반투명 객체까지 포함된 씬으로 블룸 효과 생성
    if (FAILED(Render_Bloom()))
        return E_FAIL;
   
    // 6. 씬과 블룸 효과를 최종 합성하여 화면에 출력
    if (FAILED(Render_PostProcess()))
        return E_FAIL;
#pragma endregion
   
    // 7. NonLight, UI 등 나머지 렌더링
    if (FAILED(Render_NonLight()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;

    if (FAILED(Render_StaticUI()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_Debug()))
        return E_FAIL;
#endif

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
    m_DebugComponent.push_back(pComponent);

    Safe_AddRef(pComponent);

    return S_OK;
}

#endif // _DEBUG


HRESULT CRenderer::Render_Priority()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FinalScene"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }
    m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    // 후처리 쉐이딩.
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
    {
        CRASH("Failed Begin MRT_GameObjects");
        return E_FAIL;
    }
        

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    /* Shade타겟을 채워줄 수 있는 리소스를 그려주자 == 쉐이드타겟의 모든 픽셀에 값을 채우낟. */
    /* Shade타겟 (wsx * wsy) */
    /* 삭가형버퍼 -> 직교투영 -> (wsx * wsy) */
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    {
        CRASH("Failed Binding WorldMatrix");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
    {
        CRASH("Failed Binding ViewMatrix");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        CRASH("Failed Binding Projection Matrix");
        return E_FAIL;
    }

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
    {
        CRASH("Failed Binding ViewMatrix Inverse");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
    {
        CRASH("Failed Binding ProjMatrix Inverse");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
    {
        CRASH("Failed Binding ProjMatrix Inverse");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
    {
        CRASH("Failed Target Normal Binding");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
    {
        CRASH("Failed Target Depth Binding");
        return E_FAIL;
    }

    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);



    

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{ 
    // 1. Target_SceneColor에 렌더링 시작
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FinalScene"))))
        return E_FAIL;

    // 2. Priority 그룹은 이미 Draw() 시작에서 처리됨
    /*for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)]) {
        if (nullptr != pRenderObject) pRenderObject->Render();
        Safe_Release(pRenderObject);
    }
    m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();*/


    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
    {
        CRASH("Failed Bind Target_Shade");
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
    {
        CRASH("Failed Bind Target_Combined");
        return E_FAIL;
    }
        

    //if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
    //    return E_FAIL;

    _uint iCombinedShaderIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::COMBINED);
    m_pShader->Begin(iCombinedShaderIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    
    return S_OK;
}

/* Post Process 로 Rendering 진행. */
HRESULT CRenderer::Render_PostProcess()
{
    // Bloom과 원본 씬 합성하여 백버퍼에 최종 출력
    if (FAILED(Render_BloomCombine()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    // 1. Target_SceneColor에 렌더링 시작
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FinalScene"), false)))
        return E_FAIL;


    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
    {
        return pSour->Get_CamDistance() > pDest->Get_CamDistance();
    });

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].clear();

    // 2. 렌더링이 끝났으므로 MRT 종료


    // 1. Target_SceneColor에 렌더링 시작
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    return S_OK;
}


HRESULT CRenderer::Render_UI()
{

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_StaticUI()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC_UI)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC_UI)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_BloomObjects()
{
    // BLOOM 그룹 객체들을 Target_SceneColor에 추가로 렌더링 (MRT는 Combined에서 이미 시작됨)
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLOOM)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }
    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLOOM)].clear();

    // 이제 MRT 종료
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}


HRESULT CRenderer::Render_Bloom()
{
    // 1. 밝은 부분 추출
    if (FAILED(Render_BrightPass()))
        return E_FAIL;

    // 2. 블러 처리
    if (FAILED(Render_BloomBlur()))
        return E_FAIL;



    return S_OK;
}

HRESULT CRenderer::Render_BrightPass()
{
    // Target_BrightPass에 밝은 부분만 추출
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BrightPass"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 원본 씬 텍스처를 바인딩
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_SceneColor"), m_pShader, "g_sceneTexture")))
        return E_FAIL;

    // Bright Pass 쉐이더로 렌더링
    _uint iBrightPassIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::BRIGHT_PASS);
    m_pShader->Begin(iBrightPassIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BloomBlur()
{
    // 수평 블러 (BrightPass -> BloomBlur1)
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlur1"))))
        return E_FAIL;

    // 1/4 크기로 스케일 조정
    _float4x4 BlurWorldMatrix;
    //XMStoreFloat4x4(&BlurWorldMatrix, XMMatrixScaling(1280.f / 4.f, 720.f / 4.f, 1.f));
    XMStoreFloat4x4(&BlurWorldMatrix, XMMatrixScaling(1920.f / 4.f, 1080.f / 4.f, 1.f));
    
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &BlurWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BrightPass"), m_pShader, "g_sceneTexture")))
        return E_FAIL;

    _uint iHorizontalBlurIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::BLUR_HORIZONTAL);
    m_pShader->Begin(iHorizontalBlurIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // 수직 블러 (BloomBlur1 -> BloomBlur2)
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlur2"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &BlurWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BloomBlur1"), m_pShader, "g_sceneTexture")))
        return E_FAIL;

    _uint iVerticalBlurIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::BLUR_VERTICAL);
    m_pShader->Begin(iVerticalBlurIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BloomCombine()
{
    // 백버퍼에 최종 합성
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 원본 씬과 블룸 텍스처를 바인딩
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_SceneColor"), m_pShader, "g_sceneTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BloomBlur2"), m_pShader, "g_BloomTexture")))
        return E_FAIL;

    _uint iBloomCombineIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::BLOOM_COMBINE);
    m_pShader->Begin(iBloomCombineIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{

    for (auto& pDebugCom : m_DebugComponent)
    {
        if (nullptr != pDebugCom)
            pDebugCom->Render();

        Safe_Release(pDebugCom);
    }
    m_DebugComponent.clear();


    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pGameInstance->Render_RT_Debug(m_pShader, m_pVIBuffer);

    return S_OK;
}

#endif



CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CRenderer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRenderer::Free()
{
    CBase::Free();

#ifdef _DEBUG
    // Debug list 비우기.
    for (auto& pDebugComponent : m_DebugComponent)
        Safe_Release(pDebugComponent);
    m_DebugComponent.clear();
#endif // _DEBUG



    for (size_t i = 0; i < ENUM_CLASS(RENDERGROUP::END); i++)
    {
        for (auto& pRenderObject : m_RenderObjects[i])
            Safe_Release(pRenderObject);

        m_RenderObjects[i].clear();
    }

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
