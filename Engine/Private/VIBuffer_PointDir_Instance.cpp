CVIBuffer_PointDir_Instance::CVIBuffer_PointDir_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_PointDir_Instance::CVIBuffer_PointDir_Instance(const CVIBuffer_PointDir_Instance& Prototype)
    : CVIBuffer_Instance(Prototype)
    , m_vPivot { Prototype.m_vPivot }
    , m_isLoop { Prototype.m_isLoop }
{
}

HRESULT CVIBuffer_PointDir_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    const POINTDIR_INSTANCE_DESC* pPointDirDesc = static_cast<const POINTDIR_INSTANCE_DESC*>(pDesc);

    // 인스턴스용 변수 채우기.
    m_vPivot = pPointDirDesc->vPivot;
    m_isLoop = pPointDirDesc->isLoop;
    m_vDir = pPointDirDesc->vDir;

    m_iInstanceVertexStride = sizeof(VTXINSTANCEDIR_PARTICLE);
    m_iNumInstance = pPointDirDesc->iNumInstance;

#pragma region 1. 기본 VERTEX 생성
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

    /* 기본 Vtx 생성 => 점 위치 */
    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

    D3D11_SUBRESOURCE_DATA	VBInitialData{};
    VBInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
    {
        CRASH("Failed Create Buffer");
        return E_FAIL;
    }

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region 2. 인스턴싱용 버텍스 생성.
    m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_VBInstanceDesc.MiscFlags = 0;
    m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;


    m_pInstanceVertices = new VTXINSTANCEDIR_PARTICLE[m_iNumInstance];
    //m_pSpeeds = new _float[m_iNumInstance];

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        VTXINSTANCEDIR_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCEDIR_PARTICLE*>(m_pInstanceVertices);

        _float		fScale = m_pGameInstance->Rand(pPointDirDesc->vSize.x, pPointDirDesc->vSize.y);
        _float		fLifeTime = m_pGameInstance->Rand(pPointDirDesc->vLifeTime.x, pPointDirDesc->vLifeTime.y);
        _float      fSpeed = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);
        //m_pSpeeds[i] = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);

        pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        pInstanceVertices[i].vTranslation = _float4( 0.f, 0.f, 0.f, 1.f);
        pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
        pInstanceVertices[i].fSpeed = fSpeed;
    }

    for (_uint i = 0; i < pPointDirDesc->iNumInstance; i++)
    {
        m_DeadParticleIndices.emplace(i);
    }
#pragma endregion
    return S_OK;
}

HRESULT CVIBuffer_PointDir_Instance::Initialize_Clone(void* pArg)
{
    if (FAILED(CVIBuffer_Instance::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone CVIBuffer_Point_Instance Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVIBuffer_PointDir_Instance::Bind_Resources()
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

HRESULT CVIBuffer_PointDir_Instance::Render()
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);
    return S_OK;
}

void CVIBuffer_PointDir_Instance::Update(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    auto pInstanceVertices = static_cast<VTXINSTANCEDIR_PARTICLE*>(m_pInstanceVertices);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
    auto pVertices = static_cast<VTXINSTANCEDIR_PARTICLE*>(SubResource.pData);

    while (!m_ReadyparticleIndices.empty())
    {
        auto info = m_ReadyparticleIndices.front();
        m_ReadyparticleIndices.pop();
        _uint index = info.first;
        ParticleVertexInfo particleInfo = info.second;
        pVertices[index].vDir = particleInfo.dir;
        pVertices[index].vTranslation = _float4(particleInfo.pos.x, particleInfo.pos.y, particleInfo.pos.z, 1.f);
        pVertices[index].vLifeTime.x = 0.f;
        pVertices[index].vLifeTime.y = particleInfo.lifeTime;
        m_LiveParticleIndices.emplace_back(index);
    }

    // 2. LiveParticleIndices에서 객체를 지웁니다.
    auto it = m_LiveParticleIndices.begin();
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end();)
    {
        pVertices[*it].vLifeTime.x += fTimeDelta;
        if (pVertices[*it].vLifeTime.x >= pVertices[*it].vLifeTime.y)
        {
            m_DeadParticleIndices.emplace(*it);
            it = m_LiveParticleIndices.erase(it);
        }
        else
        {
            // Update
            _float3 dir = pVertices[*it].vDir;
            _vector vMoveDir = XMVectorSet(dir.x, dir.y, dir.z, 0.f);

            XMStoreFloat4(&pVertices[*it].vTranslation,
                XMLoadFloat4(&pVertices[*it].vTranslation) + vMoveDir * fTimeDelta);
            ++it;
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);

    
}

void CVIBuffer_PointDir_Instance::Create_Buffer()
{

    D3D11_SUBRESOURCE_DATA	InitialDesc{};
    InitialDesc.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
    {
        CRASH("Faile CVIBuffer_Instance Create Buffer");
        return;
    }
}

void CVIBuffer_PointDir_Instance::PrepareParticle(_float3 vPos, _float3 vDir, _float fLifeTime)
{
    if (m_DeadParticleIndices.empty())
    {
        CRASH("No Dead Particle");
    }
     
    _float3 pointDir = {};

    // 크기는 1로 고정.
    XMStoreFloat3(&pointDir, XMVector3Normalize(XMLoadFloat3(&vDir)));

    _uint index = m_DeadParticleIndices.front();
    m_DeadParticleIndices.pop();
    ParticleVertexInfo info{};
    info.dir = pointDir;
    info.pos = vPos;
    info.lifeTime = fLifeTime;
    m_ReadyparticleIndices.emplace(make_pair(index, info));
}

CVIBuffer_PointDir_Instance* CVIBuffer_PointDir_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
    CVIBuffer_PointDir_Instance* pInstance = new CVIBuffer_PointDir_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_PointDir_Instance"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_PointDir_Instance::Clone(void* pArg)
{
    CVIBuffer_PointDir_Instance* pInstance = new CVIBuffer_PointDir_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_PointDir_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_PointDir_Instance::Free()
{
    CVIBuffer_Instance::Free();
}
