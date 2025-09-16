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
    m_ViewPortDesc = ViewportDesc;
    

    m_fThreShold = 0.9f;
    m_fSoft = 0.3f;

#pragma region 후처리 쉐이딩 Render Target 추가.
    /* For.Target_Diffuse */
    // Clear Color 가 알파 1.f라서 discard가 되어지지 않습니다. => 후처리 시에는 Diffuse의 알파 Clear 값을 0으로

    m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f));
        
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
    //m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Combine_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 0.f, 1.f, 1.f));

    m_pGameInstance->Add_RenderTarget(TEXT("Target_Distotion"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 0.f, 1.f, 1.f));


    m_pGameInstance->Add_RenderTarget(TEXT("Target_BrightPass"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlurX"), ViewportDesc.Width / 8, ViewportDesc.Height / 8, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlurY"), ViewportDesc.Width / 8, ViewportDesc.Height / 8, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    
        

    /* GameObjects*/
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"));

    m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"));
    m_pGameInstance->Add_MRT(TEXT("MRT_Combine"), TEXT("Combine_Shade"));
    //m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"));

    m_pGameInstance->Add_MRT(TEXT("MRT_BrightPass"), TEXT("Target_BrightPass"));
    m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlurX"), TEXT("Target_BloomBlurX"));
    m_pGameInstance->Add_MRT(TEXT("MRT_BloomBlurY"), TEXT("Target_BloomBlurY"));
    


#pragma endregion


    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    ASSERT_CRASH(m_pVIBuffer);

    m_pDefferedShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    ASSERT_CRASH(m_pDefferedShader);

    m_pDistortionShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Distortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    ASSERT_CRASH(m_pDistortionShader);

    m_pPostLightShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_BloomBright.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    ASSERT_CRASH(m_pPostLightShader);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#pragma region DEBUGING 용도 Render Target 화면 추가
#ifdef _DEBUG
    
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 150.0f, 750.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Combine_Shade"), 450.0f, 450.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distotion"), 450.0f, 750.0f, 300.f, 300.f);
    //m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), ViewportDesc.Width - 150.0f, 150.0f, 300.f, 300.f);
    
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_BrightPass"), ViewportDesc.Width - 150.0f, 150.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlurX"), ViewportDesc.Width - 150.0f, 450.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_BloomBlurY"), ViewportDesc.Width - 150.0f, 750.0f, 300.f, 300.f);
        
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
    if (FAILED(Render_Priority()))
        return E_FAIL;

    // 1. G-Buffer 생성 (불투명 객체)
    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    // 2. 조명 계산
    if (FAILED(Render_Lights()))
        return E_FAIL;
    //
    // 3. G-Buffer 합성 및 스카이박스 렌더링으로 최종 씬 완성
    if (FAILED(Render_Combined()))
        return E_FAIL;
    
    // 4. Distotion 추가.
    if (FAILED(Render_Distortion()))
        return E_FAIL;

    // 5. 모든 반투명 객체(HitFlashEffect 포함)를 최종 씬 위에 렌더링
    if (FAILED(Render_Blend()))
        return E_FAIL;

   

    // 6. 반투명 객체까지 포함된 씬으로 블룸 효과 생성
    if (FAILED(Render_BloomBlur()))
        return E_FAIL;
    //
    // 7. 씬과 블룸 효과를 최종 합성하여 화면에 출력
    if (FAILED(Render_BloomCombine()))
        return E_FAIL;
//   
    // 8. NonLight,  등 나머지 렌더링 => 사실상 안씀.
    if (FAILED(Render_NonLight()))
        return E_FAIL;


    // EX) 나머지 효과를 적용하지 않을 객체들 그려넣기.
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

void CRenderer::Setting_Threshold(_float fThreShold)
{
    m_fThreShold = fThreShold;
}

void CRenderer::Setting_Soft(_float fSoft)
{
    m_fSoft = fSoft;
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
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;
    
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }
    m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();

    //m_pGameInstance->End_MRT();


    return S_OK;
}

//HRESULT CRenderer::Render_Shadow()
//{
//    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"))))
//        return E_FAIL;
//
//    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
//    {
//        if (nullptr != pRenderObject)
//            pRenderObject->Render();
//
//        Safe_Release(pRenderObject);
//    }
//
//    m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();
//
//    if (FAILED(m_pGameInstance->End_MRT()))
//        return E_FAIL;
//
//    return S_OK;
//}

HRESULT CRenderer::Render_NonBlend()
{
    // 후처리 쉐이딩.
    /* Diffuse + Normal */
    //if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
    //    return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();

    m_pGameInstance->End_MRT();


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
    m_pDefferedShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
        
    m_pDefferedShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
   
        
    m_pDefferedShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pDefferedShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW));
 
        
    m_pDefferedShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ));
        
    m_pDefferedShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pDefferedShader, "g_NormalTexture");
    
    // Depth 기록 => 이부분 다름.
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pDefferedShader, "g_DepthTexture");

    m_pGameInstance->Render_Lights(m_pDefferedShader, m_pVIBuffer);
    m_pGameInstance->End_MRT();

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{ 
    // 1. Combine_Shade에 렌더링 시작 => Begin MRT 사용시 RenderTarget을 Clear하므로 Begin_MRT를 조심해서 쓸것.
    m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"));

    m_pDefferedShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pDefferedShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pDefferedShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pDefferedShader, "g_DiffuseTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pDefferedShader, "g_ShadeTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Emissive"), m_pDefferedShader, "g_EmissiveTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pDefferedShader, "g_DepthTexture");
    //m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_SkyBox"), m_pDefferedShader, "g_SkyTexture");
        

    _uint iCombinedShaderIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::COMBINED);
    m_pDefferedShader->Begin(iCombinedShaderIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();
    m_pGameInstance->End_MRT();

    
    return S_OK;
}

//[추가] 왜곡 효과 렌더링
// Render_Combined의 결과물을 배경으로 사용
HRESULT CRenderer::Render_Distortion()
{
    // 1. Combine Texture에 Blend 객체들 덮어 그리기.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"), false)))
        return E_FAIL;


    m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
        {
            return pSour->Get_CamDistance() > pDest->Get_CamDistance();
        });

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)].clear();

    // 2. 최종 Combine Texture 완료.
    m_pGameInstance->End_MRT();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    // 1. Combine Texture에 Blend 객체들 덮어 그리기.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"), false)))
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

    // 2. 최종 Combine Texture 완료.
    m_pGameInstance->End_MRT();

    return S_OK;
}

#pragma region 디버깅 용도 Combine Texture만 출력.
HRESULT CRenderer::Render_DebugCombined()
{
    // 1. 화면 전체에 사각형을 그리기 위한 행렬들을 바인딩합니다.
    m_pDefferedShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pDefferedShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pDefferedShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    // 2. 우리가 보고 싶은 'Combine_Shade' 텍스처를 셰이더의 g_Texture 변수에 바인딩합니다.
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Combine_Shade"), m_pDefferedShader, "g_Texture");

    // 3. 단순 텍스처 출력용 셰이더 패스(0번, Debug Pass)를 시작합니다.
    _uint iDebugPassIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::DEBUG);
    m_pDefferedShader->Begin(iDebugPassIndex);

    // 4. 사각형 버퍼를 그리고 렌더링합니다.
    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    return S_OK;
}
#pragma endregion



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

HRESULT CRenderer::Render_BloomBlur()
{
    D3D11_VIEWPORT originalVP = m_ViewPortDesc;

    // Target_BrightPass에 밝은 부분만 추출
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BrightPass"))))
        return E_FAIL;

    // 다운 샘플링된 월드행렬을 그대로 사용해서. 
    if (FAILED(m_pPostLightShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostLightShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostLightShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pPostLightShader->Bind_RawValue("threshold", &m_fThreShold, sizeof(_float));
    m_pPostLightShader->Bind_RawValue("soft", &m_fSoft, sizeof(_float));




    // 원본 씬 텍스처를 바인딩 => Combine Shader
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Combine_Shade"), m_pPostLightShader, "g_DiffuseTexture")))
        return E_FAIL;

    // Bright Pass 쉐이더로 렌더링
    _uint iBloomExtractBrgihtindex = static_cast<_uint>(SHADER_POSTLIGHT::BLOOM_EXTRACT_BRGIHT);
    m_pPostLightShader->Begin(iBloomExtractBrgihtindex);
    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    // 수평 블러 (BrightPass -> BloomBlur1)
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlurX"))))
        return E_FAIL;

    // 1/4 크기로 스케일 조정
     // 쿼터 해상도 뷰포트 생성
    /*D3D11_VIEWPORT quarterVP = m_ViewPortDesc;
    quarterVP.Width = m_ViewPortDesc.Width / 4.f;
    quarterVP.Height = m_ViewPortDesc.Height / 4.f;
    quarterVP.TopLeftX = 0.f;
    quarterVP.TopLeftY = 0.f;
    quarterVP.MinDepth = 0.f;
    quarterVP.MaxDepth = 1.f;*/

    D3D11_VIEWPORT quarterVP = m_ViewPortDesc;
    quarterVP.Width = m_ViewPortDesc.Width / 8.f;
    quarterVP.Height = m_ViewPortDesc.Height / 8.f;
    quarterVP.TopLeftX = 0.f;
    quarterVP.TopLeftY = 0.f;
    quarterVP.MinDepth = 0.f;
    quarterVP.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &quarterVP);


    m_pPostLightShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pPostLightShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pPostLightShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BrightPass"), m_pPostLightShader, "g_DiffuseTexture");
    _uint iHorizontalBlurIndex = static_cast<_uint>(SHADER_POSTLIGHT::GAUSSIAN_BLURX);
    m_pPostLightShader->Begin(iHorizontalBlurIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();



    // 수직 블러 (BloomBlur1 -> BloomBlur2)
    m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlurY"));

    m_pPostLightShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pPostLightShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pPostLightShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BloomBlurX"), m_pPostLightShader, "g_DiffuseTexture");

    _uint iVerticalBlurIndex = static_cast<_uint>(SHADER_POSTLIGHT::GAUSSIAN_BLURY);
    m_pPostLightShader->Begin(iVerticalBlurIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    m_pGameInstance->End_MRT();

    m_pContext->RSSetViewports(1, &originalVP);

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



HRESULT CRenderer::Render_BloomCombine()
{

    D3D11_VIEWPORT ViewPort{};
    _uint iNumViewports = { 1 };
    m_pContext->RSGetViewports(&iNumViewports, & ViewPort);
    // 백버퍼에 최종 합성
    m_pPostLightShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pPostLightShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pPostLightShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    // 원본 씬과 블룸 텍스처를 바인딩
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Combine_Shade"), m_pPostLightShader, "g_DiffuseTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BloomBlurY"), m_pPostLightShader, "g_BloomTexture");

    _uint iBloomCombineIndex = static_cast<_uint>(SHADER_POSTLIGHT::SUM_BLUR);
    m_pPostLightShader->Begin(iBloomCombineIndex);

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


    if (FAILED(m_pDefferedShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pDefferedShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pGameInstance->Render_RT_Debug(m_pDefferedShader, m_pVIBuffer);

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

    Safe_Release(m_pDefferedShader);
    Safe_Release(m_pDistortionShader);
    Safe_Release(m_pPostLightShader);
    Safe_Release(m_pVIBuffer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
