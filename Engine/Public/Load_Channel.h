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
	_uint Get_BoneIndex() { return m_iBoneIndex; }

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