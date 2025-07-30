#include "Tool_Bone.h"

CTool_Bone::CTool_Bone()
{
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

    //const _float4x4& m = m_TransformationMatrix;
    //std::wstringstream ss;
    //ss  << m._11 << L' ' << m._12 << L' ' << m._13 << L' ' << m._14 << L'\n'
    //    << m._21 << L' ' << m._22 << L' ' << m._23 << L' ' << m._24 << L'\n'
    //    << m._31 << L' ' << m._32 << L' ' << m._33 << L' ' << m._34 << L'\n'
    //    << m._41 << L' ' << m._42 << L' ' << m._43 << L' ' << m._44 << L'\n';
    //
    //_wstring strBoneDebug = ss.str();
    //OutputDebugString(strBoneDebug.c_str());

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    

    // 부모 본 인덱스 설정하기.
    m_iParentBoneIndex = iParentBoneIndex;

    return S_OK;
}

void CTool_Bone::Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CTool_Bone*>& Bones)
{
    // 부모 본의 인덱스가 -1이면, 즉 루트 본이면
    if (m_iParentBoneIndex == -1)
    {
        // 결합된 변환 행렬은 현재 본의 변환 행렬과 동일합니다.
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&PreTransformMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
        return;
    }

    // 부모 본의 결합된 변환 행렬을 가져와서 현재 본의 변환 행렬과 곱합니다.
    XMStoreFloat4x4(&m_CombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

/* Bone 정보 저장. */
void CTool_Bone::Save_Bones(BONE_INFO& BoneInfo)
{
    BoneInfo.iParentBoneIndex = m_iParentBoneIndex;
    BoneInfo.TransformMatrix = m_TransformationMatrix;
    BoneInfo.strName = m_szName;
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

CTool_Bone* CTool_Bone::Clone()
{
    return new CTool_Bone(*this);
}

void CTool_Bone::Free()
{
    __super::Free();
}
