#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Bone final : public CBase
{
private:
	explicit CLoad_Bone();
	virtual ~CLoad_Bone() = default;


public:
	void Update_CombinedTransformMatrix(const vector<CLoad_Bone*>& Bones); // 읽기 전용으로 벡터를 가져와서 부모 본의 변환 행렬과 결합된 본의 변환 행렬을 업데이트합니다.

public:
	HRESULT Initialize(std::ifstream& ifs);


private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentBoneIndex = { -1 }; // 부모 인덱스.

	_float4x4 m_TransformationMatrix = {}; // 본의 변환 행렬.
	_float4x4 m_CombinedTransformationMatrix = {}; // 부모 본의 변환 행렬과 결합된 본의 변환 행렬.

public:
	static CLoad_Bone* Create(std::ifstream& ifs);
	virtual void Free();

};
NS_END

