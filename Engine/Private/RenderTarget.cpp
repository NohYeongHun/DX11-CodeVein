﻿
CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

ID3D11RenderTargetView* CRenderTarget::Get_RTV() const
{
    return m_pRTV;
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    D3D11_TEXTURE2D_DESC		TextureDesc{};

    TextureDesc.Width = iSizeX;
    TextureDesc.Height = iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;
    
    // 1. Render Target에서 생성할 Texture(장면), TargetView(버퍼), SRV(Shader Resource)
    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
    {
        CRASH("Failed Create Texture2D");
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
    {
        CRASH("Failed Create Render Target View");
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
    {
        CRASH("Failed Create Shader Resource View");
        return E_FAIL;
    }

    m_vClearColor = vClearColor;

    return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    // Render Target이 가지고있는 Shader Resource View를 전달받은 매개변수 Shader에 바인딩합니다.
    return pShader->Bind_SRV(pConstantName, m_pSRV);
}

/* RTV View를 Clear() 함. */
void CRenderTarget::Clear()
{
    m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

#ifdef _DEBUG


// 디버깅 용도 함수들.
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(fSizeX, fSizeY, 1.f));
    m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
    m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

    return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
        return E_FAIL;

    pShader->Begin(0);

    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    return S_OK;
}

#endif


CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
    {
        MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    CBase::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pSRV);
    Safe_Release(m_pRTV);
    Safe_Release(m_pTexture2D);
    
    
    
}
