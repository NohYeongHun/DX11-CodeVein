#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(CELLPOINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ENUM_CLASS(ePoint)]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighborCell) {
		m_iNeighborIndices[ENUM_CLASS(eLine)] = pNeighborCell->m_iIndex;
	}

	_float3	Get_PointPos(CELLPOINT ePoint) const {
		return m_vPoints[ENUM_CLASS(ePoint)]; 
	}

	void Set_PointPos(CELLPOINT ePoint, _float3 vPos);
	

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);

	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_fvector vLocalPos);

public:
	_vector Get_Center();

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

public:
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3					m_vPoints[ENUM_CLASS(CELLPOINT::END)];
	_float3					m_vNormals[ENUM_CLASS(LINE::END)];
	_int					m_iIndex = {};
	_int					m_iNeighborIndices[ENUM_CLASS(LINE::END)] = { -1, -1, -1 };

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };


public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

NS_END