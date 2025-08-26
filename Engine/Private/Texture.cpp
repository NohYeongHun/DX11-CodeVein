#include "Texture.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent(Prototype)
    , m_SRVs{ Prototype.m_SRVs }
    , m_iNumTextures{ Prototype.m_iNumTextures }
{
    for (auto& pResource : m_SRVs)
        Safe_AddRef(pResource);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
    m_iNumTextures = iNumTextures;

    _tchar szExt[MAX_PATH] = {};
    
    _wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

    /* D:\Burger\153\Framework\Engine\PrivateBurger\153\Framework\Engine\Private\Texture%d.png*/
    //_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
    /*_tchar szFullPath[MAX_PATH] = {};
        string strPath = strTexturePath.data;
        HRESULT hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);*/

    for (_uint i = 0; i < iNumTextures; i++)
    {
        _tchar szFullPath[MAX_PATH] = {};

        wsprintf(szFullPath, pTextureFilePath, i);
        ID3D11ShaderResourceView* pSRV = { nullptr };
        HRESULT hr = {};

        if (false == lstrcmp(szExt, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
        else if (false == lstrcmp(szExt, TEXT(".tga")))
            return E_FAIL;
        else
            hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }

    return S_OK;
}

HRESULT CTexture::Initialize_Clone(void* pArg)
{
    return S_OK;
}

// Bind_Shader_Resource할 때 Texture Idx 변경 가능.
HRESULT CTexture::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
    if (iTextureIndex >= m_iNumTextures)
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_Shader_Resources(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
    CTexture* pInstance = new CTexture(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
    {
        MSG_BOX(TEXT("Create Failed : CTexture"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CTexture* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTexture"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture::Free()
{
    CComponent::Free();
    for (auto& pResource : m_SRVs)
        Safe_Release(pResource);
}
