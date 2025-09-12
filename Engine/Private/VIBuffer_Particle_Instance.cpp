CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Particle{ pDevice, pContext }
{
}

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& Prototype)
    : CVIBuffer_Particle(Prototype)
{
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Prototype(const PARTICLE_INSTANCE_DESC* pDesc)
{

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

        

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Clone(void* pArg)
{
    // 프로토타입의 m_InstanceVBDesc를 사용해 이 클론만의 고유한 인스턴스 버퍼를 생성
    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, nullptr, &m_pVBInstance)))
    {
        CRASH("Failed Create Instance Buffer for Clone");
        return E_FAIL;
    }
    m_iNumRenderInstance = 0; // 렌더링할 개수 초기화
    return S_OK;

}

HRESULT CVIBuffer_Particle_Instance::Bind_Resources()
{
   /* if (FAILED(CVIBuffer_Particle::Bind_Resources()))
    {
        CRASH("FAILED Bind_Resources");
        return E_FAIL;
    }*/

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

HRESULT CVIBuffer_Particle_Instance::Render()
{
    /*if (FAILED(CVIBuffer_Particle::Render()))
    {
        CRASH("FAILED Render");
        return E_FAIL;
    }*/
    //m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumRenderInstance, 0, 0, 0);
    m_pContext->DrawInstanced(m_iNumIndexPerInstance, m_iNumRenderInstance, 0, 0);

    return S_OK;
}

/* Particle 데이터를 바탕으로 데이터 지정. */
void CVIBuffer_Particle_Instance::Update_Buffer(const vector<PARTICLE_DATA>& particles, const _matrix& finalTransformMatrix)
{
    // GPU 버퍼를 잠그고, GPU 메모리를 직접 가리키는 포인터 pVertices를 얻어옵니다.
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
    VTXINSTANCEPOINTDIR_PARTICLE* pVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(SubResource.pData);

    for (size_t i = 0; i < particles.size(); ++i)
    {
        XMVECTOR vLocalPos = XMLoadFloat3(&particles[i].vPosition);
        XMVECTOR vFinalWorldPos = XMVector3TransformCoord(vLocalPos, finalTransformMatrix);

        // CPU 데이터(PARTICLE_DATA)를 GPU 데이터(VTXPOINTDIRPARTICLE) 형식에 맞게 복사
        XMStoreFloat4(&pVertices[i].vTranslation, vFinalWorldPos); // 최종 변환된 위치
        pVertices[i].vLifeTime = particles[i].vLife;
        pVertices[i].vDir = particles[i].vDir;
        pVertices[i].fDirSpeed = particles[i].fSpeed;

        // Right, Up, Look은 셰이더에서 빌보드 등을 위해 사용. 지금은 크기 정보만 넘겨줌.
        _float fSize = particles[i].fCurrentSize;
        pVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
        pVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
        pVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
    }

    m_pContext->Unmap(m_pVBInstance, 0);
    m_iNumRenderInstance = static_cast<_uint>(particles.size());
}

CVIBuffer_Particle_Instance* CVIBuffer_Particle_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_INSTANCE_DESC* pDesc)
{
    CVIBuffer_Particle_Instance* pInstance = new CVIBuffer_Particle_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Particle_Instance"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Particle_Instance::Clone(void* pArg)
{
    CVIBuffer_Particle_Instance* pInstance = new CVIBuffer_Particle_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Particle_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Instance::Free()
{
    CVIBuffer_Particle::Free();

}
