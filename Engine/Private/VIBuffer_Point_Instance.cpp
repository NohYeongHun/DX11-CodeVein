﻿CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance(Prototype)
	, m_vPivot { Prototype.m_vPivot}
	, m_pSpeeds { Prototype.m_pSpeeds}
	, m_isLoop { Prototype.m_isLoop }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	const POINT_INSTANCE_DESC* pPointDesc = static_cast<const POINT_INSTANCE_DESC*>(pDesc);

	m_vPivot = pPointDesc->vPivot;
	m_isLoop = pPointDesc->isLoop;

	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumInstance = pPointDesc->iNumInstance;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertexBuffers = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;


	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	m_pSpeeds = new _float[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pPointDesc->vSize.x, pPointDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pPointDesc->vLifeTime.x, pPointDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pPointDesc->vSpeed.x, pPointDesc->vSpeed.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(pPointDesc->vCenter.x - pPointDesc->vRange.x * 0.5f, pPointDesc->vCenter.x + pPointDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand(pPointDesc->vCenter.y - pPointDesc->vRange.y * 0.5f, pPointDesc->vCenter.y + pPointDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand(pPointDesc->vCenter.z - pPointDesc->vRange.z * 0.5f, pPointDesc->vCenter.z + pPointDesc->vRange.z * 0.5f),
			1.f
		);

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);

	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(CVIBuffer_Instance::Initialize_Clone(pArg)))
	{
		CRASH("Failed Clone CVIBuffer_Point_Instance Failed");
		return E_FAIL;
	}

	return S_OK;
}

/* CVIBuffer_Point는 Index Buffer를 사용하지 않습니다.*/
HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

/* CVIBuffer_Point는 Index Buffer를 사용하지 않습니다.*/
HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	/*m_pVB->Lock(0, 0, (void**)&pVertex, 0);*/

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_isLoop)
		{
			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
{
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);
}

