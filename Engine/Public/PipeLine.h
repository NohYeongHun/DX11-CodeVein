#pragma once
#include "Base.h"

/*
* 매 프레임 한번만 연산.
*/
NS_BEGIN(Engine)
class CPipeLine final : public CBase
{
private:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;

#pragma region ENGINE 제공
public:
	_matrix Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

	


public:
	HRESULT Initialize();
	void    Update();

private:
	_float4x4 m_TransformMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4 m_TransformMatrixInverse[ENUM_CLASS(D3DTS::END)] = {};
	_float4 m_vCamPosition = {};


public:
	static CPipeLine* Create();
	virtual void Free();

};
NS_END

