#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CNavigationManager final : public CBase
{
private:
	CNavigationManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNavigationManager() = default;

public:
	HRESULT Initialize();
	HRESULT Render();

public:
	_uint	Get_CellSize() { return (_uint)m_Cells.size(); }	//전체 Cell 개수

	_uint	Get_SelectCellIndex() { return m_iSelectCellIndex; }	//선택한 셀 인덱스 번호
	void	Set_SelectCell(_uint iIndex) { m_iSelectCellIndex = iIndex; }

public:
	_uint Get_CellsSize() { return (_uint)m_Cells.size(); }
	CCell* Get_Cell();
	CCell* Get_Cell(_uint iIndex);
	CCell* Find_PickingCell(_vector vPickingPos);
	_float3 Get_ClickedPos() { return m_vClickedPos; }
	_int Get_CurrentCellIndex() { return m_iClickedCellIndex; }
	void Set_CilckedCellIndex(_uint iIndex) { m_iClickedCellIndex = iIndex; }
	

public:
	void Update_ClickedPosition(_float3 vClickedPoint);
	void Clear_ClickedPosition();
	HRESULT Add_Cell(_float3* pPoints, _bool bCheckOverlap);
	void Sort_CellByPosition(_float3* pPoints);
	void Sort_CellByDot(_float3* pPoints);
	_bool Check_Cell(_float3* pPoints);
	void Clear_Cells();
	void Cancle_Cell();
	void Erase_Cell();
	void Click_Position(_vector vPosition);

	_float3 Find_MinDistance(_vector vPosition);

	/* 피킹 점 선택하기.*/
public:
	void Add_Point(_float3 vPos);
	
#pragma region 저장용도

public:
	const vector<class CCell*>& Get_CellContainers();

public:
	void Load_CellContainters(NAVIGATIONSAVE_DESC& naviDesc);
#pragma endregion

private:
	_uint				m_iClickedCellIndex = 0;
	_float3				m_vClickedPos;
	_float				m_fMinDistance = FLT_MAX;
	vector<_float3>		m_vClickedPoints;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CShader* m_pShader = { nullptr };

	vector<class CCell*>	m_Cells;
	vector<_float3>			m_vClickPoints;

	_int					m_iSelectCellIndex = { -1 };
	_float3					m_vClickPos = {};
	//_float					m_fClosetDistance = {};
	//_float3					m_vSelectVertexPos = {};

public:
	static CNavigationManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
NS_END

