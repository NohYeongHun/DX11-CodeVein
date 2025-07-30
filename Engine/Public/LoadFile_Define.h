#pragma once

NS_BEGIN(Engine)
// 한 메시가 들고있는 정보.
typedef struct LoadMeshInfoHeader {
    uint32_t iMarterialIndex;
    uint32_t iVertexCount;
    uint32_t iIndicesCount;
    vector<VTXMESH> vertices;
    vector<uint32_t> indices;
}LOAD_MESH_INFO;

// 한 머테리얼이 들고 있는 정보.
typedef struct LoadMarterialHeader {
    uint32_t materialPathVectorSize;
    vector<_wstring> materialPathVector;
}LOAD_MATERIAL_INFO;


//_float3 vRotation{};    // Euler deg or rad — 툴에서 쓰는 형식
typedef struct LoadTransformHeader {
    _float4 vPosition{};    // (x,y,z)
    _float3 vScale{ 1.f,1.f,1.f };
}LOAD_TRANSFORM_INFO;


// 모델이 들고 있는 정보.
typedef struct LoadModelHeader
{
    _wstring strModelTag;
    uint32_t meshVectorSize;
    vector<LOAD_MESH_INFO> meshVector;

    uint32_t materialVectorSize;
    vector<LOAD_MATERIAL_INFO> materialVector;
}LOAD_MODEL_INFO;

NS_END