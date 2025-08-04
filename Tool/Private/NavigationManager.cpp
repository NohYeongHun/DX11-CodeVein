#include "NavigationManager.h"

CNavigationManager::CNavigationManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice } 
    , m_pContext { pContext }
    , m_pGameInstance{ CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}



HRESULT CNavigationManager::Initialize()
{
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        CRASH("Failed Create Sahder");
    

    return S_OK;
}

HRESULT CNavigationManager::Render()
{

#ifdef _DEBUG
    if (m_Cells.size() == 0)
        return S_OK;

    _float4x4			WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    WorldMatrix._42 += 0.1f;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Search ViewMatrix");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

    _int i = 0;

    
    for (auto& pCell : m_Cells)
    {
        
        if (nullptr != pCell)
        {
            if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            {
                CRASH("Failed Bind Matrix World");
                return E_FAIL;
            }
                

            if (i == m_iSelectCellIndex)
                vColor = _float4(1.f, 0.f, 0.f, 1.f);
            else
                vColor = _float4(0.f, 1.f, 0.f, 1.f);

            m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
            m_pShader->Begin(0);
            pCell->Render();
        }
        i++;
    }

#endif

    return S_OK;
}

// 피킹한 점 선택하기.
void CNavigationManager::Add_Point(_float3 vPos)
{
    _float3 vClickPos = {};

    
}

CNavigationManager* CNavigationManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNavigationManager* pInstance = new CNavigationManager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Create Failed CNavigation Manager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CNavigationManager::Free()
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pShader);
}
