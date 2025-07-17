#pragma once

#include "VIBuffer.h"

NS_BEGIN(Tool)

class CTool_Mesh final : public CVIBuffer
{
private:
	CTool_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Mesh(const CTool_Mesh& Prototype);
	virtual ~CTool_Mesh() = default;

public:
	const _uint Get_MaterialIndex() { return m_iMaterialIndex; }


public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	/* 준비된 자원을 이용하여 어떠한 기능(렌더링)을 수행하기위하여 자원을 장치에 바인딩한다. */
	HRESULT Bind_Resources();
	HRESULT Render();

private:
	_uint m_iMaterialIndex = { };


public:
	static CTool_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pMesh, _fmatrix PreTransformMatrix );
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END