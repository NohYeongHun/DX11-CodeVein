#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Bone final : public CBase
{
private:
	explicit CLoad_Bone();
	virtual ~CLoad_Bone() = default;


public:
	HRESULT Initialize(std::ifstream& ifs);


public:
	const _char* Get_Name() const {
		return m_szName;
	}

	// 읽기 전용으로 벡터를 가져와서 부모 본의 변환 행렬과 결합된 본의 변환 행렬을 업데이트합니다.
	void Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CLoad_Bone*>& Bones);

public:
	_float4x4* Get_CombinedTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	const _float4x4* Get_CombinedTransformationMatrix_Ptr() const {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_TransformationMatrix() const {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	const _float4x4* Get_TransformationMatrix_Ptr() const {
		return &m_TransformationMatrix;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_CombinedTransformationMatrix(_fmatrix CombinedTransformationMatrix)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedTransformationMatrix);
	}

	_bool Compare_Name(const _char* pName)
	{
		return !strcmp(pName, m_szName);
	}

#pragma region ROOT BONE 표시
public:
	void Set_Root() { m_isRoot = true; };
	_bool Is_Root() { return m_isRoot; }

private:
	_bool m_isRoot = { false };
#pragma endregion


private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentBoneIndex = { -1 }; // 부모 인덱스.

	_float4x4 m_TransformationMatrix = {}; // 본의 변환 행렬.
	_float4x4 m_CombinedTransformationMatrix = {}; // 부모 본의 변환 행렬과 결합된 본의 변환 행렬.

public:
	static CLoad_Bone* Create(std::ifstream& ifs);
	CLoad_Bone* Clone();
	virtual void Free();

};
NS_END

