#pragma once

#include "VIBuffer.h"

NS_BEGIN(Tool)

class CTool_Mesh final : public CVIBuffer
{
private:
	CTool_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Mesh(const CTool_Mesh& Prototype);
	virtual ~CTool_Mesh() = default;


#pragma region Tool 에서 추출할 Mesh 정보들
public:
	
#pragma endregion



public:
	const _uint Get_MaterialIndex() { return m_iMaterialIndex; }
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const aiMesh* pMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Initialize_Vertex_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize_Vertex_For_Anim(const aiMesh* pAIMesh);

public:
	/* 준비된 자원을 이용하여 어떠한 기능(렌더링)을 수행하기위하여 자원을 장치에 바인딩한다. */
	HRESULT Bind_Resources();
	HRESULT Render();

private:
	vector<_float3> m_vecPositions;
	vector<_uint>   m_vecIndices;

	_uint m_iMaterialIndex = { };
	_uint m_iNumBones = {};

	/* 이 메시에 영향을 주는 뼈들의 인덱스(전체뼈기준) 집합. */
	vector<_int>	m_BoneIndices;


public:
	static CTool_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const aiMesh* pMesh, _fmatrix PreTransformMatrix );
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END