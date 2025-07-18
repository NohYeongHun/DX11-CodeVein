#include "Tool_Bone.h"

CTool_Bone::CTool_Bone()
{
}

void CTool_Bone::Update_CombinedTransformMatrix(const vector<CTool_Bone*>& Bones)
{
	// 부모 본의 인덱스가 -1이면, 즉 루트 본이면
	if (m_iParentBoneIndex == -1)
	{
		// 결합된 변환 행렬은 현재 본의 변환 행렬과 동일합니다.
		m_CombinedTransformationMatrix = m_TransformationMatrix;
		return;
	}

	// 부모 본의 결합된 변환 행렬을 가져와서 현재 본의 변환 행렬과 곱합니다.
	const CTool_Bone* pParentBone = Bones[m_iParentBoneIndex];
	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&pParentBone->m_CombinedTransformationMatrix) *
		XMLoadFloat4x4(&m_TransformationMatrix));
}

HRESULT CTool_Bone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
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

CTool_Bone* CTool_Bone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
    CTool_Bone* pInstance = new CTool_Bone();
    if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
    {
        MSG_BOX(TEXT("Create Failed : CTool_Bone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTool_Bone::Free()
{
    __super::Free();
}
