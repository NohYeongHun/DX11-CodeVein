#include "pch.h"
#include "VIBuffer_Particle_Instance.h"

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& Prototype)
    : CVIBuffer_Instance(Prototype)
    , m_iMaxParticles(Prototype.m_iMaxParticles)
{
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    const PARTICLE_INSTANCE_DESC* pParticleDesc = static_cast<const PARTICLE_INSTANCE_DESC*>(pDesc);
    m_iMaxParticles = pParticleDesc->iMaxParticles;

    m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
    m_iNumInstance = m_iMaxParticles;

    // 기본 정점 생성 (포인트)
    m_iNumVertices = 1;
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumVertexBuffers = 2;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

    D3D11_BUFFER_DESC VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.0f);

    D3D11_SUBRESOURCE_DATA VBInitialData{};
    VBInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
    {
        CRASH("Failed Create Vertex Buffer");
        return E_FAIL;
    }

    Safe_Delete_Array(pVertices);

    // 인스턴스 버퍼 설정
    m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_VBInstanceDesc.MiscFlags = 0;
    m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXINSTANCE_PARTICLE) * m_iNumInstance);

    Create_Buffer();

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Clone(void* pArg)
{
    if (FAILED(CVIBuffer_Instance::Initialize_Clone(pArg)))
    {
        CRASH("Failed Clone CVIBuffer_Particle_Instance");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Bind_Resources()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance,
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride,
    };

    _uint iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Render()
{
    if (m_iCurrentParticleCount > 0)
    {
        m_pContext->DrawInstanced(1, m_iCurrentParticleCount, 0, 0);
    }
    return S_OK;
}

void CVIBuffer_Particle_Instance::Update_Buffer(const vector<Engine::PARTICLE_DATA>& particles, _matrix transformMatrix)
{
    if (particles.empty() || nullptr == m_pVBInstance)
        return;

    m_iCurrentParticleCount = min(static_cast<_uint>(particles.size()), m_iMaxParticles);

    D3D11_MAPPED_SUBRESOURCE SubResource{};
    if (SUCCEEDED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
    {
        auto pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

        for (_uint i = 0; i < m_iCurrentParticleCount; ++i)
        {
            const auto& particle = particles[i];

            // 파티클 크기 계산
            _float currentSize = particle.fStartSize + (particle.fEndSize - particle.fStartSize) * particle.fLifeRatio;

            // Transform 행렬 설정
            pVertices[i].vRight = _float4(currentSize, 0.f, 0.f, 0.f);
            pVertices[i].vUp = _float4(0.f, currentSize, 0.f, 0.f);
            pVertices[i].vLook = _float4(0.f, 0.f, currentSize, 0.f);

            // 최종 변환 행렬 적용
            _vector vPosition = XMLoadFloat3(&particle.vPosition);
            vPosition = XMVector3Transform(vPosition, transformMatrix);
            XMStoreFloat4(&pVertices[i].vTranslation, XMVectorSetW(vPosition, 1.f));

            // 생명주기 정보
            pVertices[i].vLifeTime = particle.vLife;
            pVertices[i].fLifeRatio = particle.fLifeRatio;
        }

        m_pContext->Unmap(m_pVBInstance, 0);
    }
}

void CVIBuffer_Particle_Instance::Create_Buffer()
{
    D3D11_SUBRESOURCE_DATA InitialDesc{};
    InitialDesc.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
    {
        CRASH("Failed Create Instance Buffer");
        return;
    }
}

CVIBuffer_Particle_Instance* CVIBuffer_Particle_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
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
    CVIBuffer_Instance::Free();
}