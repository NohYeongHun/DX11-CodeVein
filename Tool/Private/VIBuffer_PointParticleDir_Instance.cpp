CVIBuffer_PointParticleDir_Instance::CVIBuffer_PointParticleDir_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_PointParticleDir_Instance::CVIBuffer_PointParticleDir_Instance(const CVIBuffer_PointParticleDir_Instance& Prototype)
    : CVIBuffer_Instance(Prototype)
    , m_vPivot { Prototype.m_vPivot }
    , m_isLoop { Prototype.m_isLoop }
{
}

#pragma region 기본 함수들
HRESULT CVIBuffer_PointParticleDir_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    const PARTICLEPOINTDIR_INSTANCE_DESC* pPointDirDesc = static_cast<const PARTICLEPOINTDIR_INSTANCE_DESC*>(pDesc);

    // 인스턴스용 변수 채우기.
    m_vPivot = pPointDirDesc->vPivot;
    m_isLoop = pPointDirDesc->isLoop;
    m_vDir = pPointDirDesc->vDir;
    m_vRange = pPointDirDesc->vRange;  // 부모 INSTANCE_DESC에서 가져옴
    m_vLifeTime = pPointDirDesc->vLifeTime; // LifeTime 범위 저장
    m_eParticleType = pPointDirDesc->eParticleType;

    m_iInstanceVertexStride = sizeof(VTXINSTANCEPOINTDIR_PARTICLE);
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


    m_pInstanceVertices = new VTXINSTANCEPOINTDIR_PARTICLE[m_iNumInstance];

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        VTXINSTANCEPOINTDIR_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(m_pInstanceVertices);

        _float		fScale = m_pGameInstance->Rand(pPointDirDesc->vSize.x, pPointDirDesc->vSize.y);
        _float		fLifeTime = m_pGameInstance->Rand(pPointDirDesc->vLifeTime.x, pPointDirDesc->vLifeTime.y);
        _float      fDirSpeed = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);
        //m_pSpeeds[i] = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);

        pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        pInstanceVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
        pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
        pInstanceVertices[i].vDir = _float3(0.f, 0.f, 0.f);
        pInstanceVertices[i].fDirSpeed = fDirSpeed;
        pInstanceVertices[i].iMaskTextureIndex = i % 15; // 0-15 순환
    }

    for (_uint i = 0; i < pPointDirDesc->iNumInstance; i++)
    {
        m_DeadParticleIndices.emplace(i);
    }
#pragma endregion
    return S_OK;
}

HRESULT CVIBuffer_PointParticleDir_Instance::Initialize_Clone(void* pArg)
{
    if (FAILED(CVIBuffer_Instance::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone CVIBuffer_Point_Instance Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVIBuffer_PointParticleDir_Instance::Bind_Resources()
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

HRESULT CVIBuffer_PointParticleDir_Instance::Render()
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);
    return S_OK;
}

void CVIBuffer_PointParticleDir_Instance::Create_Buffer()
{

    D3D11_SUBRESOURCE_DATA	InitialDesc{};
    InitialDesc.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
    {
        CRASH("Faile CVIBuffer_Instance Create Buffer");
        return;
    }
}

// 부모의 트랜스폼 정보를 가져온다.
void CVIBuffer_PointParticleDir_Instance::Bind_Transform(_fvector vPos)
{
    XMStoreFloat4(&m_vParentPos, vPos);
}


#pragma endregion



#pragma region 1. PARITCLE_TYPE에 따른 다른 형태의 파티클 업데이트 결정.
void CVIBuffer_PointParticleDir_Instance::Update(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    auto pInstanceVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(m_pInstanceVertices);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
    auto pVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(SubResource.pData);

    switch (m_eParticleType)
    {
    case PARTICLE_TYPE::PARTICLE_TYPE_DEFAULT:
        Default_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_QUEENKNGIHT_WARP:
        QueenKnightWarp_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_BOSS_EXPLOSION:
        BossExplosion_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_TEST:
        TestParticle_Update(pVertices, fTimeDelta);
        break;
    default:
        break;
    }


    m_pContext->Unmap(m_pVBInstance, 0);


}

void CVIBuffer_PointParticleDir_Instance::Default_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
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
                _float3 initialPos = { pVertices[index].vUp.x, pVertices[index].vUp.y, pVertices[index].vUp.z };
                _float3 direction = pVertices[index].vDir;  // Direction 방향 
                _float speed = pVertices[index].fDirSpeed;

                // burstTime이 0이면 초기 위치는 현재 Translation 위치 사용
                if (burstTime == 0.0f)
                {
                    initialPos = _float3(pVertices[index].vTranslation.x, pVertices[index].vTranslation.y, pVertices[index].vTranslation.z);
                }

                // Direction 방향으로 이동
                _float3 currentPos = {
                    initialPos.x + direction.x * currentTime * speed * fTimeDelta,  // Direction X 방향으로 이동
                    initialPos.y + direction.y * currentTime * speed * fTimeDelta,  // Direction Y 방향으로 이동  
                    initialPos.z + direction.z * currentTime * speed * fTimeDelta  // Direction Z 방향으로 이동
                };

                pVertices[index].vTranslation = _float4(currentPos.x, currentPos.y, currentPos.z, 1.f);
            }
            else if (currentTime < totalLifeTime)
            {
                // 2단계: 터져서 방사형으로 퍼져나감
                _float3 burstDir = { pVertices[index].vLook.x, pVertices[index].vLook.y, pVertices[index].vLook.z };
                _float speed = pVertices[index].fDirSpeed;
                _float burstElapsedTime = currentTime - burstTime;

                // 터진 시점의 위치에서 방사형으로 퍼짐
                _float3 initialPos = { pVertices[index].vUp.x, pVertices[index].vUp.y, pVertices[index].vUp.z };
                _float burstStartY = initialPos.y + burstTime * speed;  // 터진 시점의 높이

                _float3 burstStartPos = { initialPos.x, burstStartY, initialPos.z };

                // 방사형으로 퍼져나가는 위치 계산
                _vector vBurstMove = XMVectorSet(burstDir.x, burstDir.y, burstDir.z, 0.f);
                _vector vBurstStart = XMLoadFloat3(&burstStartPos);

                XMStoreFloat4(&pVertices[index].vTranslation,
                    vBurstStart + vBurstMove * speed * burstElapsedTime * 2.0f);  // 2배 속도로 퍼짐
            }

            ++it;
        }
    }
}

void CVIBuffer_PointParticleDir_Instance::QueenKnightWarp_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{

    // 1. Ready Queue에 있는 파티클을 Live Queue로 한 번에 모두 옮깁니다.
    //    파티클 생성 함수에서 큐에 추가된 파티클들을 업데이트 루프로 가져옵니다.
    while (!m_ReadyparticleIndices.empty())
    {
        auto info = m_ReadyparticleIndices.front();
        m_ReadyparticleIndices.pop();
        _uint index = info.first;
        ParticleVertexInfo particleInfo = info.second;

        // 파티클 초기화
        pVertices[index].vDir = particleInfo.dir;
        pVertices[index].vTranslation = _float4(particleInfo.pos.x, particleInfo.pos.y, particleInfo.pos.z, 1.f);
        pVertices[index].vLifeTime.x = 0.f;
        pVertices[index].vLifeTime.y = particleInfo.lifeTime;
        pVertices[index].fDirSpeed = particleInfo.fRandomSpeed;
        pVertices[index].vRight.w = 1.f;
        pVertices[index].vUp = _float4(particleInfo.initialPos.x, particleInfo.initialPos.y, particleInfo.initialPos.z, 0.f);
        pVertices[index].vLook = _float4(particleInfo.burstDir.x, particleInfo.burstDir.y, particleInfo.burstDir.z, 0.f);

        m_LiveParticleIndices.emplace_back(index);
    }

    // 2. Live Queue에 있는 파티클을 업데이트하고, 수명이 다한 파티클을 제거합니다.
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end(); )
    {
        _uint index = *it;
        pVertices[index].vLifeTime.x += fTimeDelta;

        // 파티클의 수명이 다했는지 확인
        if (pVertices[index].vLifeTime.x >= pVertices[index].vLifeTime.y)
        {
            // 수명이 다한 파티클을 Dead Queue로 이동
            m_DeadParticleIndices.emplace(*it);
            it = m_LiveParticleIndices.erase(it);
        }
        else
        {
            // 파티클 위치 업데이트
            _float3 currentPos = { pVertices[index].vTranslation.x, pVertices[index].vTranslation.y, pVertices[index].vTranslation.z };
            _float3 direction = pVertices[index].vDir;
            _float speed = pVertices[index].fDirSpeed;

            
            // 여기서 fDirSpeed 값이 0인지 확인
            if (fabsf(speed) <= 0.01f)
            {
                // 0이면 디버그 메시지 출력
                OutputDebugWstring(TEXT("Warning: Particle speed is 0. Particle is not moving.\n"));
            }

            _float3 velocity = {
                direction.x * speed * fTimeDelta,
                direction.y * speed * fTimeDelta,
                direction.z * speed * fTimeDelta
            };

            _float3 newPos = {
                currentPos.x + velocity.x,
                currentPos.y + velocity.y,
                currentPos.z + velocity.z
            };

            pVertices[index].vTranslation = _float4(newPos.x, newPos.y, newPos.z, 1.f);

            ++it;
        }
    }

}

void CVIBuffer_PointParticleDir_Instance::BossExplosion_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
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

        // 폭발 정보 저장
        pVertices[index].vRight.w = particleInfo.fBurstTime;     // 폭발 시간
        pVertices[index].vUp = _float4(particleInfo.initialPos.x, particleInfo.initialPos.y, particleInfo.initialPos.z, 0.f);  // 중심점
        pVertices[index].vLook = _float4(particleInfo.burstDir.x, particleInfo.burstDir.y, particleInfo.burstDir.z, 0.f);     // 폭발 방향

        m_LiveParticleIndices.emplace_back(index);
    }

    // 파티클 업데이트
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
            _uint index = *it;
            _float currentTime = pVertices[index].vLifeTime.x;
            _float explosionTime = pVertices[index].vRight.w;
            _float totalLifeTime = pVertices[index].vLifeTime.y;

            _float3 centerPos = { pVertices[index].vUp.x, pVertices[index].vUp.y, pVertices[index].vUp.z };
            _float3 explosionDir = { pVertices[index].vLook.x, pVertices[index].vLook.y, pVertices[index].vLook.z };
            _float baseSpeed = pVertices[index].fDirSpeed;

            _float3 currentPos;

            if (currentTime < explosionTime)
            {
                // 1단계: 응집 단계 - 중심으로 천천히 모여들기
                _float gatherProgress = currentTime / explosionTime; // 0~1
                _float3 initialPos = { pVertices[index].vTranslation.x, pVertices[index].vTranslation.y, pVertices[index].vTranslation.z };
                
                // 처음 위치에서 중심으로 점진적 이동 (자기장에 끌려가는 효과)
                _float attractionForce = 2.0f * gatherProgress; // 점진적으로 증가하는 인력
                _float3 toCenterVector = {
                    centerPos.x - initialPos.x,
                    centerPos.y - initialPos.y,
                    centerPos.z - initialPos.z
                };
                
                currentPos = {
                    initialPos.x + toCenterVector.x * attractionForce * fTimeDelta,
                    initialPos.y + toCenterVector.y * attractionForce * fTimeDelta,
                    initialPos.z + toCenterVector.z * attractionForce * fTimeDelta
                };
            }
            else
            {
                // 2단계: 폭발 단계 - 중심에서 바깥쪽으로 급속 확산
                _float explosionProgress = (currentTime - explosionTime) / (totalLifeTime - explosionTime); // 0~1
                
                // 폭발 속도 (시간에 따라 점진적으로 증가)
                _float explosionSpeed = baseSpeed * (1.0f + explosionProgress * 10.0f); // 최대 11배까지 증가
                
                // 폭발 시작점에서 현재까지의 거리
                _float explosionDistance = explosionSpeed * (currentTime - explosionTime);
                
                // 중심에서 폭발 방향으로 이동
                currentPos = {
                    centerPos.x + explosionDir.x * explosionDistance,
                    centerPos.y + explosionDir.y * explosionDistance,
                    centerPos.z + explosionDir.z * explosionDistance
                };
            }

            pVertices[index].vTranslation = _float4(currentPos.x, currentPos.y, currentPos.z, 1.f);
            ++it;
        }
    }
}

// 테스트용 파티클을 생성해서 업데이트
void CVIBuffer_PointParticleDir_Instance::TestParticle_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
#pragma region 1. 정점 정보 채워주기 

#pragma endregion

    while (!m_ReadyparticleIndices.empty())
    {
        auto info = m_ReadyparticleIndices.front();
        m_ReadyparticleIndices.pop();
        _uint index = info.first;
        ParticleVertexInfo particleInfo = info.second;
        _float3 vStartPos = particleInfo.vStartPos;
        _float3 vParticleDirection = particleInfo.vParticleDir;
        _float  fLifeTime = particleInfo.fLifeTime; // 시간
        _float  fLoss = particleInfo.fLoss;
        _float  fRandomStartSpeed = particleInfo.fRandomStartSpeed;

        // 아직 안들어간 값.
        _float  fAlpha = particleInfo.fAlpha;
        _float  fSize = particleInfo.fSize;
        
        
        // 파티클 초기화
        pVertices[index].vDir = vParticleDirection;
        pVertices[index].vTranslation = _float4(vStartPos.x, vStartPos.y, vStartPos.z, 1.f);
        pVertices[index].vLifeTime.x = 0.f;
        pVertices[index].vLifeTime.y = fLifeTime;
        pVertices[index].fDirSpeed = fRandomStartSpeed;
        pVertices[index].vRight.w = 1.f;
        pVertices[index].vUp = _float4(vStartPos.x, vStartPos.y, vStartPos.z, 0.f);
        pVertices[index].vLook = _float4(vParticleDirection.x, vParticleDirection.y, vParticleDirection.z, 0.f);

        m_LiveParticleIndices.emplace_back(index);
    }

#pragma region 2. 받은 정보를 바탕으로 파티클 업데이트
    // 파티클 업데이트
    auto it = m_LiveParticleIndices.begin();
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end();)
    {

    }
#pragma endregion

    
}

void CVIBuffer_PointParticleDir_Instance::CreateAllParticles(_float3 vCenterPos, _float3 vBaseDir, _float fLifeTime)
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
        _float randomSpeed = 1.0f + static_cast<_float>(rand()) / RAND_MAX * 2.0f;
        info.fRandomSpeed = randomSpeed;

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}

void CVIBuffer_PointParticleDir_Instance::CreateBurstParticles(_float3 vGatherPoint, _float3 vUpDir, _float fGatherTime, _float fBurstTime, _float fTotalLifeTime)
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
        static_cast<_float>(rand() % 200 - 100) / 100.0f,  // -1.0f ~ 1.0fC
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
        _float randomSpeed = 1.0f + static_cast<_float>(rand()) / RAND_MAX * 2.0f;
        info.fRandomSpeed = randomSpeed;

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}


void CVIBuffer_PointParticleDir_Instance::Create_QueenKnightWarpParticle(const PARTICLE_INIT_INFO particleInitInfo)
{
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        // Direction 방향을 축으로 하는 원기둥 형태로 분포
        _float radius = min(m_vRange.x, m_vRange.z);
        _float theta = static_cast<_float>(rand()) / RAND_MAX * 2.0f * XM_PI;
        _float r = static_cast<_float>(rand()) / RAND_MAX * radius;
        _float height = (static_cast<_float>(rand()) / RAND_MAX * 2.0f - 1.0f) * m_vRange.y;

        // Direction 벡터를 정규화
        _vector vDirection = XMVector3Normalize(XMLoadFloat3(&particleInitInfo.dir));
        _float3 normalizedDir;
        XMStoreFloat3(&normalizedDir, vDirection);

        // Direction 벡터에 수직인 두 벡터 구하기 (원기둥의 기저 벡터)
        _float3 up = { 0.0f, 1.0f, 0.0f };
        _vector vUp = XMLoadFloat3(&up);
        _vector vDir = XMLoadFloat3(&normalizedDir);

        // Direction과 Up이 평행한 경우 다른 벡터 사용
        if (abs(XMVectorGetX(XMVector3Dot(vDir, vUp))) > 0.99f)
        {
            up = { 1.0f, 0.0f, 0.0f };
            vUp = XMLoadFloat3(&up);
        }

        // 원기둥의 기저 벡터 계산
        _vector vRight = XMVector3Normalize(XMVector3Cross(vDir, vUp));
        _vector vForward = XMVector3Cross(vDir, vRight);

        // 원기둥 좌표계에서 위치 계산
        _float x = r * cos(theta);
        _float z = r * sin(theta);

        // 기저 벡터를 사용해 월드 좌표로 변환
        _vector vLocalPos = XMVectorSet(x, 0.0f, z, 0.0f);
        _vector vWorldOffset = vRight * x + vForward * z + vDir * height;

        _float3 randomOffset;
        XMStoreFloat3(&randomOffset, vWorldOffset);

        /* vRange 범위 내에서 응집된 시작 위치 설정 */
        _float3 startPos = {
            m_vPivot.x + randomOffset.x,
            m_vPivot.y + randomOffset.y,
            m_vPivot.z + randomOffset.z
        };

        // --- 방향성 스프레드 효과 추가 ---
        // 해결 방법: XMVector3Orthogonal 대신 외적(Cross Product)을 사용합니다.

        // 주 방향과 수직인 임의의 벡터 생성
        _vector vCrossDir = vUp;
        if (XMVector3Equal(vDir, vUp) || XMVector3Equal(vDir, -vUp))
        {
            _vector vAltUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
            vCrossDir = vAltUp;
        }

        // 주 방향에 수직인 두 벡터(직교 벡터)를 구합니다.
        _vector vSpreadRight = XMVector3Normalize(XMVector3Cross(vDir, vCrossDir));
        _vector vSpreadUp = XMVector3Normalize(XMVector3Cross(vDir, vSpreadRight));

        // 2. 퍼짐 정도를 결정하는 각도(라디안)를 설정합니다. (원하는 값으로 조절)
        _float spreadAngle = 0.2f;

        // 3. 무작위 퍼짐 각도를 만듭니다.
        _float randomYaw = (static_cast<_float>(rand()) / RAND_MAX * 2.0f - 1.0f) * spreadAngle;
        _float randomPitch = (static_cast<_float>(rand()) / RAND_MAX * 2.0f - 1.0f) * spreadAngle;

        // 4. 회전 행렬을 생성하고 주 방향 벡터를 회전시켜 최종 방향을 구합니다.
        XMMATRIX rotationMatrix = XMMatrixRotationAxis(vSpreadUp, randomYaw) * XMMatrixRotationAxis(vSpreadRight, randomPitch);
        _vector vFinalDir = XMVector3Normalize(XMVector3Transform(vDir, rotationMatrix));

        _float3 finalDir;
        XMStoreFloat3(&finalDir, vFinalDir);

        ParticleVertexInfo info{};
        info.pos = startPos;
        info.dir = finalDir; // 새로 계산된 퍼진 방향을 사용
        info.initialPos = startPos;
        info.lifeTime = particleInitInfo.lifeTime;

        // 파티클마다 다른 행동 패턴 부여 (burstDir의 w 컴포넌트에 저장)
        // 0.0f = Direction 따라가기, 1.0f = 아래로 떨어지기, 0.5f = 혼합
        _float behaviorType = static_cast<_float>(rand()) / RAND_MAX;
        info.burstDir = _float3(particleInitInfo.dir.x, particleInitInfo.dir.y, particleInitInfo.dir.z);
        info.fBurstTime = behaviorType;
        _float randomSpeed = 1.0f + static_cast<_float>(rand()) / RAND_MAX * 2.0f;
        info.fRandomSpeed = randomSpeed; // <--- VTXINSTANCEDIR_PARTICLE에 speed 변수가 있다면 이 변수를 활용하세요.

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}


void CVIBuffer_PointParticleDir_Instance::Create_BossExplosionParticle(_float3 vCenterPos, _float fRadius, _float fGatherTime, _float fExplosionTime, _float fTotalLifeTime)
{
    // 모든 Dead 파티클을 구형 표면에서 생성
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        // 구형 표면에서 균등하게 분포된 점 생성 (Marsaglia의 방법)
        _float u = static_cast<_float>(rand()) / RAND_MAX * 2.0f - 1.0f; // -1 ~ 1
        _float theta = static_cast<_float>(rand()) / RAND_MAX * 2.0f * XM_PI; // 0 ~ 2π
        _float sqrtOneMinusU2 = sqrtf(1.0f - u * u);
        
        // 구형 표면의 단위 벡터
        _float3 sphereDirection = {
            sqrtOneMinusU2 * cosf(theta),
            u,
            sqrtOneMinusU2 * sinf(theta)
        };
        
        // 구형 표면 위의 초기 위치
        _float3 initialPos = {
            vCenterPos.x + sphereDirection.x * fRadius,
            vCenterPos.y + sphereDirection.y * fRadius,
            vCenterPos.z + sphereDirection.z * fRadius
        };
        
        // 폭발 방향 (중심에서 바깥쪽으로)
        _vector vExplosionDir = XMVector3Normalize(XMLoadFloat3(&sphereDirection));
        _float3 explosionDir;
        XMStoreFloat3(&explosionDir, vExplosionDir);
        
        // 초기 속력 (매우 낮게 시작)
        _float initialSpeed = 0.5f + static_cast<_float>(rand()) / RAND_MAX * 0.5f; // 0.5 ~ 1.0
        
        ParticleVertexInfo info{};
        info.pos = initialPos;
        info.dir = explosionDir; // 폭발 방향
        info.initialPos = vCenterPos; // 중심점 저장
        info.burstDir = explosionDir; // 폭발 방향 저장
        info.fBurstTime = fExplosionTime; // 폭발 시작 시간
        info.lifeTime = fTotalLifeTime; // 전체 생명시간
        _float randomSpeed = 1.0f + static_cast<_float>(rand()) / RAND_MAX * 2.0f;
        info.fRandomSpeed = randomSpeed;
        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}


void CVIBuffer_PointParticleDir_Instance::Create_TestParticle(const PARTICLE_TEST_INFO particleInitInfo)
{

    //_float3 vRange;
    _float  fRadius = particleInitInfo.fRadius;
    _float2 vLifeTime = particleInitInfo.vLifeTime;
    _float3 vParticleDirection = particleInitInfo.vDirection;
    _float2 vSize = particleInitInfo.vSize;
    _float  fAlpha = particleInitInfo.fAlpha;
    _float2  vLoss = particleInitInfo.vLoss;
    _float2 vRandomSpeed = particleInitInfo.vRandomSpeed;


    while (!m_DeadParticleIndices.empty())
    {
#pragma region 1. 사용할 Index를 가져옵니다.

#pragma endregion

        // 1. 사용할 Index를 가져옵니다.
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();


#pragma region 2. 보스의 상대 위치를 가지고 무작위 범위(Radius)의 구체가 생성되게함.
        // 1. 어떻게 만들어주는가?
        // 보스의 위치를 가져온다.
        _vector vParentPos = XMLoadFloat4(&m_vParentPos);

        // -1.f ~ fRadius 랜덤 범위
        _float fRandomX = m_pGameInstance->Rand(-1.f, 1.f);
        _float fRandomY = m_pGameInstance->Rand(-1.f, 1.f);
        _float fRandomZ = m_pGameInstance->Rand(-1.f, 1.f);


        // 0. x, y, z랜덤 방향 구하기.
        _float3 vRandomDir = { fRandomX, fRandomY, fRandomZ };
        _vector vDir = XMVector3Normalize(XMLoadFloat3(&vRandomDir));


        // 1. 첫 번째 단계인 구체의 표면 위에 무작위 점을 만드는 방법
        //먼저, 구체의 표면 위에서 무작위 점을 하나 만듭니다.이 점은 중심에서부터의 거리가 정확히 1.0인 벡터예요.

        //그다음, 이 벡터를 0부터 fRadius 사이의 무작위 값으로 스케일(scale)해 줍니다.이렇게 하면 
        // 파티클의 위치가 구체 안에서 무작위로 결정되죠.
        _float fRandomScale = m_pGameInstance->Rand(0, fRadius);
        vDir = vDir * fRandomScale;
        _vector vStartPos = vDir += vParentPos;

        // 파티클 마다 사라질 시간 지정.
        _float fRandomTime = m_pGameInstance->Rand(vLifeTime.x, vLifeTime.y);
        // 파티클 마다 감쇠 크기 지정.
        _float fLossValue = m_pGameInstance->Rand(vLoss.x, vLoss.y);
        // 입자마다 다른 속도 지정.
        _float fRandomSpeed = m_pGameInstance->Rand(vRandomSpeed.x, vRandomSpeed.y);
        // 입자마다 다른 크기 지정
        _float fSize = m_pGameInstance->Rand(vSize.x, vSize.y);
#pragma endregion

#pragma region 3. 계산한 정보를 바탕으로 ReadyParticle에 넣어줍니다.
       

        /*
        * 1. 생성시간
        * 2. 파티클 방향
        * 3. 감쇠값
        * 4. Random 시작 스피드
        * 5. 시작 알파값.
        * 6. 시작 사이즈.
        */
        ParticleVertexInfo info{};
        XMStoreFloat3(&info.vStartPos, vStartPos); // 시작 위치 지정.
        info.vParticleDir = vParticleDirection; // 방향
        info.fLifeTime = fRandomTime;   // 생성시간
        info.fLoss = fLossValue; // 감쇠 값.
        info.fRandomStartSpeed = fRandomSpeed; // 시작 스피드 (입자마다 다르게 줘야함)
        info.fAlpha = fAlpha;
        info.fSize = fSize;
        m_ReadyparticleIndices.emplace(make_pair(index, info));
#pragma endregion

    }

    
}


#pragma endregion





CVIBuffer_PointParticleDir_Instance* CVIBuffer_PointParticleDir_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
    CVIBuffer_PointParticleDir_Instance* pInstance = new CVIBuffer_PointParticleDir_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_PointParticleDir_Instance"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_PointParticleDir_Instance::Clone(void* pArg)
{
    CVIBuffer_PointParticleDir_Instance* pInstance = new CVIBuffer_PointParticleDir_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_PointParticleDir_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_PointParticleDir_Instance::Free()
{
    CVIBuffer_Instance::Free();
}
