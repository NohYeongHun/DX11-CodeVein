#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

HRESULT CRenderer::Initialize_Clone()
{
    if (FAILED(Ready_Render_State()))
        return E_FAIL;

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

HRESULT CRenderer::Draw()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;
    if (FAILED(Render_NonBlend()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;
    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Apply_BlendeState()
{
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f }; // 특별한 경우가 아니라면 0
    UINT sampleMask = 0xffffffff;

    m_pContext->OMSetBlendState(m_pAlphaBlend, blendFactor, sampleMask);
    return S_OK;
}

HRESULT CRenderer::Apply_DepthStencilOff()
{
    m_pContext->OMSetDepthStencilState(m_pDepthOff, 0);
    return S_OK;
}

HRESULT CRenderer::Apply_DefaultStates()
{
    m_pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // 기본 블렌드
    m_pContext->OMSetDepthStencilState(m_pDepthOn, 0);
    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
  /*  m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool 
    {
        return static_cast<CBlendObject*>(pSour)->Get_Depth() > static_cast<CBlendObject*>(pDest)->Get_Depth();
    });*/

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].clear();

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

HRESULT CRenderer::Ready_Render_State()
{
    // AlphaBlend 생성 
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlend);

    // Depth Stencil 끄기
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = FALSE;  // 깊이 비교 X
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDepthOff);

    // Depth Stencil 켜기
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS;
    desc.StencilEnable = FALSE;

    m_pDevice->CreateDepthStencilState(&desc, &m_pDepthOn);

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX(TEXT("Failed to Created : CRenderer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRenderer::Free()
{
    __super::Free();

    for (size_t i = 0; i < ENUM_CLASS(RENDERGROUP::END); i++)
    {
        for (auto& pRenderObject : m_RenderObjects[i])
            Safe_Release(pRenderObject);

        m_RenderObjects[i].clear();
    }

    Safe_Release(m_pAlphaBlend);
    Safe_Release(m_pDepthOn);
    Safe_Release(m_pDepthOff);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
