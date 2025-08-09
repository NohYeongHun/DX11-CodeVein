#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
    if (nullptr == pAINode)
    {
        MSG_BOX(TEXT("Bone Initialize Failed"));
        return E_FAIL;
    }

    // 본 이름 Copy
    strcpy_s(m_szName, pAINode->mName.data);

    // 현재 본에 행렬 정보 저장
    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

    // Col Major에서 Row Major로 변경 => DirectX에서는 Row Major를 사용합니다.
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    // 부모 본 인덱스 설정하기.
    m_iParentBoneIndex = iParentBoneIndex;

    return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    if (-1 == m_iParentBoneIndex)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

    else
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) *
            XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
    }
}



CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
    CBone* pInstance = new CBone();
    if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
    {
        MSG_BOX(TEXT("Create Failed : CBone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Clone()
{
    return new CBone(*this);
}


void CBone::Free()
{
    CBase::Free();
}
