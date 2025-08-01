#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CCell final : public CBase
{
public:
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum CELLTYPE { DEFAULT, ONLYJUMP, CELL_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* pPoints);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex, _float3* vLastNormal);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);

public:
	const _float3* Get_Point(CELLPOINT ePoint) { return &m_vPoints[ENUM_CLASS(ePoint)]; }
	CELLTYPE		Get_CellType() { return m_eCellType; }
	_vector			Get_Center();
	_int			Get_Index() { return m_iIndex; }
	_float3			Get_PointValue(CELLPOINT ePoint) const { return m_vPoints[ENUM_CLASS(ePoint)]; }
	void			SetUp_Neighbor(LINE eLine, const CCell* pCell) { m_iNeighborIndices[eLine] = pCell->m_iIndex; }
	void			Set_CellType(CELLTYPE eType) { m_eCellType = eType; }

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3				 m_vPoints[ENUM_CLASS(CELLPOINT::END)] = {};
	_float3				 m_vNormals[LINE_END];

	_int				 m_iIndex = 0;
	_int				 m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	CELLTYPE			 m_eCellType = DEFAULT;

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual void Free() override;
};
NS_END

