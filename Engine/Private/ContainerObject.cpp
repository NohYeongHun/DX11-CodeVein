#include "ContainerObject.h"


CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
    : CGameObject( Prototype )
{

}

HRESULT CContainerObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CContainerObject::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone");
        return E_FAIL;
    }
        
    return S_OK;
}


void CContainerObject::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    for (auto& pPartObject : m_PartObjects)
        pPartObject.second->Priority_Update(fTimeDelta);
}

void CContainerObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    for (auto& pPartObject : m_PartObjects)
        pPartObject.second->Update(fTimeDelta);

}

void CContainerObject::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    for (auto& pPartObject : m_PartObjects)
        pPartObject.second->Late_Update(fTimeDelta);

}

HRESULT CContainerObject::Render()
{

    return S_OK;
}


HRESULT CContainerObject::Add_PartObject(const _wstring& strPartObjectTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CPartObject** ppPart, void* pArg)
{
    if (nullptr != Find_PartObject(strPartObjectTag))
        return E_FAIL;

    CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
    if (nullptr == pPartObject)
        return E_FAIL;

    *ppPart = pPartObject;
    Safe_AddRef(pPartObject);

    m_PartObjects.emplace(strPartObjectTag, pPartObject);

    return S_OK;
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& strPartObjectTag)
{
    auto    iter = m_PartObjects.find(strPartObjectTag);
    if (iter == m_PartObjects.end())
        return nullptr;

    return iter->second;
}

void CContainerObject::Free()
{
    __super::Free();

    for (auto& Pair : m_PartObjects)
        Safe_Release(Pair.second);

    m_PartObjects.clear();
}


