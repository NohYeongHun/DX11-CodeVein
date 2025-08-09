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

HRESULT CNavigation::Initialize_Prototype(const _char* pNavigationFilePath)
{
	std::ifstream ifs(pNavigationFilePath, std::ios::binary);
	if (!ifs.is_open())
		CRASH("Load Navigation File Failed");

	NAVIGATIONSAVE_DESC NaviDesc = {};


	// 1. Cell 개수 세오기.
	ifs.read(reinterpret_cast<char*>(&NaviDesc.iCellCount), sizeof(uint32_t));

	// 2. Vector에 값 채우기.
	// 벡터 초기화
	NaviDesc.Cells.resize(NaviDesc.iCellCount);
	ifs.read(reinterpret_cast<char*>(NaviDesc.Cells.data()), NaviDesc.iCellCount * sizeof(CELLSAVE_DESC));
	ifs.close();

	vector<CELLSAVE_DESC>& cellVector = NaviDesc.Cells;

	for (_uint i = 0; i < NaviDesc.iCellCount; ++i)
	{
		_float3 vPointArray[ENUM_CLASS(CELLPOINT::END)] = {
		  cellVector[i].vPointA,
		  cellVector[i].vPointB,
		  cellVector[i].vPointC,
		};

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPointArray, i);
		if (nullptr == pCell)
		{
			CRASH("Failed Load Cell Info");
		}

		m_Cells.emplace_back(pCell);
	}

	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
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
	_vector vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex))
		return true;
	else
	{
		if (-1 != iNeighborIndex)/* 이웃이 있으면 */
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
					break;
			}

			m_iCurrentCellIndex = iNeighborIndex;

			return true;
		}
		else
			/* 이웃이 없으면 */
			return false;
	}
}

// Y오프셋만 계산.
_vector CNavigation::Compute_OnCell(_fvector vPosition)
{
	_vector vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

	vLocalPos = XMVectorSetY(vLocalPos, fHeight);

	return XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));


}

_vector CNavigation::Compute_OnCell(_fvector vPosition, _float fOffsetY)
{
	_vector vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	
	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);
	fHeight += fOffsetY;

	vLocalPos = XMVectorSetY(vLocalPos, fHeight);

	return XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));
}

_int CNavigation::Get_CellCount()
{
	return m_Cells.size();
}


_int CNavigation::Find_Cell_By_Position(_float3 vPosition)
{
	_int index = -1;
	_int iNeighborIndex = -1;

	for (_int i = 0; i < m_Cells.size(); ++i)
	{
		if (m_Cells[i]->isIn(XMLoadFloat3(&vPosition), &iNeighborIndex))
			index = i;
	}

	return index;
}

#pragma region 시작 시 위치 초기화를 위한 값들.
// w값이 -1이면 오류, w 값이 -2면 널포인터
_vector CNavigation::Get_CellPos(_int iIndex)
{
	if (iIndex >= m_Cells.size() || m_Cells.empty() || iIndex < 0)
		return XMVectorSet(-1.f, -1.f, -1.f, -1.f);

	if (nullptr == m_Cells[iIndex])
		return XMVectorSet(-2.f, -2.f, -2.f, -2.f);

	return m_Cells[iIndex]->Get_Center();
}

// Center 기준 비교.
_int CNavigation::Find_NearCellIndex(_float3 vPos)
{

	_vector vOutPos = XMLoadFloat3(&vPos);
	_uint nearIndex = -1;

	_float fMinDist = FLT_MAX;
	_float fDist = { 0.f };
	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		// 빈 셀벡터는 넘어갑니다.
		if (nullptr == m_Cells[i])
			continue;
		_vector vCenter = m_Cells[i]->Get_Center();

		fDist = XMVectorGetX(XMVector3Length(vCenter - vOutPos));
		if (fDist < fMinDist)
		{
			fMinDist = fDist;
			nearIndex = i;
		}
	}

	return nearIndex;
}
#pragma endregion


#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_float4		vColor = {};

	_float4x4	WorldMatrix = m_WorldMatrix;

	if (-1 == m_iCurrentCellIndex)
	{
		vColor = _float4(0.f, 1.f, 0.f, 1.f);

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}

	else
	{
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

		WorldMatrix._42 += 0.1f;

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}



	return S_OK;
}

#endif

void CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::A), pSourCell->Get_Point(CELLPOINT::B)))
			{
				pSourCell->Set_Neighbor(LINE::AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::B), pSourCell->Get_Point(CELLPOINT::C)))
			{
				pSourCell->Set_Neighbor(LINE::BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::C), pSourCell->Get_Point(CELLPOINT::A)))
			{
				pSourCell->Set_Neighbor(LINE::CA, pDestCell);
			}
		}
	}
}

//CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
//{
//	CNavigation* pInstance = new CNavigation(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
//	{
//		MSG_BOX(TEXT("Failed to Created : CNavigation"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}


CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pNavigationFilePath)
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

	if (FAILED(pInstance->Initialize(pArg)))
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
