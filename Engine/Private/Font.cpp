﻿#include "Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext { pContext}
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}


HRESULT CFont::Initialize_Clone(const _tchar* pFontFilePath)
{
    m_pFont = new SpriteFont(m_pDevice, pFontFilePath);
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

/* Font 출력. */
HRESULT CFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
    // Draw Call 시작.
    m_pBatch->Begin();

    if (m_pFont)
        m_pFont->DrawString(
            m_pBatch,
            pText,
            vPosition,
            vColor, 
            fRotation, 
            vOrigin,
            fScale
        );

    // Draw Call 끝.
    m_pBatch->End();

    return S_OK;
}


CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
    CFont* pInstance = new CFont(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Clone(pFontFilePath)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Create Failed : CFont"));
    }
    return pInstance;
}


void CFont::Free()
{
    __super::Free();

    if (nullptr != m_pBatch)
        delete m_pBatch;

    if (nullptr != m_pFont)
        delete m_pFont;

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    
}
