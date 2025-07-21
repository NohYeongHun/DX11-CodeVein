#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CTool_Bone final : public CBase
{
private:
	explicit CTool_Bone();
	virtual ~CTool_Bone() = default;


public:
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	_bool Compare_Name(const _char* pName)
	{
		return !strcmp(pName, m_szName);
	}


public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	// 읽기 전용으로 벡터를 가져와서 부모 본의 변환 행렬과 결합된 본의 변환 행렬을 업데이트합니다.
	void Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CTool_Bone*>& Bones);

	void Save_Bones(BONE_INFO& pBoneInfo);

private:
	_char m_szName[MAX_PATH] = {};
	_float4x4 m_TransformationMatrix = {}; // 본의 변환 행렬.
	_float4x4 m_CombinedTransformationMatrix = {}; // 부모 본의 변환 행렬과 결합된 본의 변환 행렬.

	_int m_iParentBoneIndex = { -1 }; // 부모 인덱스.

public:
	static CTool_Bone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	virtual void Free();

};
NS_END

