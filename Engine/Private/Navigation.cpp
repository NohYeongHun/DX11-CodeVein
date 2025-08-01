#include "Navigation.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent( Prototype )
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG	
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG	
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int		iNeighborIndex = -1;

	if (m_iCurrentCellIndex == -1)
		return false;

	/* 현재 존재하는 쎌안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex, &m_vLastNormal))
		return true;

	/* 현재 존재하는 쎌을 벗어난다.  */
	else
	{
		/* 나간 방향에 이웃셀이 존재한다. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, &m_vLastNormal))
					break;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}

		/* 나간 방향에 이웃셀이 존재하지않는다. . */
		else
		{
			/*슬라이딩을 위한 리턴을 정의해도 된다. */
			return false;
		}
	}


	return false;
}

_float CNavigation::Compute_Height(_vector vPosition, _float fOffset)
{
	if (m_iCurrentCellIndex == -1)
		return XMVectorGetY(vPosition);


	_float3 cellA = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::A);
	_float3 cellB = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::B);
	_float3 cellC = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::C);

	_vector PointA = XMLoadFloat3(&cellA);
	PointA = XMVectorSetW(PointA, 1.f);
	_vector PointB = XMLoadFloat3(&cellB);
	PointB = XMVectorSetW(PointB, 1.f);
	_vector PointC = XMLoadFloat3(&cellC);
	PointC = XMVectorSetW(PointC, 1.f);

	_vector vPlane = XMPlaneFromPoints(PointA, PointB, PointC);

	_float		fHeight = (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane) + fOffset;

	return fHeight;
}

_vector CNavigation::Get_CurrentCellCenter()
{
	return m_Cells[m_iCurrentCellIndex]->Get_Center();
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render();

	return S_OK;
}

#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG	
	Safe_Release(m_pShader);
#endif
}
