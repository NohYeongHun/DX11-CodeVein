CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}
CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle& Prototype)
    : CVIBuffer(Prototype)
    , m_iMaxNumInstance{ Prototype.m_iMaxNumInstance }
    , m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
    , m_pInstanceVertices{ Prototype.m_pInstanceVertices }
    , m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }
    , m_VBInstanceDesc{ Prototype.m_VBInstanceDesc }
{

}


HRESULT CVIBuffer_Particle::Initialize_Prototype(const PARTICLE_DESC* pDesc)
{
    // 이 단계에서는 GPU 자원을 실제로 생성하지 않고, 
    // 자원을 만들기 위한 설계도(Description)와 공유 데이터만 준비합니다.

#pragma region 0. 기본 소유 정보 초기화
    m_iInstanceVertexStride = sizeof(VTXINSTANCEPOINTDIR_PARTICLE);
    m_iMaxNumInstance = pDesc->iMaxNumInstance;
    m_iNumVertices = 1; // 정점 버퍼는 한개.
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumVertexBuffers = 2; // 버텍스 버퍼의 개수는 2개 (기본, 인스턴싱용)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
#pragma endregion

#pragma region 1. 기본 정점 버퍼 초기화
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

    // 기본 정점 버퍼는 생성해둡니다.
    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
    {
        CRASH("Failed Create Buffer");
        return E_FAIL;
    }

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region 2. 인스턴싱용 정점 버퍼 설계도 생성.
    m_VBInstanceDesc.ByteWidth = m_iMaxNumInstance * m_iInstanceVertexStride;
    m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_VBInstanceDesc.MiscFlags = 0;
    m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

#pragma endregion

#pragma region 3. 정점 에 대한 설계도를 넘겨받아 호출만함.

#pragma endregion


    
    return S_OK;
}


HRESULT CVIBuffer_Particle::Initialize_Clone(void* pArg)
{
    if (FAILED(Create_Buffer()))
    {
        CRASH("FAILED Crash Create Buffer");
        return E_FAIL;
    }

    

    return S_OK;
}

HRESULT CVIBuffer_Particle::Bind_Resources()
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
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT CVIBuffer_Particle::Render()
{
    m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iMaxNumInstance, 0, 0, 0);
    return S_OK;
}

HRESULT CVIBuffer_Particle::Create_Buffer()
{
    D3D11_SUBRESOURCE_DATA	InitialDesc{};
    InitialDesc.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
    {
        CRASH("Faile CVIBuffer_Instance Create Buffer");
        return E_FAIL;
    }
    return S_OK;
}




#pragma region 0. 기본함수들



void CVIBuffer_Particle::Free()
{
    CVIBuffer::Free();

    if (false == m_isCloned)
        Safe_Delete_Array(m_pInstanceVertices);

    Safe_Release(m_pVBInstance);
}

#pragma endregion

