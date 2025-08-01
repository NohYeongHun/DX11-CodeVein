#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationFilePath);
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Update(_fmatrix WorldMatrix);

public:
	_bool isMove(_fvector vPosition);
	_float Compute_Height(_vector vPosition, _float fOffset);
	_vector Get_CurrentCellCenter();

public:
	// 툴 지원 함수들
	HRESULT Add_Cell(_float3 vPointA, _float3 vPointB, _float3 vPointC);
	void Remove_Cell(_int iCellIndex);
	_int Get_CellCount() const;
	_int Find_Cell_By_Position(_float3 vPosition);
	_float3* Get_Cell_Points(_int iCellIndex);

	// 파일 IO 함수들
	HRESULT Export_To_File(const _tchar* pFilePath);
	HRESULT Import_From_File(const _tchar* pFilePath);

	// 셀 간 인접 관계 설정
	void Setup_Adjacent_Cells(_int iCellA, _int iCellB);
	void Setup_Cell_Neighbors();
	_bool Are_Cells_Adjacent(_int iCellA, _int iCellB);

	_wstring Get_Cell_Info(_int iSelectedCellIndex);
#ifdef _DEBUG

public:
	HRESULT Render();

#endif

private:
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;
	_float3							m_vLastNormal = _float3(0.f, 0.f, 0.f);
	static _float4x4				m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };

#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END