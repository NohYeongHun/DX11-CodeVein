#include "Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice}
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(CELLPOINT::END));

#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex, _float3* vLastNormal)
{
    for (_uint i = 0; i < LINE_END; ++i)
    {
        _vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
        if (0 < XMVectorGetX(XMVector3Dot(vDir, XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])))))
        {
            *pNeighborIndex = m_iNeighborIndices[i];
            if (vLastNormal != nullptr)
                *vLastNormal = m_vNormals[i];
            return false;
        }
        if (vLastNormal != nullptr)
            *vLastNormal = m_vNormals[i];
    }
    return true;
}

_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
{
    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), XMLoadFloat3(pSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), XMLoadFloat3(pDestPoint)))
            return true;

        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), XMLoadFloat3(pDestPoint)))
            return true;
    }

    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), XMLoadFloat3(pSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), XMLoadFloat3(pDestPoint)))
            return true;

        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), XMLoadFloat3(pDestPoint)))
            return true;
    }

    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), XMLoadFloat3(pSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), XMLoadFloat3(pDestPoint)))
            return true;

        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), XMLoadFloat3(pDestPoint)))
            return true;
    }

    return false;
}

_vector CCell::Get_Center()
{
    _float PositionX = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].x + m_vPoints[ENUM_CLASS(CELLPOINT::B)].x + m_vPoints[ENUM_CLASS(CELLPOINT::C)].x) / 3.f;
    _float PositionY = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].y + m_vPoints[ENUM_CLASS(CELLPOINT::B)].y + m_vPoints[ENUM_CLASS(CELLPOINT::C)].y) / 3.f;
    _float PositionZ = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].z + m_vPoints[ENUM_CLASS(CELLPOINT::B)].z + m_vPoints[ENUM_CLASS(CELLPOINT::C)].z) / 3.f;
    return XMVectorSet(PositionX, PositionY, PositionZ, 1.f);
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    return S_OK;
}
#endif


CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
    CCell* pInstance = new CCell(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints)))
    {
        MSG_BOX(TEXT("Failed to Created : CCell"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
