#include "Texture_Manager.h"
CTexture_Manager::CTexture_Manager()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTexture_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_pTextures = new TEXTURES[iNumLevels];

    return S_OK;
}

#pragma region ENGINE APPLY.
HRESULT CTexture_Manager::Add_Texture(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strTextureTag)
{
    if (nullptr != Find_Texture(iLevelIndex, strTextureTag))
        return E_FAIL;

    CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT
        , iLevelIndex
        , strPrototypeTag, nullptr));

    if (nullptr == pTexture)
        return E_FAIL;

    // Clone 객체는 새로운 객체를 생성하는거라 Reference가 존재하지 않음.
    m_pTextures[iLevelIndex].emplace(strTextureTag, pTexture);

    return S_OK;
}

/* GameObject에 Texture를 교체 해준다. */
// GameObject 주소, 멤버 변수 주소, LevelIndex, TextureTag 필요.
void CTexture_Manager::Change_Texture_ToGameObject(CGameObject* pGameObject, const _wstring& strComponentTag, CComponent** ppOut, _uint iLevelIndex, const _wstring& strTextureTag)
{
    CTexture* pFindTexture = Find_Texture(iLevelIndex, strTextureTag);
    if (nullptr == pFindTexture)
        return;

    pGameObject->Change_Component(strComponentTag, ppOut, pFindTexture);
}

void CTexture_Manager::Clear(_uint iLevelIndex)
{
    if (iLevelIndex >= m_iNumLevels)
        return;

    for (auto& Pair : m_pTextures[iLevelIndex])
        Safe_Release(Pair.second);

    m_pTextures[iLevelIndex].clear();
}
#pragma endregion



CTexture* CTexture_Manager::Find_Texture(_uint iLevelIndex, const _wstring& strTextureTag)
{
    if (iLevelIndex >= m_iNumLevels)
        return nullptr;

    auto iter = m_pTextures[iLevelIndex].find(strTextureTag);

    if (iter == m_pTextures[iLevelIndex].end())
        return nullptr;


    return iter->second;
}

CTexture_Manager* CTexture_Manager::Create(_uint iNumLevels)
{
    CTexture_Manager* pInstance = new CTexture_Manager();
    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX(TEXT("Create Failed : CTexture Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pTextures[i])
            Safe_Release(Pair.second);
        m_pTextures[i].clear();
    }

    Safe_Delete_Array(m_pTextures);
}
