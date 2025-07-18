#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CBone final : public CBase
{
private:
	explicit CBone();
	virtual ~CBone() = default;


public:
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones); // 읽기 전용으로 벡터를 가져와서 부모 본의 변환 행렬과 결합된 본의 변환 행렬을 업데이트합니다.

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);


private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentBoneIndex = { -1 }; // 부모 인덱스.

	_float4x4 m_TransformationMatrix = {}; // 본의 변환 행렬.
	_float4x4 m_CombinedTransformationMatrix = {}; // 부모 본의 변환 행렬과 결합된 본의 변환 행렬.

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	virtual void Free();

};
NS_END

