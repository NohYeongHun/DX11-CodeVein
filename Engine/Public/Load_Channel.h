#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Channel :
    public CBase
{

private:
	explicit CLoad_Channel();
	virtual ~CLoad_Channel() = default;


public:
	HRESULT Initialize(std::ifstream& ifs);
	void Update_TransformationMatrix(const vector<class CLoad_Bone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);
	void Update_TransformationMatrixBlend(const vector<class CLoad_Bone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

public:
	/* 보간*/
	_uint Get_BoneIndex() { return m_iBoneIndex; }
	void Reset() { m_iCurrentKeyFrameIndex = 0; }
	_matrix Get_TransformMatrixAtTime(_float fCurrentTrackPosition); // 현재 시간에 맞는 TransformMatrix를 가져옵니다.
	KEYFRAME Get_KeyFrameAtTime(_float fTime);
	_uint Get_NumKeyFrames() { return m_iNumKeyFrames; }
	_vector QuaternionSlerpShortest(_vector q1, _vector q2, _float t);


private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iBoneIndex = {};

	/* 시간에 따른 뼈의 상태를 보관합니다. */
	_uint m_iCurrentKeyFrameIndex = {};
	_uint m_iNumKeyFrames = {};
	vector<KEYFRAME> m_KeyFrames = {};

private:
	HRESULT Load_Channel(std::ifstream& ifs);


public:
	static CLoad_Channel* Create(std::ifstream& ifs);
	virtual void Free();

};
NS_END