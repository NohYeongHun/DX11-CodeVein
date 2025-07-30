#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class CLoad_Mesh final : public CVIBuffer
{
private:
	CLoad_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoad_Mesh(const CLoad_Mesh& Prototype);
	virtual ~CLoad_Mesh() = default;


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, _fmatrix PreTransformMatrix, std::ifstream& ifs);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Initialize_Vertex_For_NonAnim(std::ifstream& ifs, _fmatrix PreTransformMatrix);
	HRESULT Initialize_Vertex_For_Anim(std::ifstream& ifs);

public:
	const _uint Get_MaterialIndex() { return m_iMaterialIndex; }
	const _bool Is_Ray_Hit(const _float3& rayOrigin, const _float3& rayDir, _float* pOutDist);



public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CLoad_Bone*>& Bones);

public:
	/* 준비된 자원을 이용하여 어떠한 기능(렌더링)을 수행하기위하여 자원을 장치에 바인딩한다. */
	HRESULT Bind_Resources();
	HRESULT Render();

private:
	vector<_float3> m_vecPositions;
	vector<_uint>   m_vecIndices;

	_char m_szName[MAX_PATH] = {};
	_uint m_iMaterialIndex = { };
	_uint m_iNumBones = {};

	/* 이 메시에 영향을 주는 뼈들의 인덱스(전체뼈기준) 집합. */
	vector<_int>	m_BoneIndices;
	_float4x4		m_BoneMatrices[g_iMaxNumBones] = {}; // 메시에서 사용하는 Bone 행렬.

	vector<_float4x4> m_OffsetMatrices;

public:
	static CLoad_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, _fmatrix PreTransformMatrix, std::ifstream& ifs );
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END