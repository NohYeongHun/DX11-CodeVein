#pragma once

class RenderManager : public IService
{
public:
    ~RenderManager() override;

    void InitializeImpl(const BaseDesc& _desc) override;

    void PriorityUpdate(const float _deltaTime) override
    {
    };

    void Update(const float _deltaTime) override
    {
    };
    void LateUpdate(float _deltaTime) override;

    void AddRenderGroup(RENDERGROUP _renderGroup, shared_ptr<GameObject> _renderObject);

private:
    void RenderObjects(const list<weak_ptr<GameObject>>& _renderGroup);
    void RenderPriorityObjects(const list<weak_ptr<GameObject>>& _renderGroup);
    void RenderNonBlendObjects(const list<weak_ptr<GameObject>>& _renderGroup);
    void RenderLights();
    void RenderCombined();
    void RenderPostLight();
    void RenderFog();
    void Render_Debug();

    shared_ptr<TargetManager> targetManager_;
    array<list<weak_ptr<GameObject>>, RENDER_GROUP_COUNT> renderGroups_;
    shared_ptr<VIBufferRect> vi_buffer_;
    shared_ptr<Shader> shader_;
    shared_ptr<Shader> fog_shader_;
    shared_ptr<Shader> shader_post_light_;
    XMFLOAT4X4 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
    D3D11_VIEWPORT ViewportDesc{};
};
