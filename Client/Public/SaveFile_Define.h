#pragma once
// 1. Layer에서 첫번째 객체만 저장하고 불러와보기.

// 한 메시가 들고있는 정보.
typedef struct MeshInfoHeader {
    uint32_t iMarterialIndex;
    uint32_t iVertexCount;
    uint32_t iIndicesCount;
    vector<VTXMESH> vertices;
    vector<uint32_t> indices;
}MESH_INFO;

// 한 머테리얼이 들고 있는 정보.
typedef struct MarterialHeader {
    uint32_t materialVectorSize;
    vector<_wstring> materialPathVector;
}MATERIAL_INFO;

// 모델이 들고 있는 정보.
typedef struct ModelInfoHeader
{
    _wstring strModelTag;
    uint32_t meshVectorSize;
    vector<MESH_INFO> meshVector;
    
    uint32_t materialVectorSize;
    vector<MATERIAL_INFO> materialVector;
}MAP_PART_INFO;

// 여러 모델들을 들고 있는 바이너리 파일.


