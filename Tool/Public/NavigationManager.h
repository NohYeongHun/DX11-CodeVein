#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CNavigationManager final : public CBase
{
private:
	CNavigationManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNavigationManager() = default;

public:
	_uint	Get_CellSize() { return (_uint)m_Cells.size(); }	//전체 Cell 개수

	_uint	Get_SelectCellIndex() { return m_iSelectCellIndex; }	//선택한 셀 인덱스 번호
	void	Set_SelectCell(_uint iIndex) { m_iSelectCellIndex = iIndex; }



public:
	HRESULT Initialize();
	HRESULT Render();

	/* 피킹 점 선택하기.*/
public:
	void Add_Point(_float3 vPos);
	

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

