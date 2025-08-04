﻿#pragma once

#include "Component.h"


NS_BEGIN(Engine)
/*
* 전용 오프셋(Nav Mesh 위를 올라탈?)
*/
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
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix WorldMatrix);

public:
	_bool isMove(_fvector vPosition);

	
	_vector Compute_OnCell(_fvector vPosition);
	/* 오프셋 만큼 이동 시킴*/
	_vector Compute_OnCell(_fvector vPosition, _float fOffsetY);

	_int Get_CellCount();
	//_float3* Get_Cell_Points(_uint iIndex);
	_int Find_Cell_By_Position(_float3 vPosition);

#ifdef _DEBUG

public:
	HRESULT Render();

#endif

private:
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;

	static _float4x4				m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };

#endif

private:
	void SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END