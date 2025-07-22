#pragma once

NS_BEGIN(Engine)
// 1. Layer에서 첫번째 객체만 저장하고 불러와보기.

// 한 머테리얼이 들고 있는 정보. // 공통
typedef struct MarterialHeader {
    uint32_t materialPathVectorSize;
    vector<_wstring> materialPathVector;
}MATERIAL_INFO;

//_float3 vRotation{};    // Euler deg or rad — 툴에서 쓰는 형식
typedef struct TransformHeader {
    _float4 vPosition{};    // (x,y,z)
    _float3 vScale{ 1.f,1.f,1.f };
}TRANSFORM_INFO;

#pragma region NONANIM저장 방식
// 한 메시가 들고있는 정보.
typedef struct MeshInfoHeader {
    uint32_t iMarterialIndex;
    uint32_t iVertexCount;
    uint32_t iIndicesCount;
    vector<VTXMESH> vertices;
    vector<uint32_t> indices;
}MESH_INFO;

// 모델이 들고 있는 정보. => 모델 Prototype 생성시 만듭니다.
typedef struct ModelHeader
{
    _wstring strModelTag;
    uint32_t meshVectorSize;
    vector<MESH_INFO> meshVector;

    uint32_t materialVectorSize;
    vector<MATERIAL_INFO> materialVector;
}MODEL_INFO;
#pragma endregion


#pragma region ANIM 저장 방식.
// 한 메시가 들고있는 정보.
typedef struct AnimMeshInfoHeader {
  
    uint32_t iMarterialIndex;
    uint32_t iVertexCount;
    uint32_t iIndicesCount;
    uint32_t iBoneCount;

    string strName; // Mesh 이름

    vector<VTXANIMMESH> vertices;
    vector<uint32_t> indices;

    /* Bone 정보들 */
    uint32_t BoneIndexVectorSize;
    vector<_int> Boneindices;

    uint32_t OffSetVectorSize;    
    vector<_float4x4> OffsetMatrices;
    
}ANIMMESH_INFO;

typedef struct BoneInfoHeader {
    _int iParentBoneIndex; // Bone 부모 인덱스
    string strName; // Bone 이름
    _float4x4 TransformMatrix; // TransformMatrix
}BONE_INFO;


typedef struct ChannelInfoHeader
{
    string channelName;
    uint32_t iBoneIndex;
    uint32_t iNumKeyFrames;
    vector<KEYFRAME> KeyFrames;
}CHANNEL_INFO;

typedef struct AnimationInfoHeader {
    _float fDuration;
    _float fTickPerSecond;
    _float fCurrentTrackPostion;
    uint32_t iNumChannels; // channel 크기.
    vector<CHANNEL_INFO> Channels;
}ANIMATION_INFO;

// 모델이 들고 있는 정보. => 모델 Prototype 생성시 만듭니다.
typedef struct AnimModelHeader
{
    _wstring strModelTag;
    uint32_t meshVectorSize;
    vector<ANIMMESH_INFO> meshVector;

    uint32_t materialVectorSize;
    vector<MATERIAL_INFO> materialVector;

    uint32_t boneVectorSize;
    vector <BONE_INFO> boneVector;

    uint32_t iCurrentAnimIndex;
    uint32_t iNumAnimations;   // vector 크기.
    vector<ANIMATION_INFO> animationVector;
}ANIMMODEL_INFO;
#pragma endregion


// 맵이 들고 있는 정보.
typedef struct MapPartHeader
{
    _wstring strModelTag;
    uint32_t meshVectorSize;
    vector<MESH_INFO> meshVector;

    uint32_t materialVectorSize;
    vector<MATERIAL_INFO> materialVector;

    TRANSFORM_INFO transformInfo;
}MAP_PART_INFO;


// 여러 모델들을 들고 있는 바이너리 파일.



NS_END