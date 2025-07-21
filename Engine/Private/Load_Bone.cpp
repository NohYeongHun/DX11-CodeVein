#include "Load_Bone.h"
CLoad_Bone::CLoad_Bone()
{
}

HRESULT CLoad_Bone::Initialize(std::ifstream& ifs)
{
    /* 1. Bone의 부모 인덱스. */
    _int iParentBoneIndex = {};
    ifs.read(reinterpret_cast<char*>(&iParentBoneIndex), sizeof(_int));

    m_iParentBoneIndex = iParentBoneIndex;

    // 2. Bone 이름
    string strBoneName = ReadString(ifs);

    // 본 이름 복사.
    strcpy_s(m_szName, strBoneName.c_str());

    // 3. Matrix 복사.
    _float4x4 TransformMatrix;
    ifs.read(reinterpret_cast<char*>(&TransformMatrix), sizeof(_float4x4));

    m_TransformationMatrix = TransformMatrix;
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CLoad_Bone::Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CLoad_Bone*>& Bones)
{
    // 부모 본의 인덱스가 -1이면, 즉 루트 본이면
    if (m_iParentBoneIndex == -1)
    {
        /* 
        * 최상위 부모 Bone에 PreTransform을 곱해줍니다.
        * 자식 객체는 부모 본의 행렬을 거듭해서 곱해지므로 PreTransform이 모두 적용됩니다.
        */
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&PreTransformMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
        return;
    }

    // 부모 본의 결합된 변환 행렬을 가져와서 현재 본의 변환 행렬과 곱합니다.
    XMStoreFloat4x4(&m_CombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

CLoad_Bone* CLoad_Bone::Create(std::ifstream& ifs)
{
    CLoad_Bone* pInstance = new CLoad_Bone();
    if (FAILED(pInstance->Initialize(ifs)))
    {
        MSG_BOX(TEXT("Create Failed : CLoad_Bone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoad_Bone::Free()
{
    __super::Free();
}
