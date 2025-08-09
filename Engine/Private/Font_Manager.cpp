
CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext}
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFont_Manager::Initialize()
{
    // Text를 그릴 SpriteBatch 생성.
    return S_OK;
}

#pragma region ENGINE 제공

HRESULT CFont_Manager::Load_Font(const _wstring& strFontID, const _tchar* pFontFilePath)
{
    // 이미 있다면?
    CFont* pFont = Find_Font(strFontID);
    if (nullptr != pFont)
        return E_FAIL;
    
    m_Fonts.emplace(strFontID, CFont::Create(m_pDevice, m_pContext, pFontFilePath));

    return S_OK;
}


HRESULT CFont_Manager::Render_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(pText, vPosition, vColor, fRotation, vOrigin, fScale);
}
#pragma endregion



CFont* CFont_Manager::Find_Font(const _wstring& strFontID)
{
    auto iter = m_Fonts.find(strFontID);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Create Failed : CFont_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont_Manager::Free()
{
    CBase::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    
    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);

    m_Fonts.clear();
    
}
