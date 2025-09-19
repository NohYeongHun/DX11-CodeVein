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
    
    m_fViewportWidth = ViewportDesc.Width;
    m_fViewportHeight = ViewportDesc.Height;

    m_fThreShold = 0.9f;
    m_fSoft = 0.3f;

#pragma region 후처리 쉐이딩 Render Target 추가.
    /* For.Target_Diffuse */
    // Clear Color 가 알파 1.f라서 discard가 되어지지 않습니다. => 후처리 시에는 Diffuse의 알파 Clear 값을 0으로

    m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f));
        
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));
    /* Target Light Depth */
    m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iMaxWidth, g_iMaxHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f));
    m_pGameInstance->Add_RenderTarget(TEXT("Combine_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f));


    m_pGameInstance->Add_RenderTarget(TEXT("Target_BrightPass"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlurX"), ViewportDesc.Width / 8, ViewportDesc.Height / 8, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    m_pGameInstance->Add_RenderTarget(TEXT("Target_BloomBlurY"), ViewportDesc.Width / 8, ViewportDesc.Height / 8, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f));
    
        

    /* GameObjects*/
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"));
    m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"));

    m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"));
    m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"));

    /* MRT Shadow 생성*/
    m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"));
    m_pGameInstance->Add_MRT(TEXT("MRT_Combine"), TEXT("Combine_Shade"));
    

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

    /* 쉐도우 용도 Depth Stencil View 생성*/
    if (FAILED(Ready_Shadow_Depth_Stencil_View()))
        return E_FAIL;

#pragma region DEBUGING 용도 Render Target 화면 추가
#ifdef _DEBUG
    //m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.0f, 150.0f, 300.f, 300.f); // 오른쪽 위에서 첫번째.

    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 150.0f, 750.0f, 300.f, 300.f);
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f); // 오른쪽 위에서 첫번째.
    
    m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), 450.f, 450.0f, 300.f, 300.f); // 그림자 전용
    m_pGameInstance->Ready_RT_Debug(TEXT("Combine_Shade"), 450.0f, 750.0f, 300.f, 300.f);
    
    
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
    if (FAILED(Render_Shadow()))
        return E_FAIL;

    // 0. Priority 렌더링 (스카이박스 등) - 백버퍼에 직접
    if (FAILED(Render_Priority()))
        return E_FAIL;
    

    // 1. G-Buffer 생성 (불투명 객체)
    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    // 2. 조명 계산
    if (FAILED(Render_Lights()))
        return E_FAIL;
    

    // 3. G-Buffer 합성 및 스카이박스 렌더링으로 최종 씬 완성
    if (FAILED(Render_Combined()))
        return E_FAIL;
    
    // 4. Distotion 추가.
    //if (FAILED(Render_Distortion()))
    //    return E_FAIL;

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

    // 8. NonLight,  등 나머지 렌더링 => 사실상 안씀.
    if (FAILED(Render_NonLight()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_Debug()))
        return E_FAIL;
#endif



    // EX) 나머지 효과를 적용하지 않을 객체들 그려넣기.
    if (FAILED(Render_UI()))
        return E_FAIL;

    if (FAILED(Render_StaticUI()))
        return E_FAIL;

    if (FAILED(Render_LastEffect()))
        return E_FAIL;

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


HRESULT CRenderer::Render_Shadow()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
        return E_FAIL;

    if (FAILED(SetUp_Viewport(g_iMaxWidth, g_iMaxHeight)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render_Shadow();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"), nullptr)))
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
    // [추가] Light Pass에서 입력으로 사용할 G-Buffer 텍스처들을 미리 해제합니다.
   // g_NormalTexture(0번), g_DepthTexture(1번) 슬롯을 해제한다고 가정합니다.
    /*ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);*/

    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"), nullptr)))
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
    // [추가] Combine Pass에서 입력으로 사용할 모든 G-Buffer와 조명 텍스처들을 미리 해제합니다.
   // 보통 5-6개 이상 사용되므로, 넉넉하게 8개 슬롯을 모두 비웁니다.
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    // 1. Combine_Shade에 렌더링 시작 => Begin MRT 사용시 RenderTarget을 Clear하므로 Begin_MRT를 조심해서 쓸것.
    m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"), nullptr);

    m_pDefferedShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pDefferedShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pDefferedShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix); 

    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pDefferedShader, "g_DiffuseTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pDefferedShader, "g_ShadeTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pDefferedShader, "g_SpecularTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Emissive"), m_pDefferedShader, "g_EmissiveTexture");
    m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pDefferedShader, "g_DepthTexture");
    

#pragma region 그림자 정보 바인딩
    if (FAILED(m_pDefferedShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pDefferedShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_LightDepth"), m_pDefferedShader, "g_LightDepthTexture")))
        return E_FAIL;
#pragma endregion



    _uint iCombinedShaderIndex = static_cast<_uint>(DEFFERED_SHADERTYPE::COMBINED);
    m_pDefferedShader->Begin(iCombinedShaderIndex);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();
    m_pGameInstance->End_MRT();

    
    return S_OK;
}

//[추가] 왜곡 효과 렌더링
// Render_Combined의 결과물을 배경으로 사용
//HRESULT CRenderer::Render_Distortion()
//{
//    // 1. Combine Texture에 Blend 객체들 덮어 그리기.
//    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"),nullptr, false)))
//        return E_FAIL;
//
//
//    m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
//        {
//            return pSour->Get_CamDistance() > pDest->Get_CamDistance();
//        });
//
//    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)])
//    {
//        if (nullptr != pRenderObject)
//            pRenderObject->Render();
//
//        Safe_Release(pRenderObject);
//    }
//
//    m_RenderObjects[ENUM_CLASS(RENDERGROUP::DISTOTION)].clear();
//
//    // 2. 최종 Combine Texture 완료.
//    m_pGameInstance->End_MRT();
//
//    return S_OK;
//}

HRESULT CRenderer::Render_Blend()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    // 1. Combine Texture에 Blend 객체들 덮어 그리기.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combine"), nullptr, false)))
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
    // 0. 원본 뷰포트 정보 저장
    D3D11_VIEWPORT originalVP = m_ViewPortDesc;


    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    // Target_BrightPass에 밝은 부분만 추출
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BrightPass"), nullptr)))
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


    // --- 2. Horizontal Blur ---
   // [수정] 다음 단계의 입력이 될 'Target_BrightPass'가 바인딩된 슬롯을 미리 해제합니다.
    m_pContext->PSSetShaderResources(0, 1, &pNullSRVs[0]); // g_DiffuseTexture 슬롯(0번) 해제


    // 수평 블러 (BrightPass -> BloomBlur1)
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlurX"), nullptr)))
        return E_FAIL;

     // 쿼터 해상도 뷰포트 생성

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


    // --- 3. Vertical Blur ---
    // [수정] 다음 단계의 입력이 될 'Target_BloomBlurX'가 바인딩된 슬롯을 미리 해제합니다.
    m_pContext->PSSetShaderResources(0, 1, &pNullSRVs[0]); // g_DiffuseTexture 슬롯(0번) 해제

    // 수직 블러 (BloomBlur1 -> BloomBlur2)
    m_pGameInstance->Begin_MRT(TEXT("MRT_BloomBlurY"), nullptr);

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

HRESULT CRenderer::Render_BloomCombine()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);


    D3D11_VIEWPORT ViewPort{};
    _uint iNumViewports = { 1 };
    m_pContext->RSGetViewports(&iNumViewports, &ViewPort);
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



HRESULT CRenderer::Render_UI()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

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
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC_UI)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC_UI)].clear();

    return S_OK;
}


HRESULT CRenderer::Render_LastEffect()
{
    ID3D11ShaderResourceView* pNullSRVs[8] = { nullptr, nullptr };
    m_pContext->PSSetShaderResources(0, 8, pNullSRVs);

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::LAST_EFFECT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::LAST_EFFECT)].clear();

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

    //m_pGameInstance->Render_RT_Debug(m_pDefferedShader, m_pVIBuffer);

    return S_OK;
}


#endif

HRESULT CRenderer::Ready_Shadow_Depth_Stencil_View()
{
    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    TextureDesc.Width = g_iMaxWidth;
    TextureDesc.Height = g_iMaxHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    /* 동적? 정적?  */
    TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
    /* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
        /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    /* RenderTargetView */
    /* ShaderResourceView */
    /* DepthStencilView */

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);


    return S_OK;
}

HRESULT CRenderer::SetUp_Viewport(_float fWidth, _float fHeight)
{
    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = fWidth;
    ViewPortDesc.Height = fHeight;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    return S_OK;
}


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

    Safe_Release(m_pShadowDSV); // 그림자전용.

    Safe_Release(m_pDefferedShader);
    Safe_Release(m_pDistortionShader);
    Safe_Release(m_pPostLightShader);
    Safe_Release(m_pVIBuffer);
    

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
