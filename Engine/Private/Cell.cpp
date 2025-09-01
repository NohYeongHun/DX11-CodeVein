#include "Cell.h"
CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}



void CCell::Set_PointPos(CELLPOINT ePoint, _float3 vPos)
{
	m_vPoints[ENUM_CLASS(ePoint)] = vPos;
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(CELLPOINT::END));

	_vector		vLine = {};

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]);
	m_vNormals[ENUM_CLASS(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]);
	m_vNormals[ENUM_CLASS(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]);
	m_vNormals[ENUM_CLASS(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));


	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

//#ifdef _DEBUG
//	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
//	if (nullptr == m_pVIBuffer)
//		return E_FAIL;
//#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (_uint i = 0; i < ENUM_CLASS(LINE::END); ++i)
	{
		_vector	vDir = XMVector3Normalize(vPosition - XMVectorSetW(XMLoadFloat3(&m_vPoints[i]), 1.f));
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];

			return false;
		}
	}

	return true;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex, LINE* pOutLine)
{
	for (_uint i = 0; i < ENUM_CLASS(LINE::END); ++i)
	{
		_vector	vDir = XMVector3Normalize(vPosition - XMVectorSetW(XMLoadFloat3(&m_vPoints[i]), 1.f));
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			*pOutLine = static_cast<LINE>(i); // 벗어난 경계선 정보 저장

			return false;
		}
	}

	return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vDestPoint))
			return true;
	}

	return false;
}

_float CCell::Compute_Height(_fvector vLocalPos)
{
	// y = (-ax - cz - d) / b;

	_vector		vPlane = XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), 1.f)
	);

	return (XMVectorGetX(vPlane) * -1.f * XMVectorGetX(vLocalPos) - vPlane.m128_f32[2] * vLocalPos.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

_vector CCell::Get_Center()
{
	_float PositionX = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].x + m_vPoints[ENUM_CLASS(CELLPOINT::B)].x + m_vPoints[ENUM_CLASS(CELLPOINT::C)].x) / 3.f;
	_float PositionY = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].y + m_vPoints[ENUM_CLASS(CELLPOINT::B)].y + m_vPoints[ENUM_CLASS(CELLPOINT::C)].y) / 3.f;
	_float PositionZ = (m_vPoints[ENUM_CLASS(CELLPOINT::A)].z + m_vPoints[ENUM_CLASS(CELLPOINT::B)].z + m_vPoints[ENUM_CLASS(CELLPOINT::C)].z) / 3.f;
	return XMVectorSet(PositionX, PositionY, PositionZ, 1.f);
}

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	return S_OK;
}

//#ifdef _DEBUG
//HRESULT CCell::Render()
//{
//	m_pVIBuffer->Bind_Resources();
//	m_pVIBuffer->Render();
//
//	return S_OK;
//}
//#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	CBase::Free();

//#ifdef _DEBUG
//	Safe_Release(m_pVIBuffer);
//#endif

	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
