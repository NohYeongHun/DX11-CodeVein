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
        pInstanceVertices[i].vDir = _float3(0.f, 0.f, 0.f);
        pInstanceVertices[i].fDirSpeed = fSpeed;
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
        
        // 추가 정보를 Right, Up, Look 벡터에 임시 저장 (w 컴포넌트 활용)
        pVertices[index].vRight.w = particleInfo.fBurstTime;     // 터지는 시간
        pVertices[index].vUp = _float4(particleInfo.initialPos.x, particleInfo.initialPos.y, particleInfo.initialPos.z, 0.f);  // 초기 위치
        pVertices[index].vLook = _float4(particleInfo.burstDir.x, particleInfo.burstDir.y, particleInfo.burstDir.z, 0.f);     // 터질 방향
        
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
            // 단계별 파티클 동작
            _uint index = *it;
            _float currentTime = pVertices[index].vLifeTime.x;
            _float burstTime = pVertices[index].vRight.w;  // 터지는 시간
            _float totalLifeTime = pVertices[index].vLifeTime.y;
            
            if (burstTime == 0.0f || currentTime < burstTime)
            {
                // PrepareParticle로 생성된 파티클 또는 1단계: Direction 방향으로 이동
                _float3 initialPos = {pVertices[index].vUp.x, pVertices[index].vUp.y, pVertices[index].vUp.z};
                _float3 direction = pVertices[index].vDir;  // Direction 방향 
                _float speed = pVertices[index].fDirSpeed;
                
                // burstTime이 0이면 초기 위치는 현재 Translation 위치 사용
                if (burstTime == 0.0f)
                {
                    initialPos = _float3(pVertices[index].vTranslation.x, pVertices[index].vTranslation.y, pVertices[index].vTranslation.z);
                }
                
                // Direction 방향으로 이동
                _float3 currentPos = {
                    initialPos.x + direction.x * currentTime * speed,  // Direction X 방향으로 이동
                    initialPos.y + direction.y * currentTime * speed,  // Direction Y 방향으로 이동  
                    initialPos.z + direction.z * currentTime * speed   // Direction Z 방향으로 이동
                };
                
                pVertices[index].vTranslation = _float4(currentPos.x, currentPos.y, currentPos.z, 1.f);
            }
            else if (currentTime < totalLifeTime)
            {
                // 2단계: 터져서 방사형으로 퍼져나감
                _float3 burstDir = {pVertices[index].vLook.x, pVertices[index].vLook.y, pVertices[index].vLook.z};
                _float speed = pVertices[index].fDirSpeed;
                _float burstElapsedTime = currentTime - burstTime;
                
                // 터진 시점의 위치에서 방사형으로 퍼짐
                _float3 initialPos = {pVertices[index].vUp.x, pVertices[index].vUp.y, pVertices[index].vUp.z};
                _float burstStartY = initialPos.y + burstTime * speed;  // 터진 시점의 높이
                
                _float3 burstStartPos = {initialPos.x, burstStartY, initialPos.z};
                
                // 방사형으로 퍼져나가는 위치 계산
                _vector vBurstMove = XMVectorSet(burstDir.x, burstDir.y, burstDir.z, 0.f);
                _vector vBurstStart = XMLoadFloat3(&burstStartPos);
                
                XMStoreFloat4(&pVertices[index].vTranslation,
                    vBurstStart + vBurstMove * speed * burstElapsedTime * 2.0f);  // 2배 속도로 퍼짐
            }
            
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
    info.initialPos = vPos;        // 초기 위치 저장
    info.fBurstTime = 0.0f;        // PrepareParticle은 burstTime 없음
    info.burstDir = _float3(0,0,0); // 기본값
    m_ReadyparticleIndices.emplace(make_pair(index, info));
}

void CVIBuffer_PointDir_Instance::CreateAllParticles(_float3 vCenterPos, _float3 vBaseDir, _float fLifeTime)
{
    // 모든 Dead 파티클을 한번에 활성화
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();
        
        // 각 파티클마다 약간씩 다른 위치와 방향 생성
        _float3 randomOffset = {
            static_cast<_float>(rand() % 200 - 100) / 100.0f * 2.0f,  // -2.0f ~ 2.0f
            static_cast<_float>(rand() % 200 - 100) / 100.0f * 2.0f,
            static_cast<_float>(rand() % 200 - 100) / 100.0f * 2.0f
        };
        
        _float3 randomDir = {
            vBaseDir.x + static_cast<_float>(rand() % 100 - 50) / 100.0f,
            vBaseDir.y + static_cast<_float>(rand() % 100 - 50) / 100.0f,
            vBaseDir.z + static_cast<_float>(rand() % 100 - 50) / 100.0f
        };
        
        _float3 particlePos = {
            vCenterPos.x + randomOffset.x,
            vCenterPos.y + randomOffset.y,
            vCenterPos.z + randomOffset.z
        };
        
        // 방향 정규화
        _vector vNormalizedDir = XMVector3Normalize(XMLoadFloat3(&randomDir));
        _float3 normalizedDir;
        XMStoreFloat3(&normalizedDir, vNormalizedDir);
        
        ParticleVertexInfo info{};
        info.pos = particlePos;
        info.dir = normalizedDir;
        info.lifeTime = fLifeTime + static_cast<_float>(rand() % 100) / 100.0f; // 약간의 생명시간 변화
        
        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}

void CVIBuffer_PointDir_Instance::CreateBurstParticles(_float3 vGatherPoint, _float3 vUpDir, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime)
{
    // 모든 Dead 파티클을 한번에 활성화
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        // 각 파티클마다 다른 초기 위치 (흩어진 시작점)
        _float3 randomStartOffset = {
        static_cast<_float>(rand() % 200 - 100) / 100.0f * 3.0f,  // -3.0f ~ 3.0f
        static_cast<_float>(rand() % 200 - 100) / 100.0f * 3.0f,
        static_cast<_float>(rand() % 200 - 100) / 100.0f * 3.0f
        };

        _float3 startPos = {
        vGatherPoint.x + randomStartOffset.x,
        vGatherPoint.y + randomStartOffset.y,
        vGatherPoint.z + randomStartOffset.z
        };
        // 터질 때의 랜덤 방향 (구형으로 퍼짐)
        _float3 burstDir = {
        static_cast<_float>(rand() % 200 - 100) / 100.0f,  // -1.0f ~ 1.0f
        static_cast<_float>(rand() % 200 - 100) / 100.0f,
        static_cast<_float>(rand() % 200 - 100) / 100.0f
        };
        // 방향 정규화
        _vector vNormalizedBurst = XMVector3Normalize(XMLoadFloat3(&burstDir));
        XMStoreFloat3(&burstDir, vNormalizedBurst);

        // 위쪽 방향 정규화
        _vector vNormalizedUp = XMVector3Normalize(XMLoadFloat3(&vUpDir));
        _float3 upDir;
        XMStoreFloat3(&upDir, vNormalizedUp);

        ParticleVertexInfo info{};
        info.pos = startPos;                    // 시작 위치 (흩어진 곳)
        info.dir = vUpDir;                      // 지정한 Direction 방향으로 이동
        info.initialPos = startPos;             // 초기 위치 저장
        info.burstDir = burstDir;               // 터질 때 방향
        info.fBurstTime = fBurstTime;           // 터지는 시간
        info.lifeTime = fTotalLifeTime;         // 전체 생명시간

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
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
