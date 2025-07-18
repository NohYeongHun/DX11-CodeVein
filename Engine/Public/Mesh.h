#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	const _uint Get_MaterialIndex() { return m_iMaterialIndex; }


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Initialize_Vertex_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize_Vertex_For_Anim(const aiMesh* pAIMesh);


public:
	/* 준비된 자원을 이용하여 어떠한 기능(렌더링)을 수행하기위하여 자원을 장치에 바인딩한다. */
	HRESULT Bind_Resources();
	HRESULT Render();

private:
	_uint m_iMaterialIndex = { };
	_uint m_iNumBones = {};

	/* 이 메시에 영향을 주는 뼈들의 인덱스(전체뼈기준) 집합. */
	vector<_int>	m_BoneIndices;


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType,  const aiMesh* pMesh, _fmatrix PreTransformMatrix );
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END