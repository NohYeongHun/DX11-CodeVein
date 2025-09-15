#include "VIBuffer_PointParticleDir_Instance.h"
CVIBuffer_PointParticleDir_Instance::CVIBuffer_PointParticleDir_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_PointParticleDir_Instance::CVIBuffer_PointParticleDir_Instance(const CVIBuffer_PointParticleDir_Instance& Prototype)
    : CVIBuffer_Instance(Prototype)
    , m_vPivot{ Prototype.m_vPivot }
    , m_isLoop{ Prototype.m_isLoop }
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
    m_vSpeed = pPointDirDesc->vSpeed;
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

    //for (size_t i = 0; i < m_iNumInstance; i++)
    //{
    //    VTXINSTANCEPOINTDIR_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(m_pInstanceVertices);

    //    _float		fScale = m_pGameInstance->Rand(pPointDirDesc->vSize.x, pPointDirDesc->vSize.y);

    //    _float		fLifeTime = m_pGameInstance->Rand(pPointDirDesc->vLifeTime.x, pPointDirDesc->vLifeTime.y);
    //    _float      fDirSpeed = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);
    //    //m_pSpeeds[i] = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);

    //    pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
    //    pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
    //    pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
    //    pInstanceVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
    //    pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
    //    pInstanceVertices[i].vDir = _float3(0.f, 0.f, 0.f);
    //    pInstanceVertices[i].fDirSpeed = fDirSpeed;
    //    pInstanceVertices[i].iMaskTextureIndex = i % 15; // 0-15 순환
    //}

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        VTXINSTANCEPOINTDIR_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCEPOINTDIR_PARTICLE*>(m_pInstanceVertices);

        _float		fScaleX = pPointDirDesc->vSize.x;
        _float		fScaleY = pPointDirDesc->vSize.y;

        _float		fLifeTime = m_pGameInstance->Rand(pPointDirDesc->vLifeTime.x, pPointDirDesc->vLifeTime.y);
        _float      fDirSpeed = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);
        //m_pSpeeds[i] = m_pGameInstance->Rand(pPointDirDesc->vSpeed.x, pPointDirDesc->vSpeed.y);

        pInstanceVertices[i].vRight = _float4(fScaleX, 0.f, 0.f, 0.f);
        pInstanceVertices[i].vUp = _float4(0.f, fScaleY, 0.f, 0.f);
        pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScaleX, 0.f);
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
    //OutputDebugWstring(TEXT("CVIBuffer_PointParticleDir_Instance::Render() - DrawInstanced Called"));
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
    case PARTICLE_TYPE::PARTICLE_TYPE_QUEEN_WARP:
        QueenKnightWarp_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_EXPLOSION:
        Explosion_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_BOSS_EXPLOSION:
        BossExplosion_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_HIT_PARTICLE:
        HitParticle_Update(pVertices, fTimeDelta);
        break;
    case PARTICLE_TYPE::PARTICLE_TYPE_TORNADO:
        Tornado_Update(pVertices, fTimeDelta);
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
#ifdef _DEBUG
    //m_fDebugTime += fTimeDelta;
    //OutputDebugWstring(TEXT("QueenKnight Warp Particle"));
    //OutPutDebugFloat(m_fDebugTime);
#endif



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
                //_float explosionSpeed = baseSpeed * (1.0f + explosionProgress * 10.0f); // 최대 11배까지 증가
                _float explosionSpeed = baseSpeed * (1.0f + explosionProgress); // 최대 11배까지 증가

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


void CVIBuffer_PointParticleDir_Instance::Explosion_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
    // 새로 생성된 파티클을 활성 목록으로 옮기는 부분
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
        pVertices[index].fDirSpeed = particleInfo.fRandomSpeed;
        m_LiveParticleIndices.emplace_back(index);
    }

    // 활성화된 파티클들의 움직임을 계산
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end(); )
    {
        _uint index = *it;
        pVertices[index].vLifeTime.x += fTimeDelta;

        if (pVertices[index].vLifeTime.x >= pVertices[index].vLifeTime.y)
        {
            m_DeadParticleIndices.emplace(*it);
            it = m_LiveParticleIndices.erase(it);
        }
        else
        {
            // [핵심] 단순 직진 운동: 각자 부여받은 방향으로 자신의 속도만큼 일정하게 이동합니다.
            _vector currentPos = XMLoadFloat4(&pVertices[index].vTranslation);
            _vector direction = XMLoadFloat3(&pVertices[index].vDir);
            _float speed = pVertices[index].fDirSpeed;

            _vector newPos = currentPos + (direction * speed * fTimeDelta);

            XMStoreFloat4(&pVertices[index].vTranslation, newPos);
            ++it;
        }
    }
}

// VIBuffer_PointParticleDir_Instance.cpp

void CVIBuffer_PointParticleDir_Instance::HitParticle_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
    // 1. 생성 대기 중인 파티클을 활성 목록으로 이동
    while (!m_ReadyparticleIndices.empty())
    {
        auto info = m_ReadyparticleIndices.front();
        m_ReadyparticleIndices.pop();
        _uint index = info.first;
        ParticleVertexInfo particleInfo = info.second;

        pVertices[index].vTranslation = _float4(particleInfo.pos.x, particleInfo.pos.y, particleInfo.pos.z, 1.f);
        pVertices[index].vDir = particleInfo.dir;
        pVertices[index].vLifeTime.x = 0.f;
        pVertices[index].vLifeTime.y = particleInfo.lifeTime;
        pVertices[index].fDirSpeed = particleInfo.fRandomSpeed;

        m_LiveParticleIndices.emplace_back(index);
    }

    // 2. 활성화된 파티클들의 움직임 계산
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end(); )
    {
        _uint index = *it;
        pVertices[index].vLifeTime.x += fTimeDelta;

        // 수명이 다하면 비활성 목록으로 이동
        if (pVertices[index].vLifeTime.x >= pVertices[index].vLifeTime.y)
        {
            m_DeadParticleIndices.emplace(*it);
            it = m_LiveParticleIndices.erase(it);
        }
        else
        {
            // 단순 직진 운동: 각자 부여받은 방향으로 자신의 속도만큼 이동
            _vector currentPos = XMLoadFloat4(&pVertices[index].vTranslation);
            _vector direction = XMLoadFloat3(&pVertices[index].vDir);
            _float speed = pVertices[index].fDirSpeed;

            _vector newPos = currentPos + (direction * speed * fTimeDelta);

            XMStoreFloat4(&pVertices[index].vTranslation, newPos);
            ++it;
        }
    }
}

void CVIBuffer_PointParticleDir_Instance::Tornado_Update(VTXINSTANCEPOINTDIR_PARTICLE* pVertices, _float fTimeDelta)
{
    // 1. 생성 대기 중인 파티클을 활성 목록으로 이동
    while (!m_ReadyparticleIndices.empty())
    {
        auto info = m_ReadyparticleIndices.front();
        m_ReadyparticleIndices.pop();
        _uint index = info.first;
        ParticleVertexInfo particleInfo = info.second;

        m_LiveParticleIndices.emplace_back(index);
    }

    // 2. 활성화된 파티클들의 토네이도 움직임 계산
    for (auto it = m_LiveParticleIndices.begin(); it != m_LiveParticleIndices.end(); )
    {
        _uint index = *it;
        pVertices[index].vLifeTime.x += fTimeDelta;
        // ...
    }
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


/* 사실상 Activate시 지정해줘야하는 정보. */
void CVIBuffer_PointParticleDir_Instance::Create_QueenKnightWarpParticle_Limited(const PARTICLE_INIT_INFO particleInitInfo, _uint iSpawnCount)
{
    _uint spawnedCount = 0;
    while (!m_DeadParticleIndices.empty() && spawnedCount < iSpawnCount)
    {
        spawnedCount++;
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        // Direction 방향을 축으로 하는 원기둥 형태로 분포
        _float radius = min(m_vRange.x, m_vRange.z); // 처음에 설정했던 range
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
            particleInitInfo.pos.x + randomOffset.x,
            particleInitInfo.pos.y + randomOffset.y,
            particleInitInfo.pos.z + randomOffset.z
        };

        // --- 방향성 스프레드 효과 추가 ---
        // 해결 방법: XMVector3Orthogonal 대신 외적(Cross Product)을 사용합니다.

        // 주 방향과 수직인 임의의 벡터 생성
        up = { 0.0f, 1.0f, 0.0f };
        vUp = XMLoadFloat3(&up);
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
        _float spreadAngle = XMConvertToRadians(30.f);

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
        _float randomSpeed = m_vSpeed.x + static_cast<_float>(rand()) / RAND_MAX * (m_vSpeed.y - m_vSpeed.x);
        info.fRandomSpeed = randomSpeed;

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}

void CVIBuffer_PointParticleDir_Instance::Create_QueenKnightWarpParticle(const PARTICLE_INIT_INFO particleInitInfo)
{
    // 모든 Dead 파티클을 생성 (기존 동작 유지)
    Create_QueenKnightWarpParticle_Limited(particleInitInfo, static_cast<_uint>(m_DeadParticleIndices.size()));
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


void CVIBuffer_PointParticleDir_Instance::Create_ExplosionParticle(_float3 vNomalDir, _float3 vCenterPos, _float fRadius, _float fExplosionTime, _float fTotalLifeTime)
{
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        // 1. 평면 위에서 바깥으로 향하는 무작위 방향 생성 (기존과 동일)
        _vector vPlaneNormal = XMLoadFloat3(&vNomalDir);
        vPlaneNormal = XMVector3Normalize(vPlaneNormal);
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        if (XMVectorGetX(XMVector3Dot(vPlaneNormal, vUp)) > 0.99f)
        {
            vUp = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        }
        _vector vPlaneRight = XMVector3Normalize(XMVector3Cross(vUp, vPlaneNormal));
        _vector vPlaneUp = XMVector3Normalize(XMVector3Cross(vPlaneNormal, vPlaneRight));
        _float randomAngle = static_cast<_float>(rand()) / RAND_MAX * 2.0f * XM_PI;
        _vector vOutwardDir = (vPlaneRight * cosf(randomAngle)) + (vPlaneUp * sinf(randomAngle));
        _float3 outwardDir;
        XMStoreFloat3(&outwardDir, XMVector3Normalize(vOutwardDir));

        _float3 initialPos = vCenterPos;

        // 3. ★파티클 이동 방향★을 중심으로 향하도록 (-1을 곱해) 뒤집어 줌
        _float3 inwardDir = {
            -outwardDir.x,
            -outwardDir.y,
            -outwardDir.z
        };

        ParticleVertexInfo info{};
        info.pos = initialPos;          // 바깥에서 시작
        info.dir = inwardDir;           // 안쪽으로 이동
        info.initialPos = vCenterPos;
        info.burstDir = outwardDir;     // (참고용) 원래 방향 저장
        info.fBurstTime = fExplosionTime;
        info.lifeTime = fTotalLifeTime;
        info.fRandomSpeed = m_pGameInstance->Rand(m_vSpeed.x, m_vSpeed.y);

        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}

void CVIBuffer_PointParticleDir_Instance::Create_HitParticle(_float3 vCenterPos, _float fRadius, _float fLifeTime)
{
    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();


        // 1. 구 형태의 폭발을 위해 랜덤 방향 벡터 생성
        _float3 randomDir = {
            m_pGameInstance->Rand(-1.f, 1.f),
            m_pGameInstance->Rand(-1.f, 1.f),
            m_pGameInstance->Rand(-1.f, 1.f)
        };

        // 2. 방향 벡터 정규화
        _vector vNormalizedDir = XMVector3Normalize(XMLoadFloat3(&randomDir));
        XMStoreFloat3(&randomDir, vNormalizedDir);
     

        // 3. 파티클 정보 설정
        ParticleVertexInfo info{};
        info.pos = vCenterPos;                                  // 시작 위치는 타격 지점
        info.dir = randomDir;                                   // 이동 방향은 랜덤
        info.lifeTime = m_pGameInstance->Rand(m_vLifeTime.x, m_vLifeTime.y); // 수명 랜덤 설정 Pooling시 생성한 설정.
        info.fRandomSpeed = m_pGameInstance->Rand(m_vSpeed.x, m_vSpeed.y); // 속도 랜덤 설정 (멤버 변수 m_vSpeed 사용)

        // 사용하지 않는 정보 초기화
        info.initialPos = vCenterPos;
        info.burstDir = randomDir;
        info.fBurstTime = 0.f; // 즉시 폭발하므로 0

        // 3. 파티클을 생성 대기열에 추가
        m_ReadyparticleIndices.emplace(make_pair(index, info));
    }
}

/*
  1. _float3 vCenterPos - Blood_Pillar의 중심 위치 (기준점)
  2. _float fRadius - 토네이도 회전 반지름
  3. _float fHeight - 토네이도 높이 범위
  4. _float fRotationSpeed - 회전 속도 (라디안/초)
  5. _float fLifeTime - 파티클 생명시간
*/
void CVIBuffer_PointParticleDir_Instance::Create_TornadoParticle(_float3 vCenterPos, _float fRadius, _float fHeight, _float fLifeTime)
{

    while (!m_DeadParticleIndices.empty())
    {
        _uint index = m_DeadParticleIndices.front();
        m_DeadParticleIndices.pop();

        ParticleVertexInfo info{};

        // 1. 수명: 풀링시 설정한 범위 또는 매개변수값 사용
        info.lifeTime = m_pGameInstance->Rand(0.1f, fLifeTime);

        // 2. 시작 각도는 0~360도 사이에서 완전 무작위
        info.fAngle = m_pGameInstance->Rand(0.f, 360.f);

        // 3. 각 파티클의 회전 반지름 (0부터 최대 반지름까지 무작위)
        info.fRadius = m_pGameInstance->Rand(1.f, fRadius);

        // 4. 회전 속도
        info.fAngularSpeed = m_pGameInstance->Rand(m_vSpeed.x, m_vSpeed.y);

        // 5. 상승 속도는 파티클이 자신의 수명 동안 최대 높이에 도달하도록 계산
        info.fUpwardSpeed = fabs(fHeight) / info.lifeTime; // 절댓값을 사용해서 항상 위로 상승

        // 6. 초기 위치 계산 (원형 궤도 상의 위치)
        _float fRadian = XMConvertToRadians(info.fAngle);
        _float fInitialX = vCenterPos.x + info.fRadius * cosf(fRadian);
        _float fInitialZ = vCenterPos.z + info.fRadius * sinf(fRadian);

        // 최종 정보 설정
        info.pos = { fInitialX, vCenterPos.y, fInitialZ };
        info.initialPos = vCenterPos;
        info.dir = { 0.f, 0.f, 0.f };
        info.fHeight = fHeight;
        info.fRandomSpeed = info.fAngularSpeed;

        // 3. 파티클을 생성 대기열에 추가
        m_ReadyparticleIndices.emplace(make_pair(index, info));
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
