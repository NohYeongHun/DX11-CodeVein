#include "pch.h"
#include "Manager/Renderer/RenderManager.h"

RenderManager::~RenderManager() = default;

void RenderManager::InitializeImpl(const BaseDesc& _desc)
{
    targetManager_ = ServiceLocator::getInstance().get<TargetManager>();
    uint32_t iNumViewports = {1};


    g_context.Get()->RSGetViewports(&iNumViewports, &ViewportDesc);
    targetManager_->Add_RenderTarget(
        TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM,
        XMFLOAT4(1.f, 1.f, 1.f, 0.f));
    targetManager_->Add_RenderTarget(
        TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM,
        XMFLOAT4(1.f, 1.f, 1.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT,
        XMFLOAT4(1.f, 0.f, 0.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM,
        XMFLOAT4(0.f, 0.f, 0.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM,
        XMFLOAT4(1.f, 1.f, 1.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Combine_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM,
        XMFLOAT4(1.f, 1.f, 1.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Post_Light"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM,
        XMFLOAT4(0.f, 0.f, 0.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Post_BlurX"), ViewportDesc.Width / 4.f, ViewportDesc.Height / 4.f, DXGI_FORMAT_R8G8B8A8_UNORM,
        XMFLOAT4(0.f, 0.f, 0.f, 1.f));
    targetManager_->Add_RenderTarget(
        TEXT("Post_BlurY"), ViewportDesc.Width / 4.f, ViewportDesc.Height / 4.f, DXGI_FORMAT_R8G8B8A8_UNORM,
        XMFLOAT4(0.f, 0.f, 0.f, 1.f));


    targetManager_->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"));
    targetManager_->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"));
    targetManager_->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"));
    targetManager_->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"));
    targetManager_->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"));
    targetManager_->Add_MRT(TEXT("MRT_Combine"), TEXT("Combine_Shade"));
    targetManager_->Add_MRT(TEXT("MRT_Post_Light"), TEXT("Post_Light"));
    targetManager_->Add_MRT(TEXT("MRT_Post_BlurX"), TEXT("Post_BlurX"));
    targetManager_->Add_MRT(TEXT("MRT_Post_BlurY"), TEXT("Post_BlurY"));


    vi_buffer_ = VIBufferRect::Create(VIBufferRect::VIBufferRectDesc{});
    ASSERT_CRASH(vi_buffer_);

    Shader::ShaderDesc shader_desc;
    shader_desc.elements = VTXPOSTEX::Elements;
    shader_desc.numElements = VTXPOSTEX::iNumElements;
    shader_desc.shaderFilePath = TEXT("../HLSL/Engine_Shader_Deferred.hlsl");
    shader_ = Shader::Create(shader_desc);
    ASSERT_CRASH(shader_);

    shader_desc.shaderFilePath = TEXT("../HLSL/Shader_Fog.hlsl");
    fog_shader_ = Shader::Create(shader_desc);
    ASSERT_CRASH(fog_shader_);

    shader_desc.shaderFilePath = TEXT("../HLSL/Shader_Extract_BloomBright.hlsl");
    shader_post_light_ = Shader::Create(shader_desc);
    ASSERT_CRASH(shader_post_light_);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    targetManager_->Ready_Debug(
        TEXT("Target_Diffuse"));
    targetManager_->
        Ready_Debug(TEXT("Target_Normal"));
    targetManager_->
        Ready_Debug(TEXT("Combine_Shade"));
    targetManager_->
        Ready_Debug(TEXT("Post_Light"));

    targetManager_->
        Ready_Debug(TEXT("Post_BlurX"));
    targetManager_->
        Ready_Debug(TEXT("Post_BlurY"));
    // targetManager_->
    //     Ready_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f);
    // targetManager_->Ready_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f);
}

void RenderManager::LateUpdate(const float _deltaTime)
{
    for (size_t i = 0; i < RENDER_GROUP_COUNT; i++)
    {
        auto& render_group = renderGroups_[i];
        if (i == static_cast<size_t>(RENDERGROUP::PRIORITY))
        {
            RenderPriorityObjects(render_group);
            render_group.clear();
            continue;
        }
        if (i == static_cast<size_t>(RENDERGROUP::NONBLEND))
        {
            RenderNonBlendObjects(render_group);
            RenderLights();
            RenderCombined();
            RenderPostLight();
            RenderFog();
            render_group.clear();
            continue;
        }
        RenderObjects(render_group);
        render_group.clear();
    }
    Render_Debug();
}

void RenderManager::AddRenderGroup(const RENDERGROUP _renderGroup, shared_ptr<GameObject> _renderObject)
{
    if (nullptr == _renderObject)
        CRASH("_renderObject is nullptr");

    const size_t groupIndex = static_cast<size_t>(_renderGroup);
    renderGroups_[groupIndex].emplace_back(std::move(_renderObject));
}

void RenderManager::RenderObjects(const list<weak_ptr<GameObject>>& _renderGroup)
{
    for (const auto& renderObject : _renderGroup)
    {
        if (const auto sharedRenderObject = renderObject.lock())
        {
            sharedRenderObject->Render();
        }
    }
}

void RenderManager::RenderPriorityObjects(const list<weak_ptr<GameObject>>& _renderGroup)
{
    targetManager_->Begin_MRT(TEXT("MRT_GameObjects"));

    for (const auto& renderObject : _renderGroup)
    {
        if (const auto sharedRenderObject = renderObject.lock())
        {
            sharedRenderObject->Render();
        }
    }
}

void RenderManager::RenderNonBlendObjects(const list<weak_ptr<GameObject>>& _renderGroup)
{
    /* Diffuse + Normal */
    // targetManager_->Begin_MRT(TEXT("MRT_GameObjects"));

    for (const auto& renderObject : _renderGroup)
    {
        if (const auto sharedRenderObject = renderObject.lock())
        {
            sharedRenderObject->Render();
        }
    }
    targetManager_->End_MRT();
}

void RenderManager::RenderLights()
{
    targetManager_->Begin_MRT(TEXT("MRT_LightAcc"));
    shader_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    shader_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    targetManager_->Bind_ShaderResource(
        TEXT("Target_Normal"), shader_, "g_NormalTexture");

    ServiceLocator::getInstance().get<LightManager>()->Render(shader_, vi_buffer_);
    targetManager_->End_MRT();
}

void RenderManager::RenderCombined()
{
    targetManager_->Begin_MRT(TEXT("MRT_Combine"));
    /* 백버퍼 */
    shader_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    shader_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Target_Diffuse"), shader_, "g_DiffuseTexture");

    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Target_Shade"), shader_, "g_ShadeTexture");

    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Target_Emissive"), shader_, "g_EmissiveTexture");
    shader_->Begin(3);
    vi_buffer_->Bind_Resources();
    vi_buffer_->Render();
    targetManager_->End_MRT();
}

void RenderManager::RenderPostLight()
{
    // targetManager_->Add_MRT(TEXT("MRT_Post_Light"), TEXT("Post_Light"));
    // targetManager_->Add_MRT(TEXT("MRT_Post_BlurX"), TEXT("Post_BlurX"));
    // targetManager_->Add_MRT(TEXT("MRT_Post_BlurY"), TEXT("Post_BlurY"));

    // 원래 뷰포트 저장 및 전체 뷰포트로 설정
    D3D11_VIEWPORT originalVP = ViewportDesc;
    g_context.Get()->RSSetViewports(1, &originalVP);

    targetManager_->Begin_MRT(TEXT("MRT_Post_Light"));
    /* 백버퍼 */
    shader_post_light_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    shader_post_light_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_post_light_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    // static float threshold = 0.5f;
    // static float soft = 0.2;
    // shader_post_light_->Bind_RawValue("threshold", &threshold, sizeof(float));
    // shader_post_light_->Bind_RawValue("soft", &soft, sizeof(float));

    // ImGui::Begin("Bloom Settings");
    // ImGui::SliderFloat("threshold", &threshold, 0.f, 1.f);
    // ImGui::SliderFloat("soft", &soft, 0.f, 1.f);
    // ImGui::End();
    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Combine_Shade"), shader_post_light_, "g_DiffuseTexture");
    shader_post_light_->Begin(0);
    vi_buffer_->Bind_Resources();
    vi_buffer_->Render();
    targetManager_->End_MRT();

    // 쿼터 해상도 뷰포트 생성
    D3D11_VIEWPORT quarterVP = ViewportDesc;
    quarterVP.Width = ViewportDesc.Width / 4.f;
    quarterVP.Height = ViewportDesc.Height / 4.f;
    quarterVP.TopLeftX = 0.f;
    quarterVP.TopLeftY = 0.f;
    quarterVP.MinDepth = 0.f;
    quarterVP.MaxDepth = 1.f;

    // Blur X (쿼터 해상도로 렌더)
    g_context.Get()->RSSetViewports(1, &quarterVP);

    targetManager_->Begin_MRT(TEXT("MRT_Post_BlurX"));
    shader_post_light_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    shader_post_light_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_post_light_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Post_Light"), shader_post_light_, "g_DiffuseTexture");
    shader_post_light_->Begin(1);
    vi_buffer_->Bind_Resources();
    vi_buffer_->Render();
    targetManager_->End_MRT();

    // Blur Y (쿼터 해상도로 렌더)
    targetManager_->Begin_MRT(TEXT("MRT_Post_BlurY"));
    shader_post_light_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    shader_post_light_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_post_light_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Post_BlurX"), shader_post_light_, "g_DiffuseTexture");
    shader_post_light_->Begin(2);
    vi_buffer_->Bind_Resources();
    vi_buffer_->Render();
    targetManager_->End_MRT();

    // 원래 뷰포트 복원
    g_context.Get()->RSSetViewports(1, &originalVP);
}


void RenderManager::RenderFog()
{
    /* 백버퍼 */
    fog_shader_->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    fog_shader_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    fog_shader_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Combine_Shade"), fog_shader_, "g_DiffuseTexture");

    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Target_Depth"), fog_shader_, "g_DepthTexture");

    // ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
    //     TEXT("Post_BlurX"), fog_shader_, "g_BloomTextureX");
    ServiceLocator::getInstance().get<TargetManager>()->Bind_ShaderResource(
        TEXT("Post_BlurY"), fog_shader_, "g_BloomTexture");

    fog_shader_->Begin(0);
    vi_buffer_->Bind_Resources();
    vi_buffer_->Render();
}

void RenderManager::Render_Debug()
{
    shader_->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    shader_->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    targetManager_->Render(shader_, vi_buffer_);
}
